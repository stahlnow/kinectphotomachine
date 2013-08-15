#include "testApp.h"
#include "ofAppGlutWindow.h"

//--------------------------------------------------------------
int main() {
   ofAppGlutWindow window; // create a window
   
   // set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
   // Test window
   ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
   
    //ofSetupOpenGL(&window, 1920, 1080, OF_FULLSCREEN);
    
   // 42" screen installation setting
   //ofSetupOpenGL(&window, 1080, 1920, OF_FULLSCREEN);
   
   ofRunApp(new testApp()); // start the app
}
