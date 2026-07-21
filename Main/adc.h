#ifndef __ADC_H__
#define __ADC_H__

#include "stdint.h"
#include "stm32f0xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

extern ADC_HandleTypeDef hadc;

uint16_t ReadADC(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif
