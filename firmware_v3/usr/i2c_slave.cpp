#include "i2c_slave.h"


#ifdef __cplusplus
extern "C" {
#endif

I2CSlave *g_i2c_ptr;





void I2C1_IRQHandler()
{
    // Address matched    
    if (LL_I2C_IsActiveFlag_ADDR(I2C1))
    {
        // Verify the Address Match with the OWN Slave address
        if (LL_I2C_GetAddressMatchCode(I2C1) == (I2C_SLAVE_ADDRESS << 1))
        {
            if (LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_READ)
            {
                LL_I2C_EnableIT_TX(I2C1);   

                // transmitting state
                g_i2c_ptr->state = 3;
            }
            else
            // receive address state
            {
                g_i2c_ptr->state = 1;
            }   
        }
       
        LL_I2C_ClearFlag_ADDR(I2C1);

        // toogle debug led
        //GPIOA->ODR ^= (1 << 8);
    }       

    // receiving from master
    if (LL_I2C_IsActiveFlag_RXNE(I2C1)) 
    {
        uint8_t byte = I2C1->RXDR;

        // first receive byte is register address
        if (g_i2c_ptr->state == 1)
        {
            g_i2c_ptr->ptr   = byte;
            g_i2c_ptr->state = 2;
        }

        // store rest received bytes to current address
        else if (g_i2c_ptr->state == 2) 
        {
            g_i2c_ptr->buffer[g_i2c_ptr->ptr] = byte;
            g_i2c_ptr->ptr++;
        }

        g_i2c_ptr->wr_flag = 1;
    }

    // sending data to master
    if (LL_I2C_IsActiveFlag_TXIS(I2C1)) 
    {
        LL_I2C_TransmitData8(I2C1, g_i2c_ptr->buffer[g_i2c_ptr->ptr]);
        //LL_I2C_ClearFlag_TXE(I2C1);
        //LL_I2C_EnableIT_TX(I2C1);
    
        g_i2c_ptr->ptr++;
    }


    // stop received, back to initial state
    if (LL_I2C_IsActiveFlag_STOP(I2C1)) 
    {
        LL_I2C_ClearFlag_STOP(I2C1);

        LL_I2C_DisableIT_TX(I2C1);

        if (!LL_I2C_IsActiveFlag_TXE(I2C1))
        {
            LL_I2C_ClearFlag_TXE(I2C1);
        }

        // reset to initial state        
        g_i2c_ptr->ptr   = 0;
        g_i2c_ptr->state = 0;   
    }

    // nack handling
    if (LL_I2C_IsActiveFlag_NACK(I2C1)) 
    {
        LL_I2C_ClearFlag_NACK(I2C1);
    }
}



#ifdef __cplusplus
}
#endif



void I2CSlave::init()
{
    g_i2c_ptr       = this;

    this->wr_flag   = 0;
    this->ptr       = 0;
    this->state     = 0;

    for (unsigned int i = 0; i < I2C_BUFFER_SIZE; i++)
    {
        buffer[i] = 0;
    }
    
    this->_i2c_init();
}


uint8_t I2CSlave::is_write_flag()
{
    uint8_t result = this->wr_flag;

    this->wr_flag = 0;

    return result;
}




void I2CSlave::_i2c_init() 
{    
    /*
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
    */


    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};


    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    
    LL_I2C_InitTypeDef I2C_InitStruct = {0};


    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  
    NVIC_SetPriority(I2C1_IRQn, 0);
    NVIC_EnableIRQ(I2C1_IRQn);

    LL_I2C_DeInit(I2C1);

    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.Timing = 0x00303D5B;
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0x0;
    I2C_InitStruct.OwnAddress1 = (I2C_SLAVE_ADDRESS << 1);
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &I2C_InitStruct);
    LL_I2C_EnableAutoEndMode(I2C1);
    LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
    LL_I2C_DisableOwnAddress2(I2C1);
    LL_I2C_DisableGeneralCall(I2C1);
    //LL_I2C_EnableClockStretching(I2C1);
    
    LL_I2C_Init(I2C1, &I2C_InitStruct);
   

    // i2c interrupts
    LL_I2C_EnableIT_ADDR(I2C1);

    LL_I2C_EnableIT_RX(I2C1);
    //LL_I2C_EnableIT_TX(I2C1);

    LL_I2C_EnableIT_STOP(I2C1);
    LL_I2C_EnableIT_NACK(I2C1);
    
}