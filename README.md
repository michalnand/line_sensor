# i2c sensor for line follower

- 10x analog readed sensor
- stm32f051 48MHz arm cortex m0 mcu
- i2c interface, with FFC cable
- uart, swdio availible for debug


# photos

TODO


# arduino example

```c++
#include <Wire.h>



// slave address
#define LS_I2C_ADDR       ((uint8_t)0x42)

// main registers
#define LS_WHOAMI_REG     ((uint8_t)0x00)  //ID reg, readed as WHO_AM_I_VALUE (0x6A)
#define LS_CONFIG0_REG    ((uint8_t)0x01)  //config 0, led drive modes
#define LS_CONFIG1_REG    ((uint8_t)0x02)  //config 1, not used
#define LS_FILTER_REG     ((uint8_t)0x03)  //filter coeff, 0..255

//data registers
#define LS_RAW_OFF_REG    ((uint8_t)0x04)  //data starting, base
#define LS_RAW_ON_REG     ((uint8_t)0x18)  //base + 1*20

#define LS_FIL_OFF_REG    ((uint8_t)0x2c)  //base + 2*20
#define LS_FIL_ON_REG     ((uint8_t)0x40)  //base + 3*20

#define LS_DIF_RAW_REG    ((uint8_t)0x54)  //base + 4*20
#define LS_DIF_FIL_REG    ((uint8_t)0x68)  //base + 5*20


#define WHO_AM_I_VALUE    ((uint8_t)171)
#define LS_DATA_SIZE      ((uint8_t)10)



/*
   validate if device is connected and responding
   must returns value WHO_AM_I_VALUE (171 dec)
*/
uint8_t read_who_am_i()
{
  Wire.beginTransmission(LS_I2C_ADDR);
  Wire.write(LS_WHOAMI_REG);

  Wire.endTransmission(true);
  Wire.requestFrom(LS_I2C_ADDR, (uint8_t)1);

  return Wire.read();
}




/*
  fill buffer with uint16_t values from sensors,
  result_buffer : uint16_t, buffer size is LS_DATA_SIZE (10 sensors)
  adr           : one of the data registers address : LS_RAW_OFF_REG, LS_RAW_ON_REG .. LS_DIF_FIL_REG
*/
void read_data(uint16_t *result_buffer, unsigned char adr)
{
  Wire.beginTransmission(LS_I2C_ADDR);
  Wire.write(adr);
  Wire.endTransmission(false);

  // reading 2bytes per sensor, total 2*LS_DATA_SIZE
  Wire.requestFrom(LS_I2C_ADDR, (uint8_t)(2*LS_DATA_SIZE));
  for (uint8_t i = 0; i < LS_DATA_SIZE; i++)
  {
    // first read high, then lower byte
    uint16_t tmp_h = Wire.read();
    uint16_t tmp_l = Wire.read();

    // combine into single uint16_t value
    result_buffer[i] = (tmp_h << 8)|tmp_l;
  }
}

// debug data print
void print_data(uint16_t *result_buffer)
{
  for (uint8_t i = 0; i < LS_DATA_SIZE; i++)
  {
    uint16_t tmp = result_buffer[i];
    Serial.print(tmp);
    Serial.print(" ");
  }

  Serial.println();
}

uint16_t sensor_reading[LS_DATA_SIZE];

void loop() 
{
    // check connection, reading who am I register
    uint8_t result = read_who_am_i();
    Serial.print("who am i reg : ");
    Serial.println((int)result);


    // read all data regs and print
    Serial.println("readed data");

    read_data(sensor_reading, LS_RAW_OFF_REG);
    print_data(sensor_reading);

    read_data(sensor_reading, LS_RAW_ON_REG);
    print_data(sensor_reading);

    read_data(sensor_reading, LS_FIL_OFF_REG);
    print_data(sensor_reading);

    read_data(sensor_reading, LS_FIL_ON_REG);
    print_data(sensor_reading);

    read_data(sensor_reading, LS_DIF_RAW_REG);
    print_data(sensor_reading);

    read_data(sensor_reading, LS_DIF_FIL_REG);
    print_data(sensor_reading);

    Serial.println("\n\n");

    delay(500);   
}
```


# hardware


![image](doc/images/top_3d.png)
![image](doc/images/dims.png)

![image](doc/images/top.png)
![image](doc/images/bottom.png)
![image](doc/images/board.png)
![image](doc/images/schem.png)


# usage

## compiling firmware

- arm-none-eabi-gcc is required

```bash
cd firmware
make clean
make
```

## writing into flash

- use st-link for write firmware
- connect gnd, swclk, swdio and reset to debug connector
- execture **st-flash --connect-under-reset write build.bin 0x8000000**

```bash
cd firmware/bin
./arm_burn
```