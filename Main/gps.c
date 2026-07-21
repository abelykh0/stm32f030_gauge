#include "gps.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include "minmea.h"

extern UART_HandleTypeDef huart1;

static uint8_t gps_rx_byte;
static uint16_t gps_index = 0;

// Single buffer - used directly for collection AND as the latest complete sentence
char gps_latest[NMEA_MAX];
bool gps_ready = false;

void gps_init()
{
    HAL_UART_Receive_IT(&huart1, &gps_rx_byte, 1);
}

GPS_Result_t ParseGNRMC(char* nmea)
{
    struct minmea_sentence_rmc frame;
    GPS_Result_t gps_result;

    if (minmea_parse_rmc(&frame, nmea))
    {
        gps_result.isValid = true;
        gps_result.minute = frame.time.minutes;
        gps_result.second = frame.time.seconds;
        gps_result.day = frame.date.day;
        gps_result.month = frame.date.month;
        gps_result.year = frame.date.year % 100;

        // apply timezone (hardcoded -4)
        int h = frame.time.hours + (-4);
        if (h < 0)
        {
            h += 24;
            gps_result.day--;
        }
        else if (h >= 24)
        {
            h -= 24;
            gps_result.day++;
        }
        gps_result.hour = h;

        gps_result.speed_kmh10 =
            ((int64_t)frame.speed.value * 1852) /
            frame.speed.scale /
            100;
    }
    else
    {
        gps_result.isValid = false;
    }

    return gps_result;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        char c = gps_rx_byte;

        if (c == '\n')
        {
            // Terminate the string directly in gps_latest
            gps_latest[gps_index] = '\0';
            gps_ready = true;
            gps_index = 0;
        }
        else
        {
            if (gps_index < NMEA_MAX - 1)
            {
                // Write directly into gps_latest
                gps_latest[gps_index++] = c;
            }
        }

        HAL_UART_Receive_IT(&huart1, &gps_rx_byte, 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        uint32_t err = HAL_UART_GetError(huart);

        if (err & HAL_UART_ERROR_ORE)
        {
            __HAL_UART_CLEAR_OREFLAG(huart);
        }

        HAL_UART_Receive_IT(&huart1, &gps_rx_byte, 1);
    }
}
