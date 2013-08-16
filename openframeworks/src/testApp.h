#pragma once

#include <time.h>

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxKinect.h"
#include "ofxDelaunay.h"
#include "ofxPostProcessing.h"
#include "ofxFatLine.h"

#include "Helper.h"
#include "Arduino.h"
#include "ProgressWheel.h"
#include "Slideshow.h"
#include "Countdown.h"

class testApp : public ofBaseApp {

public:
   void setup();
   void update();
   void draw();

   void keyPressed(int key);
   void keyReleased(int key);
   void mouseMoved(int x, int y);
   void mouseDragged(int x, int y, int button);
   void mousePressed(int x, int y, int button);
   void mouseReleased(int x, int y, int button);
   void windowResized(int w, int h);
   void dragEvent(ofDragInfo dragInfo);
   void gotMessage(ofMessage msg);

   void exit();

   enum Mode {
      MODE_KINECT = 0,
      MODE_SLIDESHOW
   };

   Mode mode;


private:
    
   void printPhoto();
    
   // Kinect
   ofxKinect kinect;
   ofEasyCam cam;
   ofxPostProcessing postFx;
   int iKinectServoAngle;
   ofMesh convertedMesh;
   ofMesh wireframeMesh;
   ofxDelaunay del;
   ofImage blob;

   ofTrueTypeFont font;
   ofTrueTypeFont fontText;
    
   bool startPhotoCountdown = false;
   Countdown countdown;
   bool freeze = false;

   // Gui
   bool showGui;
   ofxPanel gui;
   ofxSlider<int> colorAlpha;
   ofxSlider<float> noiseAmount;
   ofxToggle useRealColors;
   ofxToggle drawWireMesh;
   ofxToggle hasSlideshow; 
   ofxSlider<int> pointSkip;
   ofxSlider<int> screenRotation;
   ofxSlider<int> kinectZTranslation;
   ofxSlider<int> slideDuration;
   ofxSlider<int> kinectRed;
   ofxSlider<int> kinectGreen;
   ofxSlider<int> kinectBlue;
    ofxSlider<int> sensorMinimum;
    ofxSlider<int> sensorMaximum;

   // Settings
   string log_level;
   string serial_port;
   string mobile_printer_name;
   string mobile_printer_format;
   string mobile_printer_quality;
   string mobile_printer_media_type;
   string printer_name;
   string printer_format;
   string printer_quality;
   string printer_media_type;

   string slides_dir;
   string shader_file;
   int transition_steps;
    
    string photo_dir;
  
   // Arduino
   Arduino* arduino;

   // Slideshow
   Slideshow* slideshow;
   
   // Loading animation
    bool showWheel = false;
   ProgressWheel progressWheel;

};
