#ifndef _I2C_REGS_H_
#define _I2C_REGS_H_

#include <stdint.h>

// slave address
#define LS_I2C_ADDR       ((uint8_t)0x42)

// main registers
#define LS_WHOAMI_REG     ((uint8_t)0x00)  //ID reg, readed as WHO_AM_I_VALUE (0x6A)
#define LS_CONFIG0_REG    ((uint8_t)0x01)  //config 0, led drive modes
#define LS_CONFIG1_REG    ((uint8_t)0x02)  //config 1, not used
#define LS_FILTER_REG     ((uint8_t)0x03)  //filter coeff, 0..255

//data registers
#define LS_RAW_OFF_REG    ((uint8_t)0x04)  //data starting, base
#define LS_RAW_ON_REG     ((uint8_t)0x20)  //base + 1*28

#define LS_FIL_OFF_REG    ((uint8_t)0x3c)  //base + 2*28
#define LS_FIL_ON_REG     ((uint8_t)0x58)  //base + 3*28

#define LS_DIF_RAW_REG    ((uint8_t)0x74)  //base + 4*28
#define LS_DIF_FIL_REG    ((uint8_t)0x90)  //base + 5*28

//statistics    
#define LS_STATS_0_REG    ((uint8_t)0xac)  //mean value, 16bit
#define LS_STATS_1_REG    ((uint8_t)0xae)  //var value, 16bit
#define LS_STATS_2_REG    ((uint8_t)0xb0)  //min value, 16bit
#define LS_STATS_3_REG    ((uint8_t)0xb2)  //max value, 16bit




#define WHO_AM_I_VALUE    ((uint8_t)171)
#define LS_DATA_SIZE      ((uint8_t)10)

#endif