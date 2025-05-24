#include "device.h"

#include <gpio.h>
#include <terminal.h>
#include <clock.h>

#include <line_sensor.h>

#include <i2c_slave.h>
#include <i2c_regs.h>


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

int main(void)
{
    //cpu clock init
    SystemInit();

    // setup PLL, 6*HSI = 48MHz
    SetPLL();  

    __enable_irq();

    Gpio<TGPIOA, 8, GPIO_MODE_OUT> led_user;
    led_user = 1;
    
    /*
    Terminal terminal;
    terminal.init(115200, USART1);
    terminal << "terminal init done\n";

    while (1)
    {
      led_user = 1;

      terminal << ">>>\n";

      for (unsigned int i = 0; i < SENSORS_COUNT; i++)
      {
        int32_t d  = line_sensor.led_dif_result[i];
        terminal << d << " ";
      }
      
      terminal << "\n";

      for (unsigned int i = 0; i < SENSORS_COUNT; i++)
      {
        int32_t d  = line_sensor.led_dif_fil_result[i];
        terminal << d << " ";
      }
      
      terminal << "\n\n";

      led_user = 0;

      delay_loops(1000);
    }
    */

    /*
    I2CSlave i2c_slave;
    i2c_slave.init();

    while (1)
    {
      __asm("nop");
    }
    */

    
    I2CSlave i2c_slave;
    i2c_slave.init();
    
    LineSensor line_sensor;
    line_sensor.init();
    line_sensor.set_mode(LED_OFF);
    line_sensor.set_filter(7);

    i2c_slave.write_reg(LS_WHOAMI_REG, WHO_AM_I_VALUE); 
    i2c_slave.write_reg(LS_CONFIG0_REG, LED_OFF);
    i2c_slave.write_reg(LS_FILTER_REG, 7);  

    while (1)
    {    
      if (i2c_slave.is_write_flag())
      {
        uint8_t mode = i2c_slave.read_reg(LS_CONFIG0_REG);

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


        uint8_t filter = i2c_slave.read_reg(LS_FILTER_REG);
        line_sensor.set_filter(filter);
      }

      
      update_regs(i2c_slave.regs, LS_RAW_OFF_REG, line_sensor.led_off_result);
      update_regs(i2c_slave.regs, LS_RAW_ON_REG,  line_sensor.led_on_result);
      
      update_regs(i2c_slave.regs, LS_FIL_OFF_REG, line_sensor.led_off_fil_result);
      update_regs(i2c_slave.regs, LS_FIL_ON_REG,  line_sensor.led_on_fil_result);

      update_regs(i2c_slave.regs, LS_DIF_RAW_REG, line_sensor.led_dif_result);
      update_regs(i2c_slave.regs, LS_DIF_FIL_REG, line_sensor.led_dif_fil_result);
    }
    
  

    return 0;
} 