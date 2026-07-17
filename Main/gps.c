#include "gps.h"

#include "stm32f0xx_hal.h"
#include <string.h>
#include "minmea.h"

extern UART_HandleTypeDef huart1;

static uint8_t gps_rx_byte;
static uint16_t gps_index = 0;
static char gps_line[NMEA_MAX];

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

    	// apply timezone
    	// TODO hardcoded -5
    	int h = frame.time.hours + (-5); // timezone
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

        // km/h * 10
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
            gps_line[gps_index] = 0;

            // copy completed sentence as "latest"
            memcpy(
                gps_latest,
                gps_line,
                gps_index + 1
            );

            gps_ready = 1;

            gps_index = 0;
        }
        else
        {
            if (gps_index < NMEA_MAX - 1)
            {
                gps_line[gps_index++] = c;
            }
        }

        // continue receiving
        HAL_UART_Receive_IT(
            &huart1,
            &gps_rx_byte,
            1
        );
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

        HAL_UART_Receive_IT(
            &huart1,
            &gps_rx_byte,
            1
        );
    }
}
