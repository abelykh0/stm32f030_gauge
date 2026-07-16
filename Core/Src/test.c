#include "test.h"


#define SCL_PORT GPIOA
#define SCL_PIN  GPIO_PIN_9

#define SDA_PORT GPIOA
#define SDA_PIN  GPIO_PIN_10


volatile uint8_t found_address;
volatile uint8_t eeprom_dump[256];


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


void EEPROM_Test_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio.Pin = SCL_PIN | SDA_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &gpio);

    SDA_H();
    SCL_H();
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


static uint8_t EEPROM_ReadByte(uint8_t dev, uint8_t mem)
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

static void EEPROM_WriteByte(uint8_t dev, uint8_t mem, uint8_t data)
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

void EEPROM_Test_Run(void)
{
    uint8_t addr;


    found_address = 0;


    // Find EEPROM
    for (addr = 0x50; addr <= 0x57; addr++)
    {
        I2C_Start();

        if (I2C_WriteByte(addr << 1))
        {
            found_address = addr;
            I2C_Stop();
            break;
        }

        I2C_Stop();
    }


    if (found_address == 0)
    {
        __BKPT(1);   // EEPROM not found
        return;
    }


    // Read all EEPROM
    for (int i = 0; i < 256; i++)
    {
        eeprom_dump[i] =
            EEPROM_ReadByte(found_address, i);
    }

    // Test write/read at last EEPROM byte (0xFF)

    uint8_t old_value = eeprom_dump[255];

    EEPROM_WriteByte(found_address, 0xFF, 0xA5);

    HAL_Delay(5);

    uint8_t read_back = EEPROM_ReadByte(found_address, 0xFF);

    // Restore original value
    //EEPROM_WriteByte(found_address, 0xFF, old_value);

    HAL_Delay(5);

    volatile uint8_t write_test_ok = (read_back == 0xA5);


    __BKPT(0);   // finished
}
