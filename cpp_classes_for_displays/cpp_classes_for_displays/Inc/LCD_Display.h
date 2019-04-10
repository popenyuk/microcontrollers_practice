//
// Created by Yevhenii on 08/03/2019.
//

#ifndef INC_2_LCD_DISPLAYS_LCD_DISPLAY_H
#define INC_2_LCD_DISPLAYS_LCD_DISPLAY_H

#include "DisplayStrategy.h"
#include "main.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_i2c.h"
#include "i2c.h"
#include "gpio.h"
#include "stdio.h"
#include "stdlib.h"

class LCD_Display: public DisplayStrategy{
public:
    LCD_Display(uint8_t lcd_addr, I2C_HandleTypeDef i2c_variable);
    void turn_on() override;
    void turn_off() override;
    void clear() override;
    void print();
    void change_backlight(bool state) override;
    void change_enable_pin(bool state);
    void send_data_to_print(int row, int column, char str) override;
    void send_data_to_print(int row, int column, char* str) override;
    void search_I2C_bus(I2C_HandleTypeDef i2c);
    void search_I2C_bus_without_semihosting(I2C_HandleTypeDef i2c_Variable);
    void set_print_or_no(bool state);
    bool get_print_or_no(bool state);

private:
    bool print_or_no = true;
    uint8_t Disable = 0;
    uint8_t new_symbols[80] = {};
    uint8_t old_symbols[80] = {};
    uint8_t BackLight = 1 << 3;
    uint8_t NoBackLight = 0x00;
    uint8_t LIGHT = BackLight;
    uint8_t PIN_RS = 1 << 0;
    uint8_t Enable = 1 << 2;
    uint8_t PIN_EN = Enable;
    uint32_t LCD_DELAY_MS = 5;
    uint8_t LCD_ADDR = 0x3f<<1;
    I2C_HandleTypeDef I2C_variable;
    void SmallPrint(int start, int end);
    void LCD_SendData(uint8_t data);
    void LCD_SendCommand(uint8_t cmd);
    HAL_StatusTypeDef LCD_SendInternal(uint8_t data, uint8_t flags);
};

#endif //INC_2_LCD_DISPLAYS_LCD_DISPLAY_H
