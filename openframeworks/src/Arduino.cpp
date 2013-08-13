#include "Arduino.h"

Arduino::Arduino(string serialPort)
: serialPort(serialPort) {

   if (!serial.setup(serialPort, 19200)){
      ofLog(OF_LOG_ERROR, "Could not open serial port.");
   }

}

void Arduino::threadedFunction() {

   unsigned char buffer[COMMAND_LENGTH];

   while (isThreadRunning()) {

      if (serial.available() >= COMMAND_LENGTH) {

         int result = serial.readBytes(buffer, COMMAND_LENGTH);

         // check for error code
         if ( result == OF_SERIAL_ERROR ) {
            ofLog(OF_LOG_ERROR, "Serial: Unrecoverable error reading from serial");
            break;

         } else if ( result == OF_SERIAL_NO_DATA ) {   
            ofLog(OF_LOG_NOTICE, "Serial: Nothing was read, continue..");

         } else {
            int value = buffer[5] | (buffer[6] << 8) | (buffer[7] << 16) | (buffer[8] << 24);
            ofLog(OF_LOG_VERBOSE, "Serial value: %i", value);
            serial.flush();
         }
      }
   }

   ofLog(OF_LOG_VERBOSE, "Closing serial port...");
   serial.close();
   ofLog(OF_LOG_VERBOSE, "Serial port closed.");


}


