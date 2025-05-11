#include "line_sensor.h"



#ifdef __cplusplus
extern "C" {
#endif


LineSensor *g_line_sensor;

void ADC1_IRQHandler(void) 
{
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC)) 
    {
        int32_t value = ADC_GetConversionValue(ADC1);

        uint32_t current_channel = g_line_sensor->callback(value);

        // set next input        
        ADC_ChannelConfig(ADC1, current_channel, ADC_SampleTime_71_5Cycles);


        // clear flag and start next conversion
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
        ADC_StartOfConversion(ADC1);    
    }
}


#ifdef __cplusplus
}
#endif


void LineSensor::init()
{
    g_line_sensor = this;

    led_control = 0;
    
    this->state         = 0;
    this->led_state     = false;
    this->channel       = 0;


    this->led_control   = 0;

    this->lfsr = 0xA5A5A5A5;


    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        this->led_off_result[i]     = 4096;
        this->led_on_result[i]      = 0;
        this->led_dif_result[i]     = 0;
        this->led_dif_fil_result[i] = 0;
    }


    this->_gpio_init();
    this->_adc_init();
    this->_nvic_init();

    ADC_StartOfConversion(ADC1); 
}



uint32_t LineSensor::callback(int32_t value)
{
    if (this->state == 0)
    {
        this->led_state = this->_next_led_state();
        
        // turn on or off led
        if (this->led_state)
        {
            this->led_control = 1;
        }
        else
        {
            this->led_control = 0;
        }
        
        this->channel       = 0;
        this->state         = 1;

        int32_t filter_coeff = 7;
        for (unsigned int i = 0; i < SENSORS_COUNT; i++)
        {
            this->led_dif_result[i]     = this->led_off_result[i] - this->led_on_result[i];
            this->led_dif_fil_result[i] = (filter_coeff*this->led_dif_fil_result[i] + this->led_dif_result[i])/(filter_coeff+1);
        }
    }

    // waiting states
    else if (this->state < 10)
    {
        this->state++;
    }
    // read channels state
    else
    {


        if (this->led_state)
        {
            this->led_on_result[this->channel] = value;
        }
        else
        {
            this->led_off_result[this->channel] = value;
        }   
        
        this->channel++;
        if (this->channel >= SENSORS_COUNT)
        {
            this->channel = 0;
            this->state   = 0;
        }
    }

    return this->channel;
}


void LineSensor::_gpio_init(void) 
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    // ADC_IN0 - ADC_IN7 are on GPIOA0 - GPIOA7
    // ADC_IN8 - ADC_IN9 are on GPIOB0 - GPIOB1

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

    // GPIOA0-A7
    //gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOA, &gpio);

    // GPIOB0-B1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &gpio);
}



void LineSensor::_nvic_init() 
{
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = ADC1_IRQn;
    nvic.NVIC_IRQChannelPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}



void LineSensor::_adc_init() 
{
    ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_DeInit(ADC1);

    // Initialize ADC structure
    ADC_StructInit(&ADC_InitStructure);
    
    // Configure the ADC1 in continuous mode with a resolution equal to 12 bits
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
    ADC_Init(ADC1, &ADC_InitStructure); 
    
    
    ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_71_5Cycles);


    // ADC Calibration
    ADC_GetCalibrationFactor(ADC1);

    // End of conversion interrupt
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); 

    
    // Enable the ADC peripheral
    ADC_Cmd(ADC1, ENABLE);     
    
    // Wait the ADRDY flag
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY))
    {
        __asm("nop");
    }

}


bool LineSensor::_next_led_state()
{
    //return true;
    
    //lfsr++;
    //return (lfsr%2);

    return _lfsr_rnd();
}


// Generate next bit (0 or 1)
uint8_t LineSensor::_lfsr_rnd() 
{
    // XOR taps: bits 31, 21, 1, 0 (0-indexed)
    uint8_t lfsr_bit = ((lfsr >> 31) ^ (lfsr >> 21) ^ (lfsr >> 1) ^ (lfsr >> 0)) & 1;
    lfsr = (lfsr << 1) | lfsr_bit;
    return lfsr_bit;
}