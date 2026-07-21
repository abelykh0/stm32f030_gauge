#include "adc.h"

uint16_t ReadADC(uint8_t channel)
{
    ADC1->CHSELR = channel;   // select only one channel

    HAL_ADC_Start(&hadc);

    HAL_ADC_PollForConversion(&hadc, 100);

    uint16_t value = HAL_ADC_GetValue(&hadc);

    HAL_ADC_Stop(&hadc);

    return value;
}
