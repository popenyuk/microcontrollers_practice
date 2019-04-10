//
// Created by Yevhenii on 08/03/2019.
//

#ifndef INC_2_LCD_DISPLAYS_DISPLAYSTRATEGY_H
#define INC_2_LCD_DISPLAYS_DISPLAYSTRATEGY_H

class DisplayStrategy{
private:
    bool backlight = true;
    bool enable_pin = true;
    int x_nullspace;
    int y_nullspace;
public:
    virtual void clear() = 0;
    bool is_enabled();
    bool is_light_on();
    virtual void turn_on() = 0;
    virtual void turn_off() = 0;
    void set_backlight(bool state);
    void set_enable_pin(bool state);
    virtual void change_backlight(bool state) = 0;
    virtual void send_data_to_print(int row, int column, char str) = 0;
    virtual void send_data_to_print(int row, int column, char* str) = 0;
    void set_x_y_nullspace(int x, int y);
    void send_number_to_print(int row, int column, int number, int base);
    void send_number_to_print(int row, int column, float number, int precision);
    void send_number_to_print(int row, int column, double number, int precision);
};

#endif //INC_2_LCD_DISPLAYS_DISPLAYSTRATEGY_H
