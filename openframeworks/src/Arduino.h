#ifndef __ARDUINO_H__
#define __ARDUINO_H__

#include "ofThread.h"
#include "ofSerial.h"
#include "ofLog.h"

class Arduino : public ofThread  {

public:
   Arduino(string serialPort);
   void threadedFunction();

   ofSerial serial;

private:
   string serialPort;
   static const int COMMAND_LENGTH = 64;

};

#endif // endof __ARDUINO_H__

