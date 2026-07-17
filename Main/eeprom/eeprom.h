#ifndef __EEPROM_H
#define __EEPROM_H

#include "stdint.h"
#include "main.h"

#define SCL_PORT EEPROM_SCL_GPIO_Port
#define SCL_PIN  EEPROM_SCL_Pin

#define SDA_PORT EEPROM_SDA_GPIO_Port
#define SDA_PIN  EEPROM_SDA_Pin

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
