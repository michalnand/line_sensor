#ifndef _I2CSlave_H_
#define _I2CSlave_H_

#include "drivers.h"


#define I2C_SLAVE_ADDRESS   ((uint8_t)0x42)
#define I2C_BUFFER_SIZE     ((uint32_t)256)


class I2CSlave
{
    public:
        void init();

    private:
        void _i2c_init();

    public:
        uint8_t ptr, state;

    public:
        uint8_t buffer[I2C_BUFFER_SIZE];
};


#endif