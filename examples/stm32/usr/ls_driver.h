#ifndef _LS_DRIVER_H_
#define _LS_DRIVER_H_

#include "drivers.h"
#include "ls_i2c_regs.h"

class LSDriver
{
    public:
        /*
            returns 0 if readed correct whoamI value
            otherwise returns negative value
        */
        int init(I2C_Interface &i2c);


        /*
        validate if device is connected and responding
        must returns value WHO_AM_I_VALUE (171 dec)
        */
        uint8_t read_who_am_i();

        /*
            fill buffer with uint16_t values from sensors,
            result_buffer : uint16_t, buffer size is LS_DATA_SIZE (10 sensors)
            adr           : one of the data registers address : LS_RAW_OFF_REG, LS_RAW_ON_REG .. LS_DIF_FIL_REG
        */
        void read_data(uint16_t *result_buffer, unsigned char adr);

    private:
        void delay_loops(uint32_t loops);

    private:
        I2C_Interface *i2c;

};

#endif