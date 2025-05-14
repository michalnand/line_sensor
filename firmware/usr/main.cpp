#include "device.h"

#include <gpio.h>
#include <terminal.h>
#include <clock.h>

#include <line_sensor.h>
 


void delay_loops(uint32_t loops)
{
  while (loops--)
  {
    __asm("nop");
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
    //Gpio<TGPIOB, 3, GPIO_MODE_OUT> led_user;
    led_user = 1;
    
        
    Terminal terminal;
    terminal.init(115200, USART1);
    terminal << "terminal init done\n";

    LineSensor line_sensor;
    line_sensor.init();

    //line_sensor.set_mode(LED_OFF);
    //line_sensor.set_mode(LED_ON);
    line_sensor.set_mode(LED_ALTERNATE);
    //line_sensor.set_mode(LED_RNG);

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




    /*
    I2CSlave i2c_slave;
    i2c_slave.init();

    while (1)
    {
      i2c_slave.pool();
    }
    */
  

    return 0;
} 