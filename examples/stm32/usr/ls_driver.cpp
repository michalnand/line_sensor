#include "ls_driver.h"


int LSDriver::init(I2C_Interface &i2c)
{
    this->i2c = &i2c;

    // sensor reset
    Gpio<'A', 7, GPIO_MODE_OUT> ls_reset_pin;

    ls_reset_pin = 0;
    delay_loops(10000);
    ls_reset_pin = 1;
    delay_loops(100000);

    


    if (read_who_am_i() == WHO_AM_I_VALUE)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


/*
   validate if device is connected and responding
   must returns value WHO_AM_I_VALUE (171 dec)
*/
uint8_t LSDriver::read_who_am_i()
{
    return i2c->read_reg(LS_I2C_ADDR<<1, LS_WHOAMI_REG);
}


/*
  fill buffer with uint16_t values from sensors,
  result_buffer : uint16_t, buffer size is LS_DATA_SIZE (10 sensors)
  adr           : one of the data registers address : LS_RAW_OFF_REG, LS_RAW_ON_REG .. LS_DIF_FIL_REG
*/
void LSDriver::read_data(uint16_t *result_buffer, unsigned char adr)
{   
    
    i2c->start();
    i2c->write(LS_I2C_ADDR<<1);
    i2c->write(adr);

    i2c->start();
    i2c->write((LS_I2C_ADDR<<1)|0x01);

    for (uint8_t i = 0; i < LS_DATA_SIZE; i++)
    {
        // first read high, then lower byte
        uint16_t tmp_h = i2c->read(1);
        uint16_t tmp_l = i2c->read(1);
        
        result_buffer[i] = (tmp_h << 8)|tmp_l;
    }   

    i2c->read(0);
    i2c->stop();
    
    return;

    
    for (uint8_t i = 0; i < LS_DATA_SIZE; i++)
    {
        // first read high, then lower byte
        uint16_t tmp_h = i2c->read_reg(LS_I2C_ADDR<<1, adr + 2*i + 0);
        uint16_t tmp_l = i2c->read_reg(LS_I2C_ADDR<<1, adr + 2*i + 1);
        
        //uint16_t tmp_h = i2c_read_reg(LS_I2C_ADDR<<1, adr + 2*i + 0);
        //uint16_t tmp_l = i2c_read_reg(LS_I2C_ADDR<<1, adr + 2*i + 1);
        result_buffer[i] = (tmp_h << 8)|tmp_l;
    }
    

    return;
}

void LSDriver::delay_loops(uint32_t loops)
{
    while (loops--)
    {
        __asm volatile("nop");
    }
}