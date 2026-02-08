#ifndef _LS_I2C_REGS_H_
#define _LS_I2C_REGS_H_

#include <stdint.h>
    

#define WHO_AM_I_VALUE    ((uint8_t)171)
#define LS_DATA_SIZE      ((uint8_t)14)              // num of numerical values

#define LS_BLOCK_SIZE     ((uint8_t)2*LS_DATA_SIZE)  // num of bytes for data segment



// slave address
#define LS_I2C_ADDR       ((uint8_t)0x42)

// main registers
#define LS_WHOAMI_REG     ((uint8_t)0x00)  //ID reg, readed as WHO_AM_I_VALUE
#define LS_CONFIG0_REG    ((uint8_t)0x01)  //config 0, led drive modes
#define LS_CONFIG1_REG    ((uint8_t)0x02)  //config 1, not used
#define LS_FILTER_REG     ((uint8_t)0x03)  //filter coeff, 0..255

//data registers
#define LS_BASE_REG       ((uint8_t) 0x04)           // data starting, base

#define LS_RAW_OFF_REG  (LS_BASE_REG + 0*LS_BLOCK_SIZE)
#define LS_RAW_ON_REG   (LS_BASE_REG + 1*LS_BLOCK_SIZE)
#define LS_FIL_OFF_REG  (LS_BASE_REG + 2*LS_BLOCK_SIZE)
#define LS_FIL_ON_REG   (LS_BASE_REG + 3*LS_BLOCK_SIZE)
#define LS_DIF_RAW_REG  (LS_BASE_REG + 4*LS_BLOCK_SIZE)
#define LS_DIF_FIL_REG  (LS_BASE_REG + 5*LS_BLOCK_SIZE)
#define LS_TEST_DAT_REG (LS_BASE_REG + 6*LS_BLOCK_SIZE)

//statistics    
#define LS_STATS_BASE_REG ((uint8_t)(LS_BASE_REG + 7*LS_BLOCK_SIZE)) // 0xC8

#define LS_STATS_0_REG    (LS_STATS_BASE_REG + 0x00)  // mean, 16-bit
#define LS_STATS_1_REG    (LS_STATS_BASE_REG + 0x02)  // var,  16-bit
#define LS_STATS_2_REG    (LS_STATS_BASE_REG + 0x04)  // min,  16-bit
#define LS_STATS_3_REG    (LS_STATS_BASE_REG + 0x06)  // max,  16-bit


#endif