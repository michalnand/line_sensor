#include "drivers.h"
#include "common.h"
#include "tmath.h"

#include "i2c_regs.h"
#include "i2c_slave.h"

#include "line_sensor.h"

void update_reg(uint8_t *dest_ptr, uint8_t reg_adr, uint16_t value)
{
    uint8_t res_h = (value>>8)&0xff;
    uint8_t res_l = (value>>0)&0xff;    

    // update must be atomic
    __disable_irq();    
    dest_ptr[reg_adr + 0] = res_h;
    dest_ptr[reg_adr + 1] = res_l;
    __enable_irq(); 
}

void update_regs(uint8_t *dest_ptr, uint32_t dest_adr, int16_t *source)
{
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        uint16_t tmp = source[i];  
        update_reg(dest_ptr, 2*i + dest_adr, tmp);
    }
}



int16_t compute_mean(int16_t *values)
{   
    int32_t result = 0;

    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        __disable_irq();
        int16_t tmp = values[i];
        __enable_irq(); 

        result+= tmp;
    }

    result = result/(int32_t)SENSORS_COUNT;

    return result;
}


int main() 
{       
    drivers_init();   

    Gpio<'A', 8, GPIO_MODE_OUT> led;
    led = 0;

    
    
    LineSensor line_sensor;
    line_sensor.init();
    line_sensor.set_mode(LED_ALTERNATE);
    line_sensor.set_filter(7);
    

    
    I2CSlave i2c;
    i2c.init();

    
    i2c.buffer[LS_WHOAMI_REG]  = WHO_AM_I_VALUE; 
    i2c.buffer[LS_CONFIG0_REG] = LED_ALTERNATE;
    i2c.buffer[LS_FILTER_REG]  = 7;  
    


    while (1)
    {   
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
        
        
        // refresh data
        update_regs(i2c.buffer, LS_RAW_OFF_REG, line_sensor.led_off_result);
        update_regs(i2c.buffer, LS_RAW_ON_REG,  line_sensor.led_on_result);
        
        update_regs(i2c.buffer, LS_FIL_OFF_REG, line_sensor.led_off_fil_result);
        update_regs(i2c.buffer, LS_FIL_ON_REG,  line_sensor.led_on_fil_result);
  
        update_regs(i2c.buffer, LS_DIF_RAW_REG, line_sensor.led_dif_result);
        update_regs(i2c.buffer, LS_DIF_FIL_REG, line_sensor.led_dif_fil_result);
        


        // refresh statistics   
        update_reg(i2c.buffer, LS_STATS_0_REG, line_sensor.mean_result);
        update_reg(i2c.buffer, LS_STATS_1_REG, line_sensor.var_result);
        update_reg(i2c.buffer, LS_STATS_2_REG, line_sensor.min_result);
        update_reg(i2c.buffer, LS_STATS_3_REG, line_sensor.max_result);

        // wait for next event
        __asm("wfi");
    }
    

    return 0;
}
