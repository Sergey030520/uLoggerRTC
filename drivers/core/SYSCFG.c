#include "SYSCFG.h"
#include "memory_map.h"


SYSCFG_Type *SYSCFG = SYSCFG_REG;

void exti_set_port(uint8_t pin, GPIO_Port_t port)
{
    volatile uint32_t *exticr = &SYSCFG->EXTICR1 + pin/4;
    *exticr &= ~SYSCFG_EXTICRx_MASK(pin);       
    *exticr |= SYSCFG_EXTICRx(port, pin);       
}