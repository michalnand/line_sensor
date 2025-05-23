#ifndef _LINE_SENSOR_H_
#define _LINE_SENSOR_H_

#include "device.h"
#include <stdint.h>
#include <gpio.h>
#include <gpio.h>
#include <adc.h>



#define SENSORS_COUNT           ((uint32_t)10)
#define SENSORS_WAIT_STATES     ((uint32_t)5)


enum LEDModulationMode
{
    LED_OFF,
    LED_ON,
    LED_ALTERNATE,
    LED_RNG
};

class LineSensor
{
    public:
        LineSensor();

        void init();
        void set_mode(LEDModulationMode mode);
        void set_filter(uint8_t value);

        uint32_t callback(int32_t value);

    private:
        void _init_vars();
        
        void _gpio_init();
        void _nvic_init();
        void _adc_init();
        
        bool _next_led_state();

        uint8_t _lfsr_rnd();
        void _processing();

    private:
        Gpio<TGPIOB, 2, GPIO_MODE_OUT> led_control;
        bool led_state;

        int32_t filter_coeff;
        uint32_t channel, state;

        uint32_t lfsr;

        LEDModulationMode mode;


    public:
        int32_t led_off_result[SENSORS_COUNT];
        int32_t led_on_result[SENSORS_COUNT];
        
        int32_t led_off_fil_result[SENSORS_COUNT];
        int32_t led_on_fil_result[SENSORS_COUNT];

        int32_t led_dif_result[SENSORS_COUNT];
        int32_t led_dif_fil_result[SENSORS_COUNT];
};


#endif