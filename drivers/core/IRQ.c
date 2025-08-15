#include "IRQ.h"
#include "memory_map.h"
#include "SYSCFG.h"


NVIC_Type *NVIC = NVIC_REG;
EXTI_Type *EXTI = EXTI_REG;


static inline void __enable_irq(void)
{
    asm volatile("cpsie i" : : : "memory");
}

void init_irq()
{
    NVIC->ISERx[1] = NVIC_SDIO; //| NVIC_USART1;
    // NVIC->ICERx[1] = NVIC_DMA2_STREAM4;
    NVIC->ICERx[2] = NVIC_DMA2_STREAM7;
    __enable_irq();
}

void enable_irq_nvic(uint8_t irqn, uint8_t priority)
{
    // Устанавливаем приоритет (старшие 4 бита используются)
    NVIC->IPRx[irqn] = (priority & 0xF) << 4;

    // Разрешаем прерывание в ISER
    NVIC->ISERx[irqn / 32] = (1UL << (irqn % 32));
}
void disable_irq_nvic(uint8_t irqn)
{
    NVIC->ICERx[irqn / 32] = (1UL << (irqn % 32));
}

// EXTI


void configure_gpio_exti(GPIO_Port_t port, uint8_t pin, EXTI_TriggerEdge_t edge, IRQn_Type irqn)
{
    // 1. Настраиваем порт в SYSCFG
    exti_set_port(pin, port);

    // 2. Настраиваем фронт/спад
    if (edge == EDGE_RISING || edge == EDGE_BOTH)
        EXTI->RSTR |= EXTI_RSTRx(pin);
    else
        EXTI->RSTR &= ~EXTI_RSTRx(pin);

    if (edge == EDGE_FALLING || edge == EDGE_BOTH)
        EXTI->FTSR |= EXTI_FTSRx(pin);
    else
        EXTI->FTSR &= ~EXTI_FTSRx(pin);

    // 3. Разрешаем прерывание и сбрасываем флаг
    EXTI->IMR |= EXTI_IMRx(pin);
    EXTI->PR  |= EXTI_PRx(pin);

    // 4. Разрешаем NVIC для соответствующего IRQ
    enable_irq_nvic(irqn, 5);
}


uint8_t exti_check_pending(uint8_t pin)
{
    return (EXTI->PR & (1 << pin)) != 0;
}

void exti_clear_pending(uint8_t pin)
{
    EXTI->PR |= (1 << pin); 
}