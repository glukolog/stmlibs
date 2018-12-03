#ifndef _BMP280_LIB_H_
#define _BMP280_LIB_H_

#include <math.h>
#include <stdint.h>
//#include "i2c-soft.h"

#ifndef BMP280_ADDR
#define BMP280_ADDR 0xEC
#endif

//#define BMP280_SDO
//#define BMP280_S32_t long int
//#define BMP280_U32_t unsigned long int
#define BMP280_S32_t int32_t
#define BMP280_U32_t uint32_t
#define adc_T b2t
#define adc_P b2p
//#define double float

#define osrs_p_x1   0x04
#define osrs_p_x2   0x08
#define osrs_p_x4   0x0C
#define osrs_p_x16  0x1C
#define osrs_t_x1   0x20
#define osrs_t_x2   0x40
#define osrs_t_x16  0xE0
#define osrm_sleep  0x00
#define osrm_force  0x01
#define osrm_norm   0x03

#define osrt_1ms    0x00
#define osrt_62ms   0x20
#define osrt_125ms  0x40
#define osrt_250ms  0x60
#define osrt_1s     0xA0

unsigned char buf280[24];

struct s_baroparams280 {
    unsigned short dig_T1;
    short dig_T2;
    short dig_T3;
    unsigned short dig_P1;
    short dig_P2;
    short dig_P3;
    short dig_P4;
    short dig_P5;
    short dig_P6;
    short dig_P7;
    short dig_P8;
    short dig_P9;
} bar280;

extern void I2C_Transmit(uint8_t addr, uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len);

char BMP280_Init();
char BMP280_InitForce();
char BMP280_ReadRegs();
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. 
// t_fine carries fine temperature as global value
//BMP280_S32_t t_fine;

char BMP280_ReadTP(float *t280, float *p280);
float BMP280_Altitude(float pressure);
float BMP280_AltitudeTable(float x);

#endif
