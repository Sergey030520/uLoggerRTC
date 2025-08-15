#include "button.h"
#include "board_pins.h"
#include "IRQ.h"

#define MAX_BUTTONS 3
static ButtonCallback_t button_callbacks[MAX_BUTTONS] = {0};

void handle_button_interrupt(uint8_t button_pin);

void init_buttons()
{
    GPIO_PinConfig_t pin_conf = {
        .gpiox = BUTTON_GPIO_PORT,
        .speed = GPIO_SPEED_2MHz,
        .mode = GPIO_MODER_INPUT,
        .output = GPIO_OUTPUT_PUSHPULL,
        .pull = GPIO_PULL_UP,
        .af = 0};
    for (int pin_btn = BUTTON_SW1_PIN; pin_btn <= BUTTON_SW3_PIN; ++pin_btn)
    {
        pin_conf.pin = pin_btn;
        set_gpio_conf(&pin_conf);
    }

    configure_gpio_exti(BUTTON_GPIO_PORT, BUTTON_SW1_PIN, EDGE_FALLING, EXTI15_10_IRQn);
    configure_gpio_exti(BUTTON_GPIO_PORT, BUTTON_SW2_PIN, EDGE_FALLING, EXTI15_10_IRQn);
    configure_gpio_exti(BUTTON_GPIO_PORT, BUTTON_SW3_PIN, EDGE_FALLING, EXTI15_10_IRQn);
}

void register_button_callback(uint8_t button_pin, ButtonCallback_t cb)
{
    if (button_pin < MAX_BUTTONS)
        button_callbacks[button_pin] = cb;
}

void handle_button_interrupt(uint8_t button_pin)
{
    if (button_pin >= BUTTON_SW1_PIN && button_pin <= BUTTON_SW3_PIN)
    {
        ButtonCallback_t cb = button_callbacks[button_pin - BUTTON_SW1_PIN];
        if (cb)
            cb(); 
    }
}

void EXTI15_10_IRQHandler(void)
{
    for (uint8_t pin = BUTTON_SW1_PIN; pin <= BUTTON_SW3_PIN; ++pin)
    {
        if (exti_check_pending(pin))
        {
            exti_clear_pending(pin);
            handle_button_interrupt(pin);
        }
    }
}