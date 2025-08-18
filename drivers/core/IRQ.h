#pragma once


#include <stdint.h>
#include "gpio.h"

// NVIC

typedef struct
{
    uint32_t ICTR; // Interrupt Controller Type Register
    uint32_t RESERV1[62];
    uint32_t ISERx[8]; // Interrupt Set-Enable Registers
    uint32_t RESERV2[24];
    uint32_t ICERx[8]; // Interrupt Clear-Enable Registers
    uint32_t RESERV3[24];
    uint32_t ISPRx[8]; // Interrupt Set-Pending Registers
    uint32_t RESERV4[24];
    uint32_t ICPRx[8]; // Interrupt Clear-Pending Registers
    uint32_t RESERV5[24];
    uint32_t IABRx[8]; // Interrupt Active Bit Register
    uint32_t RESERV6[56];
    uint32_t IPRx[60]; // Interrupt Priority Register
} NVIC_Type;

typedef enum
{
    NonMaskableInt_IRQn         = -14,
    HardFault_IRQn              = -13,
    MemoryManagement_IRQn       = -12,
    BusFault_IRQn               = -11,
    UsageFault_IRQn             = -10,
    SVCall_IRQn                 = -5,
    DebugMonitor_IRQn           = -4,
    PendSV_IRQn                 = -2,
    SysTick_IRQn                = -1,

    // EXTI линии (STM32F4/F1 пример)
    EXTI0_IRQn                  = 6,   // EXTI Line0
    EXTI1_IRQn                  = 7,   // EXTI Line1
    EXTI2_IRQn                  = 8,   // EXTI Line2
    EXTI3_IRQn                  = 9,   // EXTI Line3
    EXTI4_IRQn                  = 10,  // EXTI Line4
    EXTI9_5_IRQn                = 23,  // EXTI Lines 5 to 9
    EXTI15_10_IRQn              = 40,  // EXTI Lines 10 to 15

} IRQn_Type;

#define NVIC_DMA2_STREAM4 (0x1 << 28) // 60
#define NVIC_DMA2_STREAM7 (0x1 << 6)  // 70

#define NVIC_USART1 (0x1 << 5) // 37
#define NVIC_SDIO (0x1 << 17)  // 49

extern NVIC_Type *NVIC;

void init_irq();

void enable_irq_nvic(uint8_t irqn, uint8_t priority);
void disable_irq_nvic(uint8_t irqn);

// EXTI

typedef struct 
{
    uint32_t IMR;
    uint32_t EMR;
    uint32_t RTSR;
    uint32_t FTSR;
    uint32_t SWIER;
    uint32_t PR;
}EXTI_Type;

typedef enum {
    EDGE_RISING,
    EDGE_FALLING,
    EDGE_BOTH
} EXTI_TriggerEdge_t;

#define EXTI_IMRx(pin) (0x1 << pin) // Interrupt mask register
#define EXTI_EMRx(pin) (0x1 << pin) // Event mask register
#define EXTI_RSTRx(pin) (0x1 << pin) // Rising trigger selection
#define EXTI_FTSRx(pin) (0x1 << pin) // Falling trigger selection
#define EXTI_SWIERx(pin) (0x1 << pin) // Software interrupt event
#define EXTI_PRx(pin) (0x1 << pin) // Pending register



void configure_gpio_exti(GPIO_Port_t port, uint8_t pin, EXTI_TriggerEdge_t edge, IRQn_Type irqn);
uint8_t exti_check_pending(uint8_t pin);
void exti_clear_pending(uint8_t pin);

void EXTI15_10_IRQHandler();