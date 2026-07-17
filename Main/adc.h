#ifndef __ADC_H__
#define __ADC_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t ReadADC_Channel(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif
