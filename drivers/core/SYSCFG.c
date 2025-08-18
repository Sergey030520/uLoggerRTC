#include "SYSCFG.h"
#include "memory_map.h"


SYSCFG_Type *SYSCFG = SYSCFG_REG;

void exti_set_port(GPIO_Port_t port, uint8_t pin)
{
    volatile uint32_t *exticr = &SYSCFG->EXTICRx[pin/4];
    *exticr &= ~SYSCFG_EXTICRx_MASK(pin);       
    *exticr |= SYSCFG_EXTICRx(port, pin);       
}