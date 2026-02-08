#include "drivers.h"
#include "common.h"
#include "ls_driver.h"


Terminal terminal;
Timer timer;

TI2C<'A', 10, 9, 5> i2c;


    
// debug data print
void print_data(uint16_t *result_buffer)
{
  for (uint8_t i = 0; i < LS_DATA_SIZE; i++)
  {
    uint16_t tmp = result_buffer[i];
    terminal << tmp << " ";
  }

  terminal << "\n\n";
}

int main() 
{       
    drivers_init();   
    uart_init(); 

    terminal << "\n\nuart init done\n";

    timer.init();

    i2c.init();

    
    terminal << "i2c init done\n";

   

    LSDriver ls_driver;

    int init_res = ls_driver.init(i2c);
    terminal << "ls init result " << init_res << "\n";
    
    
    uint16_t sensor_reading[LS_DATA_SIZE];

    while (1)
    {
        uint8_t result = ls_driver.read_who_am_i();
        terminal << "who am i reg : " << (int)result << "\n";
    
        // perform 20 measurements, and estimate sensor reading speed
        uint32_t time_start = timer.get_time();
        for (unsigned int i = 0; i < 20; i++)
        {
            ls_driver.read_data(sensor_reading, LS_DIF_FIL_REG);
        }
        uint32_t time_stop = timer.get_time();

        uint32_t rps = ((uint32_t)20*(uint32_t)1000)/(time_stop - time_start);

        terminal << "readings per second " << rps << "\n";
    
    
        terminal << "readed data\n";

        ls_driver.read_data(sensor_reading, LS_RAW_OFF_REG);
        print_data(sensor_reading);

        ls_driver.read_data(sensor_reading, LS_RAW_ON_REG);
        print_data(sensor_reading);

        ls_driver.read_data(sensor_reading, LS_FIL_OFF_REG);
        print_data(sensor_reading);

        ls_driver.read_data(sensor_reading, LS_FIL_ON_REG);
        print_data(sensor_reading);

        ls_driver.read_data(sensor_reading, LS_DIF_RAW_REG);
        print_data(sensor_reading);

        ls_driver.read_data(sensor_reading, LS_DIF_FIL_REG);
        print_data(sensor_reading);

        ls_driver.read_data(sensor_reading, LS_TEST_DAT_REG);
        print_data(sensor_reading);

        terminal << "\n\n\n\n";

        timer.delay_ms(500);
    }
    
    
    return 0;
}
