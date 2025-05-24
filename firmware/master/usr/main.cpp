#include "drivers.h"
#include "common.h"
#include "tmath.h"

#include "i2c_regs.h"


Terminal terminal;
Timer timer;

TI2C<TGPIOA, 0, 1> i2c;
SSD1315 oled;




void read_reg_test(uint8_t reg_adr, uint8_t line)
{
    // random reads 
    //i2c.read_reg(LS_I2C_ADDR<<1, LS_WHOAMI_REG + 1);
    //i2c.read_reg(LS_I2C_ADDR<<1, LS_WHOAMI_REG + 20);
    //i2c.read_reg(LS_I2C_ADDR<<1, LS_WHOAMI_REG + 31);
    //i2c.read_reg(LS_I2C_ADDR<<1, LS_WHOAMI_REG + 50); 
   
    uint8_t data_a = i2c.read_reg(LS_I2C_ADDR<<1, reg_adr);
    uint8_t data_b = i2c.read_reg(LS_I2C_ADDR<<1, reg_adr);

    terminal << (int)data_a << " " << (int)data_b << "\n";

    oled.put_info("first ",  data_a, 2*line + 0);
    oled.put_info("second ", data_b, 2*line + 1);
}




void read_reg_single()
{
    volatile uint8_t data_a;
    volatile uint8_t data_b;
    volatile uint8_t data_c;
    volatile uint8_t data_d;
    volatile uint8_t data_e;
    volatile uint8_t data_f;
    volatile uint8_t data_g;

    i2c.read_reg(LS_I2C_ADDR<<1, 0x0F);
   
    data_a = i2c.read_reg(LS_I2C_ADDR<<1, 0x10);
    data_b = i2c.read_reg(LS_I2C_ADDR<<1, 0x11);
    data_c = i2c.read_reg(LS_I2C_ADDR<<1, 0x12);
    data_d = i2c.read_reg(LS_I2C_ADDR<<1, 0x13);
    data_e = i2c.read_reg(LS_I2C_ADDR<<1, 0x14);
    data_f = i2c.read_reg(LS_I2C_ADDR<<1, 0x15);
    data_g = i2c.read_reg(LS_I2C_ADDR<<1, 0x16);  
    

    oled.put_string("single mode  ", 0);
    oled.put_info("0x10 ",  data_a, 1); 
    oled.put_info("0x11 ",  data_b, 2);
    oled.put_info("0x12 ",  data_c, 3);
    oled.put_info("0x13 ",  data_d, 4);
    oled.put_info("0x14 ",  data_e, 5); 
    oled.put_info("0x15 ",  data_f, 6); 
    oled.put_info("0x16 ",  data_g, 7);   
    
    
    terminal << "single mode  " << "\n";
    terminal << "0x10 " <<  (int)data_a << "\n";
    terminal << "0x11 " <<  (int)data_b << "\n";
    terminal << "0x12 " <<  (int)data_c << "\n";
    terminal << "0x13 " <<  (int)data_d << "\n";
    terminal << "0x14 " <<  (int)data_e << "\n";
    terminal << "0x15 " <<  (int)data_f << "\n";
    terminal << "0x16 " <<  (int)data_g << "\n";
    terminal << "\n\n"; 
}



void read_reg_burst()
{    
    unsigned char buffer[7];

    i2c.read_reg(LS_I2C_ADDR<<1, 0x0F);

    i2c.read_reg_multi(LS_I2C_ADDR<<1, 0x10, buffer, 7);


    oled.put_string("burst mode  ", 0);
    oled.put_info("0x10 ",  buffer[0], 1);
    oled.put_info("0x11 ",  buffer[1], 2);
    oled.put_info("0x12 ",  buffer[2], 3);
    oled.put_info("0x13 ",  buffer[3], 4);
    oled.put_info("0x14 ",  buffer[4], 5); 
    oled.put_info("0x15 ",  buffer[5], 6); 
    oled.put_info("0x16 ",  buffer[6], 7); 
    

    terminal << "burst mode  " << "\n";
    terminal << "0x10 " <<  (int)buffer[0] << "\n";
    terminal << "0x11 " <<  (int)buffer[1] << "\n";
    terminal << "0x12 " <<  (int)buffer[2] << "\n";
    terminal << "0x13 " <<  (int)buffer[3] << "\n";
    terminal << "0x14 " <<  (int)buffer[4] << "\n";
    terminal << "0x15 " <<  (int)buffer[5] << "\n";
    terminal << "0x16 " <<  (int)buffer[6] << "\n";
    terminal << "\n\n";   
}




int main() 
{       
    drivers_init();   
    uart_init(); 

    Gpio<TGPIOC, 6, GPIO_MODE_OUT> led;

    led = 1;

    //Terminal terminal;
    terminal << "\n\nuart init done\n";

    timer.init();
    i2c.init();
    oled.init(i2c);

    while (1)
    {
        led = 1;
        read_reg_single();
        led = 0;

        timer.delay_ms(500);

        
        led = 1;
        read_reg_burst();
        led = 0;

        timer.delay_ms(500);

        
    }
    
    
    
    return 0;
}
