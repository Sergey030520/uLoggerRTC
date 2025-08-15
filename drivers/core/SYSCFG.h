#pragma once

#include <stdint.h>
#include "gpio.h"


typedef struct
{
    uint32_t MEMRMP;
    uint32_t PMC;
    uint32_t EXTICR1;
    uint32_t EXTICR2;
    uint32_t EXTICR3;
    uint32_t EXTICR4;
    uint32_t CMPCR;
}SYSCFG_Type;

#define SYSCFG_EXTICR1()

// MEMRMP
#define SYSCFG_MEMRMP_MEM_MODE (0x1 << 0)


// PMC
#define SYSCFG_PMC_MII_RMII_SEL (0x1 << 23)

//EXTICRx
#define SYSCFG_EXTICRx(port, pin) (port << ((pin%4) * 4)) 
#define SYSCFG_EXTICRx_MASK(pin) (0xF << ((pin%4) * 4)) 

// CMPCR
#define SYSCFG_CMPCR_READY (0x1 << 8)
#define SYSCFG_CMPCR_CMP_PD (0x1 << 0)



void exti_set_port(uint8_t pin, GPIO_Port_t port);