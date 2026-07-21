#include "startup.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "adc.h"
#include "gps.h"
#include "screen.h"
#include "GUI_Paint.h"

#define ADC_CHANNEL_COUNT 8
extern UART_HandleTypeDef huart1;

static int partial_count = 0;
static uint16_t adc_buffer[ADC_CHANNEL_COUNT] __attribute__((aligned(4)));

extern "C" void setup()
{
	EPD_2IN66_Init();
	EPD_2IN66_Clear();
	DEV_Delay_ms(500);
	Paint_NewImage(ScreenData, EPD_2IN66_WIDTH, EPD_2IN66_HEIGHT, 270, WHITE);
	EPD_2IN66_Init_Partial();

	gps_init();

    if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_ADC_Start_DMA(&hadc, (uint32_t*)adc_buffer, ADC_CHANNEL_COUNT) != HAL_OK)
    {
        Error_Handler();
    }
}

extern "C" void loop()
{
	uint16_t fuel = adc_buffer[5];
	uint16_t oil = adc_buffer[6];

	if (gps_ready)
	{
	  char local[NMEA_MAX];

	  __disable_irq();

	  gps_ready = 0;
	  strcpy(local, gps_latest);

	  __enable_irq();

	  if (strncmp(local, "$GNRMC", 6) == 0)
	  {
		  GPS_Result_t gps_result = ParseGNRMC(local);

		  partial_count++;

		  Paint_ClearWindows(
			  0,
			  0,
			  EPD_2IN66_HEIGHT - 1,
			  EPD_2IN66_WIDTH - 1,
			  WHITE
		  );

		  char display[32];

		  // Speed
		  snprintf(
			  display,
			  sizeof(display),
			  "%lu.%lu",
			  gps_result.speed_kmh10 / 10,
			  gps_result.speed_kmh10 % 10
		  );
		  Paint_DrawString_EN(
			  22, 8,
			  display,
			  &seg7_font_large,
			  BLACK,
			  WHITE
		  );

		  // Fuel level
		  sprintf(
			  display,
			  "%04d",
			  fuel
		  );
		  Paint_DrawString_EN(
			  220, 8,
			  display,
			  &seg7_font_small,
			  BLACK,
			  WHITE
		  );

		  // Oil
		  sprintf(
			  display,
			  "%04d",
			  oil
		  );
		  Paint_DrawString_EN(
			  220, 40,
			  display,
			  &seg7_font_small,
			  BLACK,
			  WHITE
		  );

		  // Time
		  sprintf(
			  display,
			  "%02d:%02d",
			  gps_result.hour % 100,
			  gps_result.minute % 100
		  );
		  Paint_DrawString_EN(
			  10, 120,
			  display,
			  &seg7_font_small,
			  BLACK,
			  WHITE
		  );

		  // Date
		  sprintf(
			  display,
			  "%02d/%02d/20%02d",
			  gps_result.day % 100,
			  gps_result.month % 100,
			  gps_result.year % 10000
		  );
		  Paint_DrawString_EN(
			  120, 120,
			  display,
			  &seg7_font_small,
			  BLACK,
			  WHITE
		  );

		  //Paint_DrawString_EN(10, 20, local, &Font12, BLACK, WHITE);

		  if (partial_count >= 60)
		  {
			  partial_count = 0;

			  //EPD_2IN66_Init();
			  //EPD_2IN66_Display(BlackImage);
			  EPD_2IN66_Update_Full();

			  EPD_2IN66_Init_Partial();
		  }
		  else
		  {
			  EPD_2IN66_Display(ScreenData);
		  }
	  }
	  //HAL_Delay(1000);
	}
}

extern "C" void initialize()
{
}
