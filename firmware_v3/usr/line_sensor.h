#ifndef _LINE_SENSOR_H_
#define _LINE_SENSOR_H_

#include "drivers.h"

#define SENSORS_COUNT           ((uint32_t)14)
#define SENSORS_WAIT_STATES     ((uint32_t)10)  

class LineSensor
{
    public:
        LineSensor();

        void init();
        void set_filter(uint8_t value);

        uint32_t callback(int32_t value);

    private:
        void _init_vars();
        
        void _gpio_init();
        void _nvic_init();
        void _adc_init();
        

        void _processing();

    public:
        Gpio<'C', 6, GPIO_MODE_OUT> led_control;

        bool led_state;

        int32_t filter_coeff;
        uint32_t channel, state;


        uint32_t channels_map[SENSORS_COUNT];

        
    public:
        int16_t led_off_result[SENSORS_COUNT];
        int16_t led_on_result[SENSORS_COUNT];
        
        int16_t led_off_fil_result[SENSORS_COUNT];
        int16_t led_on_fil_result[SENSORS_COUNT];

        int16_t led_dif_result[SENSORS_COUNT];
        int16_t led_dif_fil_result[SENSORS_COUNT];
};


#endif