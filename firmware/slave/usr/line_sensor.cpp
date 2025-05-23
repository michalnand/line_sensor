#include "line_sensor.h"



#ifdef __cplusplus
extern "C" {
#endif

LineSensor *g_line_sensor;

/*
void ADC1_COMP_IRQHandler(void) 
{
    if (ADC1->ISR & ADC_ISR_EOC) 
    {
        // read conversion result
        int32_t value = ADC1->DR;

        // process data and move to next channel
        uint32_t current_channel = g_line_sensor->callback(value);

        // select next channel
        ADC1->CHSELR = (1U << current_channel);

        // clear EOC flag by writing 1
        ADC1->ISR |= ADC_ISR_EOC;

        // start next conversion
        ADC1->CR |= ADC_CR_ADSTART;
    }
}
*/

void ADC1_IRQHandler(void) 
{
    if (LL_ADC_IsActiveFlag_EOC(ADC1))
    {
        uint32_t value = LL_ADC_REG_ReadConversionData12(ADC1);
        LL_ADC_ClearFlag_EOC(ADC1);

        uint32_t next_channel = g_line_sensor->callback(value);

        // Change channel for next conversion
        ADC1->CHSELR = 1 << next_channel;

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
    
    return this->channel;
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
}

void LineSensor::_gpio_init(void) 
{
    // Enable GPIOA and GPIOB clocks for ADC channels
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    // Configure PA0 - PA7 as analog (ADC_IN0 to ADC_IN7)
    for (int pin = LL_GPIO_PIN_0; pin <= LL_GPIO_PIN_7; pin <<= 1) 
    {
        LL_GPIO_SetPinMode(GPIOA, pin, LL_GPIO_MODE_ANALOG);
        LL_GPIO_SetPinPull(GPIOA, pin, LL_GPIO_PULL_NO);
    }

    // Configure PB0, PB1 as analog (ADC_IN8, ADC_IN9)
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);

    // Configure PA0 as analog mode (adjust if using different channel)
    //LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    //LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

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


    LL_ADC_StartCalibration(ADC1); 

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
        __asm("nop");
    }


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
    
    // XOR taps: bits 31, 21, 1, 0 (0-indexed)
    uint8_t lfsr_bit = ((lfsr >> 31) ^ (lfsr >> 21) ^ (lfsr >> 1) ^ (lfsr >> 0)) & 1;
    lfsr = (lfsr << 1) | lfsr_bit;
    return lfsr_bit;
    
    
    /*
    // 4bit LFSR
    uint8_t bit = ((lfsr >> 3) ^ (lfsr >> 2)) & 1; // taps: 4 and 3
    lfsr = ((lfsr << 1) | bit) & 0x0F; // Keep only lower 4 bits
    */

    // 5bit LFSR
    //uint8_t bit = ((lfsr >> 4) ^ (lfsr >> 2)) & 1; // taps: 5 and 3
    //lfsr = ((lfsr << 1) | bit) & 0x1F; // Keep only lower 5 bits
    
    return lfsr&1;
}



void LineSensor::_processing()
{
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

    // clip min max range
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
}