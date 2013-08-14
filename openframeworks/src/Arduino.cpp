#include "Arduino.h"

Arduino::Arduino(string serialPort, int sensorMinimum, int sensorMaximum)
: serialPort(serialPort), sensorMinimum(sensorMinimum), sensorMaximum(sensorMaximum) {

   vector <ofSerialDeviceInfo> dl = serial.getDeviceList();

   ofLog(OF_LOG_NOTICE, "### Listing serial ports:");

   for (int i = 0; i < dl.size(); i++) {
      string path = dl[i].getDevicePath();
      string name = dl[i].getDeviceName();
      ostringstream idString; idString << dl[i].getDeviceID();
      ofLog(OF_LOG_NOTICE, "\t" + path + ", " + name + ", " + idString.str());

      if (serialPort == "auto") { // if user defined "auto" in settings, auto assign serial devices that contain ACM (linux) or usbmodem (OSX)
         if (std::string::npos != path.find("ACM") || std::string::npos != path.find("tty.usbmodem")) {
            serialPort = path;
         }
      }

   }

   if (!serial.setup(serialPort, 19200)) {
      ofLog(OF_LOG_ERROR, "Could not open serial port \"" + serialPort + "\", exit...");
      exit(-1);
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
            int value = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
             
             //ofLog(OF_LOG_VERBOSE, "Serial value: %i", value);
             
             if (value > sensorMinimum && value < sensorMaximum) {
                 if (!Helper::isButtonActive) {
                     ofLog(OF_LOG_NOTICE, "Button is active, value is %i", value);
                     Helper::isButtonActive = true;
                 }
             }
             else {
                 if (Helper::isButtonActive) {
                     Helper::isButtonActive = false;
                     Helper::canSwitch = true;
                 }
             }
             
            serial.flush();
         }
      }
   }

   ofLog(OF_LOG_VERBOSE, "Closing serial port...");
   serial.close();
   ofLog(OF_LOG_VERBOSE, "Serial port closed.");


}


