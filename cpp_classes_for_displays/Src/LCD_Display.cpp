//
// Created by Yevhenii on 08/03/2019.
//

#include "../Inc/LCD_Display.h"

HAL_StatusTypeDef LCD_Display::LCD_SendInternal(uint8_t data, uint8_t flags) {
    /* Functon that send data to display. At start it check if there connection with display.
     * Then form data to send. And at the end it send data with help of build Hal function. */
    HAL_StatusTypeDef res;
    for (;;) {
        res = HAL_I2C_IsDeviceReady(&I2C_variable, LCD_ADDR, 1, HAL_MAX_DELAY);
        if (res == HAL_OK)
            break;
    }

    uint8_t up = (uint8_t)(data & 0xF0);
    uint8_t lo = (uint8_t)((data << 4) & 0xF0);

    uint8_t data_arr[4];
    data_arr[0] = up | flags | LIGHT | PIN_EN;
    data_arr[1] = up | flags | LIGHT;
    data_arr[2] = lo | flags | LIGHT | PIN_EN;
    data_arr[3] = lo | flags | LIGHT;

    res = HAL_I2C_Master_Transmit(&I2C_variable, LCD_ADDR, data_arr,
                                  sizeof(data_arr), HAL_MAX_DELAY);
    HAL_Delay(LCD_DELAY_MS);
    return res;
}

void LCD_Display::search_I2C_bus(I2C_HandleTypeDef i2c){
    // This function scan I2C bus
    // It need semihosting or another tool for printf
    printf("Scanning I2C bus:\r\n");
    HAL_StatusTypeDef result;
    uint8_t i;
    for (i=1; i<128; i++)
    {
        result = HAL_I2C_IsDeviceReady(&I2C_variable, LCD_ADDR, 1, HAL_MAX_DELAY);
        if (result != HAL_OK)
        {
            printf(".");
        }
        if (result == HAL_OK)
        {
            printf("0x%X", i);
        }
    }
    printf("\r\n");
}

void LCD_Display::search_I2C_bus_without_semihosting(I2C_HandleTypeDef i2c_Variable){
    // This function scan I2C bus and print address on display
    // You can work only with one I2C controller
    HAL_StatusTypeDef result;
    uint8_t i;
    LCD_Display* new_lcd;
    for (i=1; i<128; i++)
    {
        result = HAL_I2C_IsDeviceReady(&i2c_Variable, (uint16_t)(i<<1), 2, 2);
        if (result == HAL_OK)
        {
            new_lcd = new LCD_Display(i<<1, i2c_Variable);
            char* str {"Adress is: 0x"};
            new_lcd->send_data_to_print(0, 0, str);
//            new_lcd->send_data_to_print(0, 13, i, 16);
            new_lcd->print();
        }
    }
    printf("\r\n");
}

void LCD_Display::LCD_SendCommand(uint8_t cmd) {
    /* Send command to display. Pin Rs is 0 */
    LCD_SendInternal(cmd, 0);
}

void LCD_Display::LCD_SendData(uint8_t data) {
    /* Send command to display. Pin Rs is 0 */
    LCD_SendInternal(data, PIN_RS);
}

/* Initialise i2c display. Initialise i2c variable. Clear array. */
LCD_Display::LCD_Display(uint8_t lcd_addr, I2C_HandleTypeDef i2c_variable) {
    set_x_y_nullspace(0, 1);
    I2C_variable = i2c_variable;
    LCD_ADDR = lcd_addr;
    // 4-bit mode, 2 lines, 5x7 format
    LCD_SendCommand(0b00110000);
    // display & cursor home (keep this!)
    LCD_SendCommand(0b00000010);
    // display on, right shift, underline off, blink off
    LCD_SendCommand(0b00001100);
    // clear display (optional here)
    LCD_SendCommand(0b00000001);
    clear();
}

void LCD_Display::turn_on() {
    LIGHT = BackLight;
    change_enable_pin(true);
    LCD_SendCommand(0b00001100);
}

void LCD_Display::turn_off() {
    clear();
    print();
    LIGHT = NoBackLight;
    LCD_SendCommand(0b00001000);
}

void LCD_Display::change_backlight(bool state) {
    set_backlight(state);
    if(state){
        LIGHT = BackLight;
    }else{
        LIGHT = NoBackLight;
    }
    LCD_SendCommand(0b0);
}

void LCD_Display::change_enable_pin(bool state){
    set_enable_pin(state);
    if(state){
        PIN_EN = Enable;
    }else{
        PIN_EN = PIN_RS;
    }
    LCD_SendCommand(0b0);
}

void LCD_Display::clear() {
    for (int i = 0; i < 80; i++) {
        new_symbols[i] = ' ';
    }
    if(print_or_no){
        print();
    }
}

void LCD_Display::SmallPrint(int start, int end){
    /* Small function for print. At start it checks if new data not like old. Then pass it or
     * There some magic, for example i, i + 44 .. It connected with adreess of cells in display,
     *  you can read about it in datasheet */
    int state = 1;
    for(int i = start; i < end; i++){
        if(new_symbols[i] != old_symbols[i]){
            if(state){
                if(i < 20){
                    LCD_SendCommand(0b10000000|i);
                }
                if(i >= 20 && i < 40){
                    LCD_SendCommand(0b10000000|(i + 44));
                }
                if(i >= 40 && i < 60){
                    LCD_SendCommand(0b10000000|(i - 20));
                }
                if(i >= 60 && i < 80){
                    LCD_SendCommand(0b10000000|(i + 24));
                }
            }
            LCD_SendData(new_symbols[i]);
            state = 0;
            old_symbols[i] = new_symbols[i];
        }else{
            state = 1;
        }
    }
}

void LCD_Display::print(){
    /* Function for print new array on display */
    SmallPrint(0, 20);
    SmallPrint(40, 60);
    SmallPrint(20, 40);
    SmallPrint(60, 80);
}

void LCD_Display::send_data_to_print(int row, int column, char* str) {
    /* Write String to new cell. */
    int index = row * 20 + column;
    while(*str && index < 80) {
        new_symbols[index] = *str;
        index++;
        str++;
    }
    if(print_or_no){
        print();
    }
}

void LCD_Display::send_data_to_print(int row, int column, char str) {
    /* Write Char to new cell. */
    int index = row * 20 + column;
    if(index < 80) {
        new_symbols[index] = str;
    }
    if(print_or_no){
        print();
    }
}

void LCD_Display::set_print_or_no(bool state){
    print_or_no = state;
}

bool LCD_Display::get_print_or_no(bool state){
    return print_or_no;
}