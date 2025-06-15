  #include <Wire.h>


  // line sensor registers

  // slave address
  #define LS_I2C_ADDR       ((uint8_t)0x42)

  // main registers
  #define LS_WHOAMI_REG     ((uint8_t)0x00)  //ID reg, readed as WHO_AM_I_VALUE (0x6A)
  #define LS_CONFIG0_REG    ((uint8_t)0x01)  //config 0, led drive modes
  #define LS_CONFIG1_REG    ((uint8_t)0x02)  //config 1, not used
  #define LS_FILTER_REG     ((uint8_t)0x03)  //filter coeff, 0..255

  //data registers
  #define LS_RAW_OFF_REG    ((uint8_t)0x04)  //data starting, base
  #define LS_RAW_ON_REG     ((uint8_t)0x20)  //base + 1*28

  #define LS_FIL_OFF_REG    ((uint8_t)0x3c)  //base + 2*28
  #define LS_FIL_ON_REG     ((uint8_t)0x58)  //base + 3*28

  #define LS_DIF_RAW_REG    ((uint8_t)0x74)  //base + 4*28
  #define LS_DIF_FIL_REG    ((uint8_t)0x90)  //base + 5*28

  //statistics    
  #define LS_STATS_0_REG    ((uint8_t)0xac)  //mean value, 16bit
  #define LS_STATS_1_REG    ((uint8_t)0xae)  //var value, 16bit
  #define LS_STATS_2_REG    ((uint8_t)0xb0)  //min value, 16bit
  #define LS_STATS_3_REG    ((uint8_t)0xb2)  //max value, 16bit


  #define WHO_AM_I_VALUE    ((uint8_t)171)
  #define LS_DATA_SIZE      ((uint8_t)14)






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

  void setup() 
  {
    Serial.begin(9600);
    Serial.print("initialising device\n");
  
    pinMode(LED_BUILTIN, OUTPUT);

    //TODO implement mode changing in slave
    /* 
    // set led drive mode to alternate (mode = 2)
    Wire.beginTransmission(LS_I2C_ADDR);
    Wire.write(LS_CONFIG0_REG);
    Wire.write(2);
    Wire.endTransmission();

    // set low pass filter coeff to 7
    Wire.beginTransmission(LS_I2C_ADDR);
    Wire.write(LS_CONFIG0_REG);
    Wire.write(7);
    Wire.endTransmission();
    */
  }


  // output buffer
  uint16_t sensor_reading[LS_DATA_SIZE];

  void loop() 
  {

  // check connection, reading who am I register
    uint8_t result = read_who_am_i();
    Serial.print("who am i reg : ");
    Serial.println((uint16_t)result);


    // perform 20 measurements, and estimate sensor reading speed
    unsigned long time_start = millis();
    for (unsigned int i = 0; i < 20; i++)
    {
      read_data(sensor_reading, LS_DIF_FIL_REG);
    }
    unsigned long time_stop = millis();

    unsigned long rps = ((uint32_t)20*(uint32_t)1000)/(time_stop - time_start);

    Serial.print("readings per second ");
    Serial.println(rps);
    
    

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

    Serial.println("\n\n\n\n");



    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);                     
    digitalWrite(LED_BUILTIN, LOW);
    delay(1500);   
  }

