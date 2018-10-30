#ifndef I2CSOFT_H
#define I2CSOFT_H

#include <stddef.h>
#include <stm32f10x.h>
#define I2C_SCL_PORT GPIOB
#define I2C_SDA_PORT GPIOB
#define I2C_SCL_PIN 6
#define I2C_SDA_PIN 7
#define _delay_us(X) delay_us(X)
#define TimeX 1

void I2C_Start(void)
{
        I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
        delay_us(TimeX);
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        delay_us(TimeX);
        while(!(I2C_SDA_PORT->IDR & (1<<I2C_SDA_PIN)))
        {
        I2C_SCL_PORT->BRR  = (1<<I2C_SCL_PIN);
        delay_us(TimeX);
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        delay_us(TimeX);
        }
        I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
        delay_us(TimeX);
        I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
        delay_us(TimeX);
}

void I2C_Stop(void)
{
        I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
        _delay_us(TimeX);
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        _delay_us(TimeX);
        I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
        _delay_us(TimeX);
}

uint8_t I2C_Write_Byte(uint8_t data)
{
       uint8_t i;
       uint8_t ACK;
       for(i=0;i<8;i++)
       {
       if(data & 0x80)
       {
       I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
       }
       else
       {
       I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
       }
       _delay_us(TimeX);
       I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
       _delay_us(TimeX);
       I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
       data=data<<1;
       }
       _delay_us(TimeX);
       I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
       _delay_us(TimeX);
       ACK = !(I2C_SDA_PORT->IDR & (1<<I2C_SDA_PIN));
       I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
       I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);

       return ACK;
}

void I2C_Write_Block(uint8_t address, uint8_t *I2C_Buffer, uint8_t length)
{
        uint8_t i ;

        I2C_Start();
        I2C_Write_Byte(0xA0);
        I2C_Write_Byte(address);

        for(i=0;i<length;i++)
        {
        I2C_Write_Byte(I2C_Buffer[i]);
        }
        I2C_Stop();
}

uint8_t I2C_Read_Byte(uint8_t ACK)
{
        uint8_t i;
        uint8_t data;

        I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
      for(i=0;i<8;i++)
        {
        _delay_us(TimeX);
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        _delay_us(TimeX);
        data<<=1;
      if(I2C_SDA_PORT->IDR & (1<<I2C_SDA_PIN))
        data++;
        I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
        }
      if (ACK)
        I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
        _delay_us(TimeX);
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        _delay_us(TimeX);
        I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
        I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
        return data;
}

void I2C_Read_Block(uint8_t address, uint8_t *I2C_Buffer, uint8_t length)
{
        uint8_t i ;

        I2C_Start();
        I2C_Write_Byte(0xA0);
        I2C_Write_Byte(address);
        I2C_Start();
        I2C_Write_Byte(0xA1);
        for(i=0;i<length;i++)
        {
        I2C_Buffer[i]  = I2C_Read_Byte(i!=(length-1));
        }
        I2C_Stop();
}

void I2C_Transmit(uint8_t addr, uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len)
{
    uint8_t i;

    if (tx_len && tx_buf != NULL) {
        I2C_Start();
        I2C_Write_Byte(addr & 0xFE);
        for(i=0 ; i < tx_len ; i++) I2C_Write_Byte(tx_buf[i]);
        I2C_Stop();
        //if (!tx_len || tx_buf == NULL) I2C_Stop();
    }

    if (rx_len && rx_buf != NULL) {
        I2C_Start();
        I2C_Write_Byte(addr | 0x01);
        for(i=0;i<rx_len;i++) rx_buf[i] = I2C_Read_Byte(i!=(rx_len-1));
        I2C_Stop();
    }
}
#endif
