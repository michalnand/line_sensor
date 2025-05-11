#ifndef _LINE_SENSOR_H_
#define _LINE_SENSOR_H_

#include "device.h"
#include <stdint.h>
#include <gpio.h>
#include <gpio.h>
#include <adc.h>


#define SENSORS_COUNT   ((uint32_t)2)

class LineSensor
{
    public:
        void init();
        
        uint32_t callback(int32_t value);

    private:
        void _gpio_init();
        void _nvic_init();
        void _adc_init();
        bool _next_led_state();
        uint8_t _lfsr_rnd();

    private:
        Gpio<TGPIOB, 4, GPIO_MODE_OUT> led_control;
        bool led_state;
        uint32_t channel, state;

        uint32_t lfsr;


    public:
        int32_t led_off_result[SENSORS_COUNT];
        int32_t led_on_result[SENSORS_COUNT];
        int32_t led_dif_result[SENSORS_COUNT];
        int32_t led_dif_fil_result[SENSORS_COUNT];
};


#endif