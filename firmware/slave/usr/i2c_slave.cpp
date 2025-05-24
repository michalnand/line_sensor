#include "i2c_slave.h"

#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_i2c.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_cortex.h"





#ifdef __cplusplus
extern "C" {
#endif

I2CSlave *g_i2c_ptr;


void I2C1_IRQHandler()
{
    uint32_t isr = I2C1->ISR;

    // Address matched
    if (isr & I2C_ISR_ADDR) 
    {
        I2C1->ICR |= I2C_ICR_ADDRCF;    
        
        // master reading from slave
        if (I2C1->ISR & I2C_ISR_DIR)  
        {
            // preload register for sending to master
            // this is must done here
            I2C1->TXDR = g_i2c_ptr->buffer[g_i2c_ptr->ptr];
            g_i2c_ptr->ptr++;

            g_i2c_ptr->state = 2;
        }
        else
        {
            // master is writing to slave — prepare to receive register address
            g_i2c_ptr->state = 1;
        }

        // toogle debug led
        GPIOC->ODR ^= (1 << 6);
    }

    

    // Data received from master (master write)
    if (isr & I2C_ISR_RXNE) 
    {
        uint8_t byte = I2C1->RXDR;
        
        // first received data byte is register address
        // store it and move to next state
        if (g_i2c_ptr->state == 1)
        {
            g_i2c_ptr->ptr   = byte;
            g_i2c_ptr->state = 2;   
        }
        // other received data are written into incrementing address pointer
        else
        {
            g_i2c_ptr->buffer[g_i2c_ptr->ptr] = byte;
            g_i2c_ptr->ptr++;
        }
    }

    // sending data to master
    if (isr & I2C_ISR_TXIS)
    {
        I2C1->TXDR = g_i2c_ptr->buffer[g_i2c_ptr->ptr];
        g_i2c_ptr->ptr++;
    }
      
    // stop received, back to initial state
    if (isr & I2C_ISR_STOPF) 
    {
        I2C1->ICR |= I2C_ICR_STOPCF;
        
        g_i2c_ptr->ptr   = 0;
        g_i2c_ptr->state = 0;   
    }

    // nack handling
    if (isr & I2C_ISR_NACKF) 
    {
        I2C1->ICR |= I2C_ICR_NACKCF;
    }
}



/*
void I2C1_IRQHandler()
{
    //uint32_t isr = I2C1->ISR;


    if(LL_I2C_IsActiveFlag_ADDR(I2C1)) 
    {
        if(LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_WRITE) 
        {
            // Master is writing register index
            LL_I2C_ClearFlag_ADDR(I2C1);
            // Prepare to receive 1 byte (the register index)

            g_i2c_ptr->state = 1;
        } 
        else 
        {
            // Master is reading data
            LL_I2C_ClearFlag_ADDR(I2C1);
            // Preload first byte
            LL_I2C_TransmitData8(I2C1, g_i2c_ptr->buffer[g_i2c_ptr->ptr]);
        }
    }

    if(LL_I2C_IsActiveFlag_RXNE(I2C1)) 
    {
        // Received one byte (register index)
        if (g_i2c_ptr->state == 1)
        {
            g_i2c_ptr->ptr = LL_I2C_ReceiveData8(I2C1);
            g_i2c_ptr->state = 2;
        }
    }

    if(LL_I2C_IsActiveFlag_TXIS(I2C1)) 
    {
        // Master asked for next byte (after first)
        g_i2c_ptr->ptr++;
        LL_I2C_TransmitData8(I2C1, g_i2c_ptr->buffer[g_i2c_ptr->ptr]);
    }

    if(LL_I2C_IsActiveFlag_STOP(I2C1)) 
    {
        LL_I2C_ClearFlag_STOP(I2C1);
        // Reset state if necessary

        g_i2c_ptr->state = 0;
        g_i2c_ptr->ptr   = 0;
    }

}
*/

#ifdef __cplusplus
}
#endif



void I2CSlave::init()
{
    g_i2c_ptr       = this;
    this->ptr       = 0;
    this->state     = 0;

    for (unsigned int i = 0; i < I2C_BUFFER_SIZE; i++)
    {
        buffer[i] = 100 + i; //debug infill  
    }
    
    this->_i2c_init();

    Gpio<TGPIOC, 6, GPIO_MODE_OUT> led;
    led = 0;
}





void I2CSlave::_i2c_init() 
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
    
    /*
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    // Configure PA9 (SCL) and PA10 (SDA) as alternate function, open-drain, pull-up
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_6);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_10, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_6);
    */


    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    // Configure PB8 (SCL) and PB7 (SDA) as alternate function, open-drain, pull-up
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);
    LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_6);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_UP);
    LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_8, LL_GPIO_AF_6);

    // Reset and configure I2C
    //LL_I2C_DeInit(I2C1);
    LL_I2C_Disable(I2C1);

    LL_I2C_SetOwnAddress1(I2C1, I2C_SLAVE_ADDRESS << 1, LL_I2C_OWNADDRESS1_7BIT);
    LL_I2C_EnableOwnAddress1(I2C1);
    LL_I2C_SetMode(I2C1, LL_I2C_MODE_I2C);

    // Set timing for 400kHz (fast-mode), assuming 64 MHz PCLK
    // Use timing calculator or ST reference for exact value
    // Below timing is approximate for 400kHz at 64MHz PCLK1
    //I2C1->TIMINGR = 0x00300F33;
    I2C1->TIMINGR = 0x30420F13;


    // Enable interrupts
    LL_I2C_EnableIT_ADDR(I2C1);
    LL_I2C_EnableIT_RX(I2C1);
    LL_I2C_EnableIT_TX(I2C1);
    LL_I2C_EnableIT_STOP(I2C1);
    LL_I2C_EnableIT_NACK(I2C1);

    LL_I2C_Enable(I2C1);
    NVIC_SetPriority(I2C1_IRQn, 0);
    NVIC_EnableIRQ(I2C1_IRQn);
}