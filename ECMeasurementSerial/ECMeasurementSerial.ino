/*
  Example code for the EC Reader v1.0
  
  http://www.cyberplant.info
  by CyberPlant LLC, 03 December 2015
  This example code is in the public domain.

*/
 
#include <EEPROM.h>
#include <OneWire.h>
#include <DS18B20.h>

#define X0 0.0
#define X1 2.0
#define X2 12.88
#define X3 80.0

#define alphaLTC 0.022 // The linear temperature coefficient

int addressCheckMemory = 0;
int addressY0 = addressCheckMemory+sizeof(float);
int addressY1 = addressY0+sizeof(float);
int addressY2 = addressY1+sizeof(float);
int addressY3 = addressY2+sizeof(float);

const float CheckMemory = 3.14159;

long Y0 = EEPROM_float_read(addressY0);
long Y1 = EEPROM_float_read(addressY1);
long Y2 = EEPROM_float_read(addressY2);
long Y3 = EEPROM_float_read(addressY3);

float A;
float B;
float C;

long pulseCount = 0;  //a pulse counter variable
long pulseCal;
byte ECcal = 0;

unsigned long pulseTime,lastTime, duration, totalDuration;


unsigned int Interval=1000;
long previousMillis = 0;
unsigned long Time;

float EC;
float temp;
float tempManual = 25.0;

int sequence = 0;

const byte ONEWIRE_PIN = 3; // temperature sensor ds18b20, pin D3
byte address[8];
OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);

int incomingByte = 0;  
 
int cal_Y0 = 0;
int cal_Y1 = 0;
int cal_Y2 = 0;
int cal_Y3 = 0;

void setup()
{
  Serial.begin(9600);
  Time=millis();
  pinMode(2, INPUT_PULLUP); //  An internal 20K-ohm resistor is pulled to 5V. If you use hardware pull-up delete this
  sensors.begin();
  Search_sensors();

  Serial.println("Calibrate commands:");
  Serial.println("E.C. :");
  Serial.println("      Cal. 0,00 uS ---- 0");
  Serial.println("      Cal. 2,00 uS ---- 1");
  Serial.println("      Cal. 12,88 uS --- 2");
  Serial.println("      Cal. 80,00 uS --- 3");
  Serial.println("      Reset E.C. ------ 5");
  Serial.println("  ");
  delay(250);

}


void loop()
{
  if (millis()-Time>=Interval)
{  
  Time = millis();
  
  sequence ++;
  if (sequence > 1)
  sequence = 0;


   if (sequence==0)
  {
    pulseCount=0; //reset the pulse counter
    attachInterrupt(0,onPulse,RISING); //attach an interrupt counter to interrupt pin 1 (digital pin #3) -- the only other possible pin on the 328p is interrupt pin #0 (digital pin #2)
  }
  
   if (sequence==1)
  {
    detachInterrupt(0);
    pulseCal = pulseCount;
    temp_read();
    if (temp > 200 || temp < -20 )
  { 
    temp = tempManual;
    Serial.println("temp sensor connection error!");
    Search_sensors();
  }


  ECread();
  
  // Prints measurements on Serial Monitor
  Serial.println("  ");
  Serial.print("t ");
  Serial.print(temp);
  Serial.print(F(" *C"));
  Serial.print("    E.C. ");
  Serial.print(EC); // uS/cm
  Serial.print("    pulses/sec = ");
  Serial.print(pulseCal);
  Serial.print("    C = ");
  Serial.println(C); // Conductivity without temperature compensation
  }
}

    if (Serial.available() > 0) //  function of calibration E.C.
    {  
        incomingByte = Serial.read();
        cal_sensors();
    }

}

/*-----------------------------------End loop---------------------------------------*/
float temp_read() // calculate pH
{
      sensors.request(address);
  
  // Waiting (block the program) for measurement reesults
  while(!sensors.available());
  
    temp = sensors.readTemperature(address);
    return temp;
}


float ECread()  //graph function of read EC
{
     if (pulseCal>Y0 && pulseCal<Y1 )
      {
        A = (Y1 - Y0) / (X1 - X0);
        B = Y0 - (A * X0);
        C = (pulseCal - B) / A;
      }
      
      if (pulseCal > Y1 && pulseCal<Y2 )
      {
        A = (Y2-Y1) / (X2 - X1);
        B = Y1 - (A * X1);
        C = (pulseCal - B) / A;
      }
      if (pulseCal > Y2)
      {
        A = (Y3-Y2) / (X3 - X2);
        B = Y2 - (A * X2);
        C = (pulseCal - B) / A;
      }
      
    EC = (C / (1 + alphaLTC * (temp-25.00)));
    return EC;
}

void EEPROM_float_write(int addr, float val) // write to EEPROM
{  
  byte *x = (byte *)&val;
  for(byte i = 0; i < 4; i++) EEPROM.write(i+addr, x[i]);
}

float EEPROM_float_read(int addr) // read of EEPROM
{    
  byte x[4];
  for(byte i = 0; i < 4; i++) x[i] = EEPROM.read(i+addr);
  float *y = (float *)&x;
  return y[0];
}

void onPulse() // EC pulse counter
{
  pulseCount++;
}

void Search_sensors() // search ds18b20 temperature sensor
{
  address[8];
  
  onewire.reset_search();
  while(onewire.search(address))
  {
    if (address[0] != 0x28)
      continue;
      
    if (OneWire::crc8(address, 7) != address[7])
    {
      Serial.println(F("temp sensor connection error!"));
      temp = 25.0;
      break;
    }
   /*
    for (byte i=0; i<8; i++)
    {
      Serial.print(F("0x"));
      Serial.print(address[i], HEX);
      
      if (i < 7)
        Serial.print(F(", "));
    }
    
    */
  }

}

void cal_sensors()
{
  Serial.println(" ");
  
 if (incomingByte == 53) // press key "5"
 {
  Reset_EC();
 }
  else if (incomingByte == 48) // press key "0"
 {
  ECcal = 1;
  Serial.print("Cal. 0,00 uS ...");  
  Y0 = pulseCount / (1 + alphaLTC * (temp-25.00));
  EC = ECread();
  while (EC > 0.01)
    {
    Y0++;
    EC = ECread();
    }
  EEPROM_float_write(addressY0, Y0);
  Serial.println(" complete");
  ECcal = 0;
 }
 
 else if (incomingByte == 49) // press key "1"
 {
  ECcal = 1;
  Serial.print("Cal. 2,00 uS ...");  
  Y1 = pulseCount / (1 + alphaLTC * (temp-25.00));
  EC = ECread();
  while (EC > X1)
    {
    Y1++;
    EC = ECread();
    }
  EEPROM_float_write(addressY1, Y1);
  Serial.println(" complete");
  ECcal = 0;
 }
 
 else if (incomingByte == 50) // press key "2"
 {
  ECcal = 1;
  Serial.print("Cal. 12,88 uS ...");  
  Y2 = pulseCount / (1 + alphaLTC * (temp-25.00));
  EC = ECread();
  while (EC > X2)
    {      
    Y2++;
    EC = ECread();
    }
  EEPROM_float_write(addressY2, Y2);
  Serial.println(" complete");
  ECcal = 0;
 }
 
  else if (incomingByte == 51) // press key "3"
 {
  ECcal = 1;
  Serial.print("Cal. 80,00 uS ..."); 
  Y3 = pulseCount / (1 + alphaLTC * (temp-25.00));
  EC = ECread(); 
  while (EC > X3)
    { 
    Y3++;
    EC = ECread();
    }
  EEPROM_float_write(addressY3, Y3);
  Serial.println(" complete");
  ECcal = 0;
 }

}



void Reset_EC()
{
  Serial.print("Reset EC ...");
  EEPROM_float_write(addressY0, 240);
  EEPROM_float_write(addressY1, 1245);
  EEPROM_float_write(addressY2, 5282);
  EEPROM_float_write(addressY3, 17255);
  Y0 = EEPROM_float_read(addressY0);
  Y1 = EEPROM_float_read(addressY1);
  Y2 = EEPROM_float_read(addressY2);
  Y3 = EEPROM_float_read(addressY3);
  Serial.println(" complete");
}
