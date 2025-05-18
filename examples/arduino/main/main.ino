#include <Wire.h>


#define LS_I2C_ADDR       0x42

#define LS_WHOAMI_REG     0x00  //ID reg, readed as 0x6A
#define LS_CONFIG0_REG    0x01  //config 0, led drive modes
#define LS_CONFIG1_REG    0x02  //config 1, not used
#define LS_FILTER_REG     0x03  //filter coeff, 0..255

#define LS_RAW_OFF_REG    0x04  //data starting, base
#define LS_RAW_ON_REG     0x18  //base + 1*20
#define LS_FIL_OFF_REG    0x2c  //base + 2*20
#define LS_FIL_ON_REG     0x40  //base + 3*20
#define LS_DIF_RAW_REG    0x54  //base + 4*20
#define LS_DIF_FIL_REG    0x68  //base + 5*20

// validate if device is connected and responding
uint16_t read_who_am_i()
{
  Wire.beginTransmission(LS_I2C_ADDR);
  Wire.write(LS_WHOAMI_REG);

  Wire.endTransmission(false);
  Wire.requestFrom(LS_I2C_ADDR, 2);

  uint16_t res_h = Wire.read();
  uint16_t res_l = Wire.read();

  return (res_h<<8)|res_l;
}

  /*
  if (Wire.available()) 
  {
    uint8_t result = Wire.read();
    return result;
  }

  return 0xff;
}
*/


#define NUM_DATA_REGS 20

void read_data(unsigned char adr)
{
  unsigned char data[NUM_DATA_REGS];

  Wire.beginTransmission(LS_I2C_ADDR);
  Wire.write(adr);
  Wire.endTransmission(false);

  Wire.requestFrom(LS_I2C_ADDR, NUM_DATA_REGS);
  for (uint8_t i = 0; i < NUM_DATA_REGS; i++)
  {
    data[i] = Wire.read();
  }

}

void setup() 
{
  Serial.begin(9600);
  Serial.print("initialising\n");


  uint16_t result = read_who_am_i(); 

  Serial.print("who am i reg : ");
  Serial.println(result, HEX);

 

  /*
  // LED drive mode example

  // set LED mode OFF
  Wire.beginTransmission(LS_I2C_ADDR);
  Wire.write(LS_CONFIG0_REG);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(500);


  // set LED mode ON
  Wire.beginTransmission(LS_I2C_ADDR);
  Wire.write(LS_CONFIG0_REG);
  Wire.write(0x01);
  Wire.endTransmission();

  delay(500);


  // set LED mode alternate
  Wire.beginTransmission(LS_I2C_ADDR);
  Wire.write(LS_CONFIG0_REG);
  Wire.write(0x02);
  Wire.endTransmission();

  delay(500);
  */


  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() 
{
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(100);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(100);   

  //read_data(LS_LED_ON_RAW_REG);

  char result = read_who_am_i();

  Serial.print("who am i reg : ");
  Serial.println(result, HEX);

}
