//
// Created by Yevhenii on 08/03/2019.
//
#include <DisplayStrategy.h>

#include "../Inc/DisplayStrategy.h"
#include "stdio.h"
#include "stdlib.h"


void DisplayStrategy::set_backlight(bool state) {
    backlight = state;
}

void DisplayStrategy::set_enable_pin(bool state){
    enable_pin = state;
}

bool DisplayStrategy::is_light_on() {
    return backlight;
}

bool DisplayStrategy::is_enabled() {
    return enable_pin;
}

void DisplayStrategy::send_number_to_print(int row, int column, int number, int base) {
    /* Write Int to new cell. */
    char sentence[10];
    itoa(number, sentence, base);
    send_data_to_print(row, column, sentence);
}

void DisplayStrategy::send_number_to_print(int row, int column, float number, int precision) {
    /* Write Float to new cell. */
    int first_data = number;
    int last_data = number * precision - first_data * precision;
    send_number_to_print(row, column, first_data, 10);
    send_data_to_print(row + 5, column, ',');
    send_number_to_print(row + 10, column, last_data, 10);
}

void DisplayStrategy::send_number_to_print(int row, int column, double number, int precision) {
    /* Write Double to new cell. */
    int first_data = number;
    int last_data = number * precision - first_data * precision;
    send_number_to_print(row, column, first_data, 10);
    send_data_to_print(row + x_nullspace, column + y_nullspace, '.');
    send_number_to_print(row + 2* x_nullspace, column + 2 * y_nullspace, last_data, 10);
}

void DisplayStrategy::set_x_y_nullspace(int x, int y) {
    x_nullspace = x;
    y_nullspace = y;
}
