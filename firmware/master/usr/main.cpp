#include "drivers.h"
#include "common.h"
#include "tmath.h"

#include "i2c_regs.h"


Terminal terminal;
Timer timer;

TI2C<TGPIOA, 0, 1> i2c;
SSD1315 oled;




uint8_t read_who_am_i()
{
    return i2c.read_reg(LS_I2C_ADDR<<1, LS_WHOAMI_REG);
}

void read_data_burst(uint16_t *result_ptr, uint8_t adr)
{
    uint8_t buffer[2*LS_DATA_SIZE];

    
    i2c.read_reg_multi(LS_I2C_ADDR<<1, adr, buffer, 2*LS_DATA_SIZE);

    for (unsigned int i = 0; i < LS_DATA_SIZE; i++)
    {
        uint16_t tmp = 0;
        tmp = ((uint16_t)buffer[2*i + 0])<<8;
        tmp+= ((uint16_t)buffer[2*i + 1]);

        result_ptr[i] = tmp;
    }
}



void read_data_single(uint16_t *result_ptr, uint8_t adr)
{
    uint8_t buffer[2*LS_DATA_SIZE];

    
    for (unsigned int i = 0; i < LS_DATA_SIZE; i++)
    {
        uint16_t tmp_h = i2c.read_reg(LS_I2C_ADDR<<1, adr + 2*i + 0);
        uint16_t tmp_l = i2c.read_reg(LS_I2C_ADDR<<1, adr + 2*i + 1);

        result_ptr[i] = (tmp_h<<8)|tmp_l;
    }
}

void print_data(uint16_t *result_ptr)
{
    for (unsigned int i = 0; i < LS_DATA_SIZE; i++)
    {
        terminal << result_ptr[i] << " ";
    }

    terminal << "\n";
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

    //i2c.write_reg()


    uint16_t result_ptr[LS_DATA_SIZE];

    while (1)
    {
        led = 1;
        uint8_t who_am_i = read_who_am_i();

        oled.put_info("id  ", who_am_i, 0);
        terminal << "who am i " << who_am_i << "\n";


        read_data_burst(result_ptr, LS_RAW_OFF_REG);
        print_data(result_ptr);

        read_data_burst(result_ptr, LS_RAW_ON_REG);
        print_data(result_ptr);


        read_data_burst(result_ptr, LS_FIL_OFF_REG);
        print_data(result_ptr);

        read_data_burst(result_ptr, LS_FIL_ON_REG);
        print_data(result_ptr);


        read_data_burst(result_ptr, LS_DIF_RAW_REG);
        print_data(result_ptr);

        read_data_burst(result_ptr, LS_DIF_FIL_REG);
        print_data(result_ptr);


     

        terminal << "\n\n";
        
        led = 0;

        timer.delay_ms(300);
    }
    
    return 0;
}
