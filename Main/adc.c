#include "adc.h"

#include "stm32f0xx_hal.h"

extern ADC_HandleTypeDef hadc;

uint16_t ReadADC_Channel(uint8_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);

    uint16_t value = HAL_ADC_GetValue(&hadc);

    HAL_ADC_Stop(&hadc);

    return value;
}
