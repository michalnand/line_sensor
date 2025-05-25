#include "drivers.h"
#include "common.h"
#include "tmath.h"

#include "i2c_regs.h"
#include "i2c_slave.h"

#include "line_sensor.h"



void update_regs(uint8_t *dest_ptr, uint32_t dest_adr, int32_t *source)
{
    for (unsigned int i = 0; i < 10; i++)
    {
        uint16_t tmp  = source[i];  

        uint8_t res_h = (tmp>>8)&0xff;
        uint8_t res_l = (tmp>>0)&0xff;

        // update must be atomic
        __disable_irq();
        dest_ptr[dest_adr + 2*i + 0] = res_h;
        dest_ptr[dest_adr + 2*i + 1] = res_l;
        __enable_irq();
    }
}

void debug_fil_regs(uint8_t *dest_ptr, uint32_t dest_adr)
{
    uint16_t ofs = 0;

    if (dest_adr == LS_RAW_OFF_REG)
    {
        ofs = 100;
    }

    if (dest_adr == LS_RAW_ON_REG)
    {
        ofs = 200;
    }

    if (dest_adr == LS_FIL_OFF_REG)
    {
        ofs = 300;
    }

    if (dest_adr == LS_FIL_ON_REG)
    {
        ofs = 400;
    }

    if (dest_adr == LS_DIF_RAW_REG)
    {
        ofs = 500;
    }

    if (dest_adr == LS_DIF_FIL_REG)
    {
        ofs = 600;
    }



    for (unsigned int i = 0; i < LS_DATA_SIZE; i++)
    {
        uint16_t tmp  = i + ofs;  

        uint8_t res_h = (tmp>>8)&0xff;
        uint8_t res_l = (tmp>>0)&0xff;

        // update must be atomic
        __disable_irq();
        dest_ptr[2*i + 0 + dest_adr] = res_h;
        dest_ptr[2*i + 1 + dest_adr] = res_l;
        __enable_irq();
    }
}   



int main() 
{       
    drivers_init();   

    Gpio<TGPIOC, 6, GPIO_MODE_OUT> led;

    /*
    I2CSlave i2c;
    i2c.init();

    i2c.buffer[LS_WHOAMI_REG] = WHO_AM_I_VALUE;
    
    
    
    i2c.buffer[0x10] = 3;
    i2c.buffer[0x11] = 1;
    i2c.buffer[0x12] = 4;
    i2c.buffer[0x13] = 1;
    i2c.buffer[0x14] = 5;
    i2c.buffer[0x15] = 9;
    i2c.buffer[0x16] = 2;

    LineSensor line_sensor;
    line_sensor.init();
    line_sensor.set_mode(LED_ALTERNATE);
    //line_sensor.set_mode(LED_ON);
    //line_sensor.set_mode(LED_RNG);

    
    while (1)
    {
        __asm("nop");
        __asm("wfi");
    }
    */    


    I2CSlave i2c;
    i2c.init();

    
    /*
    LineSensor line_sensor;
    line_sensor.init();
    line_sensor.set_mode(LED_ALTERNATE);
    line_sensor.set_filter(7);
    */
    

    i2c.buffer[LS_WHOAMI_REG]  = WHO_AM_I_VALUE; 
    i2c.buffer[LS_CONFIG0_REG] = LED_ALTERNATE;
    i2c.buffer[LS_FILTER_REG]  = 7;  

    while (1)
    {
        /*
        if (i2c.is_write_flag())
        {
            uint8_t mode = i2c.buffer[LS_CONFIG0_REG];

            if (mode == LED_OFF)
            {
                line_sensor.set_mode(LED_OFF);
            }
            else if (mode == LED_ON)
            {
                line_sensor.set_mode(LED_ON);
            }
            else if (mode == LED_ALTERNATE)
            {
                line_sensor.set_mode(LED_ALTERNATE);
            }
            else 
            {
                line_sensor.set_mode(LED_RNG);
            }

            uint8_t filter = i2c.buffer[LS_FILTER_REG];
            line_sensor.set_filter(filter);
        }
        */

        /*
        // refresh data
        update_regs(i2c.buffer, LS_RAW_OFF_REG, line_sensor.led_off_result);
        update_regs(i2c.buffer, LS_RAW_ON_REG,  line_sensor.led_on_result);
        
        update_regs(i2c.buffer, LS_FIL_OFF_REG, line_sensor.led_off_fil_result);
        update_regs(i2c.buffer, LS_FIL_ON_REG,  line_sensor.led_on_fil_result);
  
        update_regs(i2c.buffer, LS_DIF_RAW_REG, line_sensor.led_dif_result);
        update_regs(i2c.buffer, LS_DIF_FIL_REG, line_sensor.led_dif_fil_result);
        */


        debug_fil_regs(i2c.buffer, LS_RAW_OFF_REG);
        debug_fil_regs(i2c.buffer, LS_RAW_ON_REG);
        
        debug_fil_regs(i2c.buffer, LS_FIL_OFF_REG);
        debug_fil_regs(i2c.buffer, LS_FIL_ON_REG);
  
        debug_fil_regs(i2c.buffer, LS_DIF_RAW_REG);
        debug_fil_regs(i2c.buffer, LS_DIF_FIL_REG);

        // wait for next event
        __asm("wfi");
    }

    return 0;
}
