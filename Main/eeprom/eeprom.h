#ifndef __EEPROM_H
#define __EEPROM_H

#include "stdint.h"

#define SCL_PORT GPIOA
#define SCL_PIN  GPIO_PIN_9

#define SDA_PORT GPIOA
#define SDA_PIN  GPIO_PIN_10

#ifdef __cplusplus
extern "C" {
#endif

void EEPROM_Init(void);
uint8_t EEPROM_ReadByte(uint8_t dev, uint8_t mem);
void EEPROM_WriteByte(uint8_t dev, uint8_t mem, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
