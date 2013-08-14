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
  
  unsigned char b[4];
  b[0]  =  s & 0xff;
  b[1]  = (s & 0xff00) >> 8;
  b[2]  = (s & 0xff0000) >> 16;
  b[3]  = (s & 0xff000000) >> 24;
  
  Serial.write(b, 4);
  Serial.flush();
  //delay(10); 
}
