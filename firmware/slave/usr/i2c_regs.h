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
#define LS_RAW_ON_REG     ((uint8_t)0x18)  //base + 1*20

#define LS_FIL_OFF_REG    ((uint8_t)0x2c)  //base + 2*20
#define LS_FIL_ON_REG     ((uint8_t)0x40)  //base + 3*20

#define LS_DIF_RAW_REG    ((uint8_t)0x54)  //base + 4*20
#define LS_DIF_FIL_REG    ((uint8_t)0x68)  //base + 5*20




#define WHO_AM_I_VALUE    ((uint8_t)171)
#define LS_DATA_SIZE      ((uint8_t)10)

#endif