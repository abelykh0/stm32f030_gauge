#include "eeprom.h"
#include "stm32f0xx_hal.h"

static void SDA_H(void)
{
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);
}

static void SDA_L(void)
{
    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET);
}

static void SCL_H(void)
{
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);
}

static void SCL_L(void)
{
    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET);
}

static uint8_t SDA_Read(void)
{
    return HAL_GPIO_ReadPin(SDA_PORT, SDA_PIN);
}

static void I2C_Delay(void)
{
    for (volatile int i = 0; i < 40; i++)
    {
        __NOP();
    }
}

static void I2C_Start(void)
{
    SDA_H();
    SCL_H();
    I2C_Delay();

    SDA_L();
    I2C_Delay();

    SCL_L();
}

static void I2C_Stop(void)
{
    SDA_L();
    SCL_H();
    I2C_Delay();

    SDA_H();
    I2C_Delay();
}

static uint8_t I2C_WriteByte(uint8_t data)
{
    for (int i = 0; i < 8; i++)
    {
        if (data & 0x80)
            SDA_H();
        else
            SDA_L();

        SCL_H();
        I2C_Delay();

        SCL_L();
        I2C_Delay();

        data <<= 1;
    }

    // ACK bit
    SDA_H();

    SCL_H();
    I2C_Delay();

    uint8_t ack = !SDA_Read();

    SCL_L();

    return ack;
}

static uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t data = 0;

    SDA_H();

    for (int i = 0; i < 8; i++)
    {
        data <<= 1;

        SCL_H();
        I2C_Delay();

        if (SDA_Read())
            data |= 1;

        SCL_L();
        I2C_Delay();
    }


    // ACK/NACK
    if (ack)
        SDA_L();
    else
        SDA_H();

    SCL_H();
    I2C_Delay();

    SCL_L();
    SDA_H();

    return data;
}

void EEPROM_Init(void)
{
    SDA_H();
    SCL_H();
}

uint8_t EEPROM_ReadByte(uint8_t dev, uint8_t mem)
{
    uint8_t data;


    I2C_Start();

    // device address + write
    I2C_WriteByte(dev << 1);

    // EEPROM internal address
    I2C_WriteByte(mem);


    // repeated START
    I2C_Start();

    // device address + read
    I2C_WriteByte((dev << 1) | 1);


    data = I2C_ReadByte(0);   // NACK

    I2C_Stop();


    return data;
}

void EEPROM_WriteByte(uint8_t dev, uint8_t mem, uint8_t data)
{
    I2C_Start();

    // EEPROM address + WRITE
    I2C_WriteByte(dev << 1);

    // EEPROM internal memory address
    I2C_WriteByte(mem);

    // Data byte
    I2C_WriteByte(data);

    I2C_Stop();

    // 24C02 internal write cycle
    HAL_Delay(5);
}
