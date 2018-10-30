#include <stddef.h>
#include <stdio.h>
#include "bmp280.h"

char BMP280_Init() {
    unsigned char sets280[4] = { 0xF4, osrs_t_x1 | osrs_p_x2 | osrm_norm, 0xF5, osrt_62ms };
    unsigned char addr280 = 0x88;

    I2C_Transmit(BMP280_ADDR, sets280, 4, NULL, 0);
    I2C_Transmit(BMP280_ADDR, &addr280, 1, (unsigned char *)&bar280, 24);
    return 0;
}

char BMP280_InitForce() {
    unsigned char sets280[4] = { 0xF4, osrs_t_x1 | osrs_p_x4 | osrm_force, 0xF5, osrt_62ms };
 //   if (twi_master_trans(BMP280_ADDR >> 1, sets280, 2, NULL, 0)) return 1;
    return 0;
}

char BMP280_ReadRegs() {
    unsigned char addr280 = 0xf7;
//    if (twi_master_trans(BMP280_ADDR >> 1, &addr280, 1, buf280, 6)) return 1;
//    else return 0;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. 
// t_fine carries fine temperature as global value
//BMP280_S32_t t_fine;

char BMP280_ReadTP(float *t280, float *p280) {
    unsigned char addr280 = 0xf7, *pbyte;
    long int b2t = 0, b2p = 0;
    long int t_fine;
    
    //if (twi_master_trans(BMP280_ADDR >> 1, &addr280, 1, buf280, 6))
    I2C_Transmit(BMP280_ADDR, &addr280, 1, buf280, 6);
    if (1) {
        if ((buf280[2] & 0x0f) | (buf280[5] & 0x0f)) return 0;  
        pbyte = (uint8_t *)&b2p;
        *(pbyte+0) = buf280[2];
        *(pbyte+1) = buf280[1];
        *(pbyte+2) = buf280[0];
        pbyte = (uint8_t *)&b2t;
        *(pbyte+0) = buf280[5];
        *(pbyte+1) = buf280[4];
        *(pbyte+2) = buf280[3];
        b2t >>= 4;
        b2p >>= 4;
        t_fine = ((((adc_T >> 4)- ((long int)bar280.dig_T1>>0))) * ((long int)bar280.dig_T2)) >> 10;
        *t280 = ((float)t_fine) / 5120.0; 
        //printf("%X %X\r\n", b2t, b2p);
        if(1) {
            BMP280_S32_t var1, var2;
            BMP280_U32_t p;
            var1 = (((BMP280_S32_t)t_fine)>>1) - (BMP280_S32_t)64000;
            var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BMP280_S32_t)bar280.dig_P6);
            var2 = var2 + ((var1*((BMP280_S32_t)bar280.dig_P5))<<1);
            var2 = (var2>>2)+(((BMP280_S32_t)bar280.dig_P4)<<16);
            var1 = (((bar280.dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BMP280_S32_t)bar280.dig_P2) * var1)>>1))>>18;
            var1 =((((32768+var1))*((BMP280_S32_t)bar280.dig_P1))>>15);
            if (var1 == 0) return 0; // avoid exception caused by division by zero
            p = (((BMP280_U32_t)(((BMP280_S32_t)1048576)-adc_P)-(var2>>12)))*3125;
            if (p < 0x80000000) p = (p << 1) / ((BMP280_U32_t)var1); else p = (p / (BMP280_U32_t)var1) * 2;
            var1 = (((BMP280_S32_t)bar280.dig_P9) * ((BMP280_S32_t)(((p>>3) * (p>>3))>>13)))>>12;
            var2 = (((BMP280_S32_t)(p>>2)) * ((BMP280_S32_t)bar280.dig_P8))>>13;
            *p280 = (BMP280_U32_t)((BMP280_S32_t)p + ((var1 + var2 + bar280.dig_P7) >> 4));
        } else {
            /*
            double var1, var2, p;
            var1 = ((double)t_fine/2.0) - 64000.0;
            var2 = var1 * var1 * ((double)bar280.dig_P6) / 32768.0;
            var2 = var2 + var1 * ((double)bar280.dig_P5) * 2.0;
            var2 = (var2/4.0)+(((double)bar280.dig_P4) * 65536.0);
            var1 = (((double)bar280.dig_P3) * var1 * var1 / 524288.0 + ((double)bar280.dig_P2) * var1) / 524288.0;
            var1 = (1.0 + var1 / 32768.0)*((double)bar280.dig_P1);
            if (var1 == 0.0) return 0; // avoid exception caused by division by zero
            p = 1048576.0 - (double)adc_P;
            p = (p - (var2 / 4096.0)) * 6250.0 / var1;
            var1 = ((double)bar280.dig_P9) * p * p / 2147483648.0;
            var2 = p * ((double)bar280.dig_P8) / 32768.0;
            p = p + (var1 + var2 + ((double)bar280.dig_P7)) / 16.0;
            *p280 = p/100.0;
            */
        } 
        return 1;
    }
    else return 0;
}

float BMP280_Altitude(float pressure)
{
    float Altitude;
//    Altitude = (1 - (pow(pressure / 1013.25,(1 / 5.255)))) * 44330;
    Altitude = (1 - (exp((1 / 5.255)*log(pressure / 1013.25)))) * 44330;
    return Altitude;
}

const int table_alti[23] = {
    30330, 19325, 15799, 13511, 11776,
    10364, 9165,  8118,  7186,  6344,
    5575,  4866,  4207,  3591,  3013,
    2467,  1949,  1458,  989,   540,
    111,   -302,  -698
};

float BMP280_AltitudeTable(float x) {
    uint8_t index;
    int y1, y2;
    float dy;
/*    if ( x < 0 ) {
        return table_alti[0];
    }
    index = x / 50;
    if ( index >= 22 ) {
        return table_alti[22];
    }*/
    if (x < 0 || x > 1100) return 0.0;
    index = x / 50;
    y2 = table_alti[index + 1];
    y1 = table_alti[index];
    dy = y2 - y1; dy *= (x - index * 50.0) / 50.0; dy += y1;  
    //dy = (y2 - y1) * (x - index * 50.0) / 50.0 + y1;  
    return dy;
}

