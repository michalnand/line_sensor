#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

#include "device.h"


#define I2C_SLAVE_ADDRESS  0x30  // 7-bit address

#define I2C_MEM_SIZE       ((uint32_t)256)

class I2CSlave
{
    public:
        void init();
        void pool();

    private:
        void _gpio_init();
        void _slave_init();

        void _i2c_write_reg(uint8_t reg_adr, uint8_t value);
        uint8_t _i2c_read_reg(uint8_t reg_adr);

    public:
        uint8_t regs[I2C_MEM_SIZE];
};
    

#endif