#include "i2c_slave.h"

I2CSlave *g_i2c_slave;  


#ifdef __cplusplus
extern "C" {
#endif

#define I2C_STATE_WAITING		((uint8_t)0)    // waiting for commands
#define I2C_STATE_ADR_RX	    ((uint8_t)1)    // waiting for register address
#define I2C_STATE_DATA_RX	    ((uint8_t)2)    // waiting for incomming data
#define I2C_STATE_DATA_TX	    ((uint8_t)3)    // waiting for sending data

volatile uint8_t g_i2c_state;
volatile uint8_t g_selected_reg;
volatile uint8_t g_regs[I2C_MEM_SIZE];
volatile bool    g_writing_flag;

void I2C1_IRQHandler(void)
{
    uint32_t isr = I2C1->ISR;

    // Address matched (can happen on write or repeated-start read)
    if (isr & I2C_ISR_ADDR)
    {
        if ((isr & I2C_ISR_DIR) == 0)
        {
            // Master is writing (register address expected)
            g_i2c_state = I2C_STATE_ADR_RX;
        }
        else
        {
            // Master is reading (repeated start condition)
            g_i2c_state = I2C_STATE_DATA_TX;

            // Preload TXDR here — THIS IS CRITICAL
            uint8_t tmp = g_regs[g_selected_reg];
            I2C1->TXDR = tmp;
            I2C1->TXDR = tmp;
        }

        // Clear address flag
        I2C1->ICR = I2C_ICR_ADDRCF;
    }

    // Receive register address from master
    if (isr & I2C_ISR_RXNE)
    {
        uint8_t data = I2C1->RXDR;

        if (g_i2c_state == I2C_STATE_ADR_RX)
        {
            g_selected_reg = data;
            g_i2c_state = I2C_STATE_DATA_RX;
        }
        else
        {
            g_regs[g_selected_reg] = data;
            g_writing_flag         = true;

            //g_selected_reg++; 
        }
    }

    // master continues to read more bytes — optional autoincrement
    if (isr & I2C_ISR_TXIS)
    {
        I2C1->TXDR = g_regs[g_selected_reg];
        //g_selected_reg++; 
    }

    // End of transaction
    if (isr & I2C_ISR_STOPF)
    {
        g_i2c_state = I2C_STATE_WAITING;
        I2C1->ICR = I2C_ICR_STOPCF;
    }
}


#ifdef __cplusplus
}
#endif


void I2CSlave::init()
{
    g_i2c_slave = this;

    this->regs = (uint8_t*)g_regs;

    _data_init();
    _gpio_init();
    _slave_init();
}

void I2CSlave::_data_init()
{
    g_i2c_state      = I2C_STATE_WAITING;
    g_selected_reg   = 0;
    g_writing_flag   = 0;

    for (uint32_t i = 0; i < I2C_MEM_SIZE; i++)
    {
        g_regs[i] = 0;
    }

    /*
    g_regs[10] = 3;
    g_regs[11] = 1;
    g_regs[12] = 4;
    g_regs[13] = 1;
    g_regs[14] = 5;
    g_regs[15] = 9;
    g_regs[16] = 2;
    g_regs[17] = 6;
    g_regs[18] = 5;
    g_regs[19] = 8;
    */
    
}

void I2CSlave::_gpio_init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_OD;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_Init(GPIOB, &gpio);


    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1); // SCL
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1); // SDA
}

void I2CSlave::_slave_init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    I2C_DeInit(I2C1);

    I2C_InitTypeDef i2c;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    i2c.I2C_DigitalFilter = 0;
    i2c.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS << 1;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;

    //i2c.I2C_Timing = 0x2000090E; // Standard mode (100kHz @ 48MHz)
    i2c.I2C_Timing = 0x00B01A4B; // 400Hz mode @ 48MHz

    I2C_Init(I2C1, &i2c);
    I2C_Cmd(I2C1, ENABLE);

    NVIC_SetPriority(I2C1_IRQn, 0);
    NVIC_EnableIRQ(I2C1_IRQn);

    I2C_ITConfig(I2C1, I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI | I2C_IT_STOPI, ENABLE);
}

bool I2CSlave::is_write_flag()
{
    bool result = false;

    if (g_writing_flag)
    {
        result = true;
        g_writing_flag = false;
    }

    return result;
}

void I2CSlave::write_reg(uint8_t reg_adr, uint8_t value)
{
    g_regs[reg_adr % I2C_MEM_SIZE] = value;
}

uint8_t I2CSlave::read_reg(uint8_t reg_adr)
{
    return g_regs[reg_adr % I2C_MEM_SIZE];
}