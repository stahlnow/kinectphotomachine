/* Capacitive Touch Sensor Test */
#include <CapacitiveSensor.h>

CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);        // 1M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

void setup()                    
{
   Serial.begin(19200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
   }
}

void loop()                    
{
  // read capacitive sensor
  long s = cs_4_2.capacitiveSensor(30);
  
  unsigned char b[64];
  b[0]  = 'S';
  b[1]  = 'T';
  b[2]  = 'A';
  b[3]  = 'R';
  b[4]  = 'T';
  b[5]  =  s & 0xff;
  b[6]  = (s & 0xff00) >> 8;
  b[7]  = (s & 0xff0000) >> 16;
  b[8]  = (s & 0xff000000) >> 24;
  b[9]  = 'S';
  b[10] = 'T';
  b[11] = 'O';
  b[12] = 'P';
  Serial.write(b, 64);
  Serial.flush();
  //delay(10); 
}
