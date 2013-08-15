#ifndef __ARDUINO_H__
#define __ARDUINO_H__

#include "Helper.h"

#include "ofThread.h"
#include "ofSerial.h"
#include "ofLog.h"

class Arduino : public ofThread  {

public:
   Arduino(string serialPort, int sensorMinimum, int sensorMaximum);
   void threadedFunction();

   ofSerial serial;
    
    inline int getSensorMinimum() { return sensorMinimum; }
    inline void setSensorMinimum(int sensorMin) { sensorMinimum = sensorMin; }
    
    inline int getSensorMaximum() { return sensorMaximum; }
    inline void setSensorMaximum(int sensorMax) { sensorMaximum = sensorMax; }

    inline int getValue() { return iValue; }
    
private:
   string serialPort;
    int iValue;
    int iErrorTresh;
    int sensorMinimum;
    int sensorMaximum;
   static const int COMMAND_LENGTH = 4;

    
};

#endif // endof __ARDUINO_H__

