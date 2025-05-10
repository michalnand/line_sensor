#include "i2c_slave.h"



void I2CSlave::init()
{
    this->_gpio_init();
    this->_slave_init();
}

void I2CSlave::pool()
{
    uint8_t reg_index = 0;

    while (1) 
    {
        // Wait for address match
        while (!I2C_GetFlagStatus(I2C1, I2C_ISR_ADDR));
        
        // Detect if write or read
        bool master_write;
        if ((I2C1->ISR & I2C_ISR_DIR) == 0)
            master_write = true;
        else
            master_write = false;

        (void)I2C1->ISR; // Clear ADDR flag
        (void)I2C1->RXDR;

        uint8_t byte_count = 0;
        
        if (master_write) 
        {
            // Write sequence (register addr, optional value)
            while (!I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF)) 
            {
                if (I2C_GetFlagStatus(I2C1, I2C_ISR_RXNE)) 
                {
                    uint8_t data = I2C_ReceiveData(I2C1);
                    if (byte_count == 0)
                    {
                        reg_index = data; // Store register index
                    } 
                    else 
                    {
                        this->_i2c_write_reg(reg_index, data);
                        reg_index++; // auto-increment for sequential writes
                    }

                    byte_count++;
                }
            }

            I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
        } 
        else 
        {
            // Read sequence
            while (!I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF)) 
            {
                if (I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS)) 
                {
                    I2C_SendData(I2C1, this->_i2c_read_reg(reg_index));
                    reg_index++; // auto-increment for sequential reads
                }
            }

            I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
        }
    }
}

void I2CSlave::_gpio_init() 
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1); // SCL
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1); // SDA
}

void I2CSlave::_slave_init() 
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    I2C_InitTypeDef I2C_InitStruct;
    I2C_DeInit(I2C1);

    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    I2C_InitStruct.I2C_DigitalFilter = 0;
    I2C_InitStruct.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS << 1;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    
    //I2C_InitStruct.I2C_Timing = 0x2000090E; // Standard mode timing (100kHz @ 48MHz)
    I2C_InitStruct.I2C_Timing = 0x00100306; // Fast mode timing (400kHz @ 48MHz)

    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
    I2C_OwnAddress2Config(I2C1, 0, I2C_OA2_NoMask); // Optional secondary address disable
}


void I2CSlave::_i2c_write_reg(uint8_t reg_adr, uint8_t value)
{
    this->regs[reg_adr%I2C_MEM_SIZE] = value;
}

uint8_t I2CSlave::_i2c_read_reg(uint8_t reg_adr)
{
    return this->regs[reg_adr%I2C_MEM_SIZE];
}