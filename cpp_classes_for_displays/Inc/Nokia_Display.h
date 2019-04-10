//
// Created by popenyuk on 3/10/19.
//

#ifndef CPP_TASK_NOKIA_DISPLAY_H
#define CPP_TASK_NOKIA_DISPLAY_H

#include "DisplayStrategy.h"
#include "main.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_i2c.h"
#include "stm32f3xx_hal_spi.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f303xc.h"
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <font6x8.h>

class Nokia_Display : public DisplayStrategy{
private:
    static const int LCD_WIDTH = 84;
    static const int LCD_HEIGHT = 48;
    typedef struct LCD5110_conf
    {
        SPI_HandleTypeDef *spi_handle;

        uint16_t 	  spi_cs_pin;
        GPIO_TypeDef *spi_cs_port;

        uint16_t 	  rst_pin;
        GPIO_TypeDef *rst_port;

        uint16_t 	  dc_pin;
        GPIO_TypeDef *dc_port;

        uint16_t blacklight_pin;
        GPIO_TypeDef *blacklight_port;
        unsigned char video_buffer[(LCD_WIDTH * LCD_HEIGHT/8)] = {' '}; // LCD buffer

    } LCD5110_conf;

    typedef struct
    {
        int16_t x0, y0, width, height;
    } rect_t;
    typedef struct
    {
        int16_t x, y;
    } point_t;

    typedef struct
    {
        rect_t frame;
        point_t cursor;
        bool refresh_on_endl;
    } LCD5110_canvas_t;

    typedef struct
    {
        unsigned int adr;
        unsigned char mask;
    } pix_coord;

    static const uint8_t LCD5110_MODE_SET_MASK = (1<<3);
    static const uint8_t LCD5110_FN_SET_H_MASK = 1;
    static const uint8_t FN_SET_MASK = (1<<5);
    static const uint8_t LCD5110_VOLTAGE_MASK = (1<<7);
    static const uint8_t LCD5110_TEMP_COEFF_MASK = (1<<2);
    static const uint8_t  LCD5110_BIAS_MASK = (1<<4);
    static const int16_t LCD5110_SET_X_BIT_MASK = (1<<7);
    static const int16_t LCD5110_SET_Y_BIT_MASK = (1<<6);
    // 	   100 --- normal mode

    LCD5110_conf hw_conf;
    LCD5110_canvas_t def_scr;
    typedef enum {LCD5110_NORMAL_MODE    = (4 | LCD5110_MODE_SET_MASK),
        LCD5110_INVERTED_MODE  = 5 | LCD5110_MODE_SET_MASK,
        LCD5110_ALL_WHITE_MODE = 0 | LCD5110_MODE_SET_MASK,
        LCD5110_ALL_BLACK_MODE = 1 | LCD5110_MODE_SET_MASK
    } LCD5110_modes;
    static inline void SPI_enable(LCD5110_conf* lcd_conf)
    {
        __HAL_SPI_ENABLE( (lcd_conf->spi_handle) );
    }
    static inline void LCD5110_CE_off(LCD5110_conf* lcd_conf) {
        HAL_GPIO_WritePin(lcd_conf->spi_cs_port, lcd_conf->spi_cs_pin, GPIO_PIN_RESET);
    }
    static inline void LCD5110_RST_off(LCD5110_conf* lcd_conf) {
        HAL_GPIO_WritePin(lcd_conf->rst_port, lcd_conf->rst_pin, GPIO_PIN_RESET);
    }
    static inline void LCD5110_RST_on(LCD5110_conf* lcd_conf) {
        HAL_GPIO_WritePin(lcd_conf->rst_port, lcd_conf->rst_pin, GPIO_PIN_SET);
    }
    static inline void LCD5110_DC_off(LCD5110_conf* lcd_conf) {
        HAL_GPIO_WritePin(lcd_conf->dc_port, lcd_conf->dc_pin, GPIO_PIN_RESET);
    }
    enum LCD5110_errors{LCD5110_OK, LCD5110_bad_opcode, LCD5110_bad_dmode,
        LCD5110_bad_voltage, LCD5110_bad_TC, LCD5110_bad_bias,
        LCD5110_bad_fn_mode,
        LCD5110_bad_coordinate,
    };
    static inline HAL_StatusTypeDef send_byte_to_LCD5110(uint8_t dat, LCD5110_conf* lcd_conf)
    {
        //! HAL_SPI_Transmit takes care about waiting transmission to finish.
        //! Details: https://habrahabr.ru/post/276605/ -- do not turn command mode off
        //! before transmission finished. (Check BSY flag before DC_on/off, if directly
        //! manipulating SPIx_DR.
        return HAL_SPI_Transmit(&hspi2, &dat, 1, 1000);
    }
    //! Code: 0010 0PVH, accepts both 00100PVH and 0PVH, but no other.
    static int LCD5110_set_function(uint8_t fn_byte, LCD5110_conf* lcd_conf) {
        if ((fn_byte & ~FN_SET_MASK) > 7) //0b111
        {
            //printf("Seq: err\n");
            return LCD5110_bad_opcode;
        }
        //printf("Seq: %i, dbg: %i \n", fn_byte | FN_SET_MASK, fn_byte );
        send_byte_to_LCD5110(fn_byte | FN_SET_MASK, lcd_conf);
        return LCD5110_OK;
    }
    static int LCD5110_set_voltage_ext(uint8_t voltage, LCD5110_conf* lcd_conf)
    {
        if ( voltage > 127)
        {
            //printf("Seq: err\n");
            return LCD5110_bad_voltage;
        }
        //printf("Seq: %i\n", voltage | LCD5110_VOLTAGE_MASK);
        send_byte_to_LCD5110(voltage | LCD5110_VOLTAGE_MASK, lcd_conf);
        return LCD5110_OK;
    }
    static inline pix_coord xy_to_pix(int x, int y)
    {
        pix_coord res;
        res.adr = static_cast<unsigned int>((y >> 3) * LCD_WIDTH + x);
        res.mask = static_cast<unsigned char>(1 << (y & 0x07));

        return res;
    }
    static int LCD5110_set_temp_coef_ext(uint8_t TC, LCD5110_conf* lcd_conf)
    {
        if ( TC > 3)
        {
            //printf("Seq: err\n");
            return LCD5110_bad_TC;
        }
        //printf("Seq: %i\n", TC | LCD5110_TEMP_COEFF_MASK);
        send_byte_to_LCD5110(TC | LCD5110_TEMP_COEFF_MASK, lcd_conf);
        return LCD5110_OK;
    }
    static int LCD5110_set_bias_ext(uint8_t bias, LCD5110_conf* lcd_conf)
    {
        if ( bias > 7)
        {
            //printf("Seq: err\n");
            return LCD5110_bad_bias;
        }
        //printf("Seq: %i\n", bias | LCD5110_BIAS_MASK);
        send_byte_to_LCD5110(bias | LCD5110_BIAS_MASK, lcd_conf);
        return LCD5110_OK;
    }
    static int LCD5110_set_mode_base(LCD5110_modes mode_byte, LCD5110_conf* lcd_conf)
    {
        if ( (mode_byte & (~LCD5110_INVERTED_MODE) ) != 0) //0b10x0y -- only possible values
        {
            //printf("Seq: err\n");
            return LCD5110_bad_dmode;
        }
        //printf("Seq: %i\n", mode_byte);
        send_byte_to_LCD5110(mode_byte, lcd_conf);
        return LCD5110_OK;
    }
    static inline int LCD5110_set_X_base(int16_t x, LCD5110_conf* lcd_conf)
    {
        if(x<0 || x>LCD_WIDTH-1)
            return LCD5110_bad_coordinate;
        send_byte_to_LCD5110(static_cast<uint8_t>(x | LCD5110_SET_X_BIT_MASK), lcd_conf);
        return LCD5110_OK;
    }
    static inline int LCD5110_set_Y_base(int16_t y, LCD5110_conf* lcd_conf)
    {
        if(y<0 || y>LCD_HEIGHT/8-1) // Byte addressable!
            return LCD5110_bad_coordinate;
        send_byte_to_LCD5110(static_cast<uint8_t>(y | LCD5110_SET_Y_BIT_MASK), lcd_conf);
        return LCD5110_OK;
    }
    static inline int LCD5110_set_XY_base(int16_t x, int16_t y, LCD5110_conf* lcd_conf)
    {
        int res;
        res = LCD5110_set_X_base(x, lcd_conf);
        if( res != LCD5110_OK)
            return res;
        res = LCD5110_set_Y_base(y, lcd_conf);
        if( res != LCD5110_OK)
            return res;

        return LCD5110_OK;
    }
    static inline HAL_StatusTypeDef send_data_to_LCD5110(uint8_t data[], uint16_t size, LCD5110_conf* lcd_conf)
    {
        return HAL_SPI_Transmit(&hspi2, data, size, 1000);
    }
    void LCD5110_drawBitmap(int x, int y, const unsigned char* bitmap, int cols, int rows, int invert);
    void LCD5110_wendl();
    void LCD5110_putpix(int x, int y, int color);
    bool is_printed = true;
    void Init();

public:
    Nokia_Display(SPI_HandleTypeDef* hspi2_var, uint16_t cs_pin, GPIO_TypeDef *cs_gpio_port, uint16_t rst_pin, GPIO_TypeDef *rst_port, uint16_t dc_pin, GPIO_TypeDef *dc_port, uint16_t blacklight_pin, GPIO_TypeDef *blacklight_port);
    void clear() override;
    void turn_on() override;
    void turn_off() override;
    void change_backlight(bool state) override;
    void send_data_to_print(int row, int column, char str) override;
    void send_data_to_print(int row, int column, char* str) override;
};




#endif //CPP_TASK_NOKIA_DISPLAY_H
