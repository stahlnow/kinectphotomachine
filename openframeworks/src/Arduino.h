#ifndef __ARDUINO_H__
#define __ARDUINO_H__

class Arduino : public ofThread  {

public:

   static const int COMMAND_LENGTH = 13;

   void threadedFunction() {

      serial.setup("/dev/ttyACM3", 9600);

      unsigned char buffer[COMMAND_LENGTH];

      while(isThreadRunning()) {

         if (serial.available() >= COMMAND_LENGTH) {
         
            int result = serial.readBytes(buffer, COMMAND_LENGTH);

            string input(buffer, buffer + sizeof buffer / sizeof buffer[0] );

            // check for error code
            if ( result == OF_SERIAL_ERROR ) {
               cout << "unrecoverable error reading from serial" << endl;
               break;

            } else if ( result == OF_SERIAL_NO_DATA ) {   
               cout << "nothing was read, try again." << endl;
            
            } else {
               
               //cout << input << endl;
               
               int p0 = input.rfind("START", input.length());
               int p1 = input.rfind("STOP", input.length());

               if (p0 > -1 && p1 > -1) {
                  int value = buffer[5] | (buffer[6] << 8) | (buffer[7] << 16) | (buffer[8] << 24);
                  cout << "found START as " << input.substr(p0, 5) << " and STOP as " << input.substr(p1, 4) << " value is: " << value << endl;
               }
            
            }
         }
      }

      cout << "closing serial port..." << endl;
      serial.close();

   }


   ofSerial serial;

};

#endif // endof __ARDUINO_H__

