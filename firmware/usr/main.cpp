#include "device.h"
#include <gpio.h>

#include <adc.h>
#include <i2c.h>
#include <terminal.h>

#include <i2c_slave.h>
 
void SetPLL()
{
  RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_12);
  RCC_PLLCmd(ENABLE); 

  // Wait for PLLRDY after enabling PLL.
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET)
  { 
    __asm("nop");
  } 

  // Select the PLL as clock source.
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  
  SystemCoreClockUpdate();
}


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
    led_user = 1;

        
    Terminal terminal;
    terminal.init(115200, USART1);
    terminal << "terminal init done\n";


    I2CSlave i2c_slave;
    i2c_slave.init();

    while (1)
    {
      i2c_slave.pool();
    }
  

    return 0;
} 