#pragma once

#include "gpio.h"
#include "memory_map.h"


#define LED_PORT    GPIOB_REG
#define BUTTON_GPIO_PORT     GPIOE_REG
#define USART1_TX_PORT  GPIOA_REG
#define USART1_RX_PORT  GPIOA_REG
#define SDIO_PORT     GPIOC_REG



// --- Определение пинов ---

// LED
#define LED1_PIN       13
#define LED2_PIN       14
#define LED3_PIN       15

// BUTTONS
#define BUTTON_SW1_PIN       10
#define BUTTON_SW2_PIN       11
#define BUTTON_SW3_PIN       12


// USART
#define USART1_TX_PIN   9
#define USART1_RX_PIN   10

// SDIO
#define SDIO_CLK_PIN  12
#define SDIO_CMD_PIN  2
#define SDIO_D0_PIN   8
#define SDIO_D1_PIN   9
#define SDIO_D2_PIN   10
#define SDIO_D3_PIN   11
#define SDIO_PD3_PIN 3