#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

#include "device.h"


#define I2C_SLAVE_ADDRESS  ((uint8_t)0x42)  // 7-bit address
#define I2C_MEM_SIZE       ((uint32_t)256)

class I2CSlave
{
    public:
        void init();
        void write_reg(uint8_t reg_adr, uint8_t value);
        uint8_t read_reg(uint8_t reg_adr);
        bool is_write_flag();
       
    private:
        void _data_init();
        void _gpio_init();
        void _slave_init();
    
    public:
        uint8_t *regs;
};
    

#endif