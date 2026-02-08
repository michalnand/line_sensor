#include "line_sensor.h"



#ifdef __cplusplus
extern "C" {
#endif

LineSensor *g_line_sensor;


void ADC1_IRQHandler(void) 
{
    if (LL_ADC_IsActiveFlag_EOC(ADC1))
    {
        uint32_t value = LL_ADC_REG_ReadConversionData12(ADC1);
        LL_ADC_ClearFlag_EOC(ADC1);

        uint32_t next_channel = g_line_sensor->callback(value);

        // Change channel for next conversion
        ADC1->CHSELR = next_channel;

        // Start new conversion
        LL_ADC_REG_StartConversion(ADC1);
    }
}

#ifdef __cplusplus
}
#endif

LineSensor::LineSensor()
{

}

void LineSensor::init()
{
    g_line_sensor = this;

    this->channels_map[0] = ADC_CHSELR_CHSEL0;
    this->channels_map[1] = ADC_CHSELR_CHSEL1;
    this->channels_map[2] = ADC_CHSELR_CHSEL2;
    this->channels_map[3] = ADC_CHSELR_CHSEL3;
    this->channels_map[4] = ADC_CHSELR_CHSEL4;
    this->channels_map[5] = ADC_CHSELR_CHSEL5;
    this->channels_map[6] = ADC_CHSELR_CHSEL6;
    this->channels_map[7] = ADC_CHSELR_CHSEL7;
    this->channels_map[8] = ADC_CHSELR_CHSEL8;
    this->channels_map[9] = ADC_CHSELR_CHSEL9;

    this->channels_map[10] = ADC_CHSELR_CHSEL16;
    this->channels_map[11] = ADC_CHSELR_CHSEL15;
    this->channels_map[12] = ADC_CHSELR_CHSEL17;
    this->channels_map[13] = ADC_CHSELR_CHSEL10;

   
    this->_init_vars();

    this->_gpio_init(); 

    
    this->_adc_init();
    this->_nvic_init();

   
    
    // select first channel
    ADC1->CHSELR = ADC_CHSELR_CHSEL0;
    // Start first conversion
    LL_ADC_REG_StartConversion(ADC1);
}


void LineSensor::set_mode(LEDModulationMode mode)
{
    this->_init_vars();

    this->mode = mode;
}

void LineSensor::set_filter(uint8_t value)
{   
    this->filter_coeff = value;
}


uint32_t LineSensor::callback(int32_t value)
{
    
    // initial state
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

        this->_processing();
    }

    // waiting states
    else if (this->state < SENSORS_WAIT_STATES)
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
            this->state   = 0;
            this->channel = 0;
        }
    }
    
    return this->channels_map[this->channel];
}



void LineSensor::_init_vars()
{
    this->led_control = 0;
    
    this->state         = 0;
    this->led_state     = false;
    this->channel       = 0;
    this->filter_coeff  = 7;


    this->mode = LED_ALTERNATE;

    this->lfsr = 0xA5A5A5A5;    

    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        this->led_off_result[i]     = 4096;
        this->led_on_result[i]      = 0;

        this->led_off_fil_result[i] = 4096;
        this->led_on_fil_result[i]  = 0;

        this->led_dif_result[i]     = 0;
        this->led_dif_fil_result[i] = 0;
    }

    this->mean_result   = 0;
    this->var_result    = 0;
    this->min_result    = 0;
    this->max_result    = 0;
}

void LineSensor::_gpio_init(void) 
{
    // Enable GPIOA and GPIOB clocks for ADC channels
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

        
    // Configure PA0 - PA7 as analog (ADC_IN0 to ADC_IN7)
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_5, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_7, LL_GPIO_PULL_NO);  

    // Configure PB0, PB1 as analog (ADC_IN8, ADC_IN9)
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);


    // Configure PA11, PA12, PA13 as analog (ADC_IN15, ADC_IN16, ADC_IN17)
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_11, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_11, LL_GPIO_PULL_NO);
    
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_12, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_13, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_13, LL_GPIO_PULL_NO);

    // Configure PB2 as analog (ADC_IN10)   
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_2, LL_GPIO_PULL_NO);

}


void LineSensor::_adc_init() 
{
    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK); 

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    

    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  
    LL_ADC_Init(ADC1, &ADC_InitStruct);



    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);


    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_COMMON_1);

    /*
    LL_ADC_StartCalibration(ADC1); 

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
        __asm("nop");
    }   
    */


    LL_ADC_Enable(ADC1);


    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
    {
        __asm("nop");
    }

    LL_ADC_EnableIT_EOC(ADC1);
}




void LineSensor::_nvic_init() 
{
    NVIC_SetPriority(ADC1_IRQn, 1);
    NVIC_EnableIRQ(ADC1_IRQn);
}


bool LineSensor::_next_led_state()
{   
    if (this->mode == LED_OFF)
    {
        return false;
    }
    else if (this->mode == LED_ON)
    {
        return true;
    }
    else if (this->mode == LED_ALTERNATE)
    {
        lfsr++;
        return (lfsr%2);
    }
    else
    {
        return _lfsr_rnd();
    }
}


// Generate next bit (0 or 1)
uint8_t LineSensor::_lfsr_rnd() 
{
    // 4bit LFSR
    uint8_t bit = ((lfsr >> 3) ^ (lfsr >> 2)) & 1; // taps: 4 and 3
    lfsr = ((lfsr << 1) | bit) & 0x0F; // Keep only lower 4 bits
    
    return lfsr&1;
}



void LineSensor::_processing()
{
    // toogle debug led
    GPIOA->ODR ^= (1 << 8);

    // filter results
    int32_t filter_coeff = this->filter_coeff;
            
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        this->led_off_fil_result[i] = (filter_coeff*this->led_off_fil_result[i] + this->led_off_result[i])/(filter_coeff+1);
        this->led_on_fil_result[i]  = (filter_coeff*this->led_on_fil_result[i]  + this->led_on_result[i])/(filter_coeff+1);
    }

    // compute difference
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        this->led_dif_result[i]     = this->led_off_result[i]       - this->led_on_result[i];
        this->led_dif_fil_result[i] = this->led_off_fil_result[i]   - this->led_on_fil_result[i];
    }   

    // clip min max range into 0..4095
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        if (this->led_dif_result[i] < 0)
        {
            this->led_dif_result[i]  = 0;
        }

        if (this->led_dif_result[i] > 4095)
        {
            this->led_dif_result[i]  = 4095;
        }
    }
    

    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        if (this->led_dif_fil_result[i] < 0)
        {
            this->led_dif_fil_result[i]  = 0;
        }

        if (this->led_dif_fil_result[i] > 4095)
        {
            this->led_dif_fil_result[i]  = 4095;
        }
    }

    /*
    // compute statistics
    int32_t mean_value = 0;
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        mean_value+= this->led_dif_fil_result[i];
    }

    mean_value = mean_value/(int32_t)SENSORS_COUNT;


    int32_t var_value = 0;
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        int32_t dif = this->led_dif_fil_result[i] - mean_value;
        if (dif < 0)
        {
            dif = -dif;
        }

        var_value+= dif;
    }

    var_value = var_value/(int32_t)SENSORS_COUNT;


    int32_t min_value = 4095;
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {
        int32_t tmp = this->led_dif_fil_result[i];
        if (tmp < min_value)
        {
            min_value = tmp;
        }
    }   

    int32_t max_value = 0;
    for (unsigned int i = 0; i < SENSORS_COUNT; i++)
    {   
        int32_t tmp = this->led_dif_fil_result[i];
        if (tmp > max_value)
        {
           max_value = tmp;
        }
    }



    this->mean_result   = mean_value;
    this->var_result    = var_value;
    this->min_result    = min_value;
    this->max_result    = max_value;
    */
}