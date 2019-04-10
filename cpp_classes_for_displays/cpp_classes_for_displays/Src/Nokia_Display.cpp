//
// Created by popenyuk on 3/10/19.
//

#include <Nokia_Display.h>

void Nokia_Display::Init(){
    SPI_enable(&hw_conf);
    LCD5110_CE_off(&hw_conf);

    //LCD5110_VCC_on();
    LCD5110_RST_off(&hw_conf); // Minimum 100 ns, maximum not limited (tbl. 12 AC CHARACTERISTICS, pic. 16)
    volatile int i = 100; // HAL_Delay() too slow, do not want to depend on some delay_us here.
    while (--i){}
    LCD5110_RST_on(&hw_conf);

    LCD5110_DC_off(&hw_conf); // Commands mode on
    //! Extended commands (bit H==1), horizontal addressing
    LCD5110_set_function(LCD5110_FN_SET_H_MASK, &hw_conf);
    //! Set display voltage
    LCD5110_set_voltage_ext(0x40, &hw_conf);
    //! Set temperature coefficient
    LCD5110_set_temp_coef_ext(2, &hw_conf);
    //! Set bias
    LCD5110_set_bias_ext(3, &hw_conf);


    //! Basic commands (bit H==0), horizontal addressing
    LCD5110_set_function(0, &hw_conf);
    //! Set display mode
    LCD5110_set_mode_base(LCD5110_NORMAL_MODE , &hw_conf);
}

Nokia_Display::Nokia_Display(SPI_HandleTypeDef* hspi2_var, uint16_t cs_pin, GPIO_TypeDef *cs_gpio_port, uint16_t rst_pin, GPIO_TypeDef *rst_port, uint16_t dc_pin, GPIO_TypeDef *dc_port, uint16_t blacklight_pin, GPIO_TypeDef *blacklight_port) {
    hw_conf.spi_handle = hspi2_var;
    hw_conf.spi_cs_pin = cs_pin;
    hw_conf.spi_cs_port = cs_gpio_port;
    hw_conf.rst_pin = rst_pin;
    hw_conf.rst_port = rst_port;
    hw_conf.dc_pin = dc_pin;
    hw_conf.dc_port = dc_port;
    hw_conf.blacklight_pin = blacklight_pin;
    hw_conf.blacklight_port = blacklight_port;
    set_x_y_nullspace(5, 0);
    def_scr = {
        { 0, 0, LCD_WIDTH, LCD_HEIGHT },     //	frame
        { 0, 0}, 							 // cursor
        true							     // Refresh on endl
    };
    Init();
}

void Nokia_Display::clear() {
    for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT/8); ++i) {
        hw_conf.video_buffer[i] = 0;
	}
    LCD5110_wendl();
}

void Nokia_Display::turn_on() {
    Init();
    HAL_GPIO_WritePin(hw_conf.blacklight_port, hw_conf.blacklight_pin, GPIO_PIN_SET);
    set_backlight(true);
}

void Nokia_Display::turn_off() {
    HAL_GPIO_WritePin(hw_conf.rst_port, hw_conf.rst_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(hw_conf.blacklight_port, hw_conf.blacklight_pin, GPIO_PIN_RESET);
    set_backlight(false);
}

void Nokia_Display::change_backlight(bool state) {
    set_backlight(state);
    if(state) {
        HAL_GPIO_WritePin(hw_conf.blacklight_port, hw_conf.blacklight_pin, GPIO_PIN_SET);
    }else{
        HAL_GPIO_WritePin(hw_conf.blacklight_port, hw_conf.blacklight_pin, GPIO_PIN_RESET);
    };
}

void Nokia_Display::send_data_to_print(int row, int column, char str) {
    if ((&def_scr)->cursor.y < 0 || (&def_scr)->cursor.x < 0)
        return;
    if ( (&def_scr)->cursor.x + base_font_width > (&def_scr)->frame.width ||
         (&def_scr)->cursor.y + base_font_height > (&def_scr)->frame.height )
        return;
//! TODO: Print part of char
    const unsigned char *fontpointer = base_font;
    LCD5110_drawBitmap(row + (&def_scr)->frame.x0, column + (&def_scr)->frame.y0, fontpointer + str * base_font_width,
                       base_font_width, base_font_height, 0);
    if(is_printed){
        LCD5110_wendl();
    }
    is_printed = true;

}

void Nokia_Display::send_data_to_print(int row, int column, char *str) {
    (&def_scr)->cursor.x = row;
    (&def_scr)->cursor.y= column;
    while (*str != 0) {
        if (*str != '\n') {
            is_printed = false;
            send_data_to_print((&def_scr)->cursor.x, (&def_scr)->cursor.y, *str);
        }
        (&def_scr)->cursor.x += base_font_width;

        if ((((&def_scr)->cursor.x + base_font_width > (&def_scr)->frame.width ) &&
             *(str + 1) != '\n') || *str == '\n')
        {
            LCD5110_wendl();
            if( isspace(*(str + 1)) )
                ++str;
        }
        if ((&def_scr)->cursor.y + base_font_height > (&def_scr)->frame.height)
            break;
        str++;

    }
}

void Nokia_Display::LCD5110_wendl() {
    (&def_scr)->cursor.y += base_font_height + 1;
    (&def_scr)->cursor.x = 2;
    if ((&def_scr)->refresh_on_endl) {
        LCD5110_CE_off(&this->hw_conf);
        LCD5110_DC_off(&this->hw_conf);
        LCD5110_set_XY_base(0, 0, &this->hw_conf);
        HAL_GPIO_WritePin(this->hw_conf.dc_port, this->hw_conf.dc_pin, GPIO_PIN_SET);
        send_data_to_LCD5110(this->hw_conf.video_buffer, LCD_HEIGHT * LCD_WIDTH / 8, &(this->hw_conf));
    }
}

void Nokia_Display::LCD5110_drawBitmap(int x, int y, const unsigned char *bitmap, int cols, int rows, int invert) {
    for(int cx=0; cx<cols; cx++)
    {
        for (int cy=0; cy<rows; cy++)
        {
            unsigned char temp = bitmap[(cy/8)*cols + cx];
            temp &= 1<<(cy%8);
            LCD5110_putpix(x+cx, y+cy, invert != 0 == !temp);
        }
    }
}

void Nokia_Display::LCD5110_putpix(int x, int y, int color) {
    if ( (x < 0) || (y < 0) || (x > LCD_WIDTH-1) || (y > LCD_HEIGHT-1))
        return;
    pix_coord  c = xy_to_pix(x, y);

    if (color)
        this->hw_conf.video_buffer[c.adr] |= c.mask;
    else
        this->hw_conf.video_buffer[c.adr] &= ~c.mask;
}
