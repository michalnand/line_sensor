#include "drivers.h"
#include "common.h"
#include "tmath.h"

#include "i2c_regs.h"
#include "i2c_slave.h"

#include "line_sensor.h"

int main() 
{       
    drivers_init();   

    Gpio<TGPIOC, 6, GPIO_MODE_OUT> led;

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
    //line_sensor.set_mode(LED_ALTERNATE);
    //line_sensor.set_mode(LED_ON);
    line_sensor.set_mode(LED_RNG);

    
    while (1)
    {
        __asm("nop");
        __asm("wfi");
    }
    
    return 0;
}
