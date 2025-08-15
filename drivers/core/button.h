#pragma once


#include <stdint.h>

typedef void (*ButtonCallback_t)(void);

void init_buttons();


void register_button_callback(uint8_t button_pin, ButtonCallback_t cb);