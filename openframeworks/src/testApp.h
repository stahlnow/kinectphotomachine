#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxKinect.h"
#include "ofxDelaunay.h"
#include "ofxPostProcessing.h"
#include "ofxFatLine.h"
#include "Arduino.h"

  
static bool isPrinting = false;
static bool isLoading = false;

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

  
private:
   ofxKinect kinect;

   ofEasyCam cam;

   ofxPostProcessing postFx;

   // gui
   bool showGui;
   ofxPanel gui;
   ofxSlider<int> colorAlpha;
   ofxSlider<float> noiseAmount;
   ofxToggle useRealColors;
   ofxSlider<int> pointSkip;
   ofxSlider<int> screenRotation;

   // other settings
   string serial_port;
   string mobile_printer_name;
   string mobile_printer_format;
   string mobile_printer_quality;
   string mobile_printer_media_type;

   

   // meshes
   ofMesh convertedMesh;
   ofMesh wireframeMesh;
   ofxDelaunay del;
   ofImage blob;

   ofFloatColor pantone165c = ofFloatColor(1.0, 0.37, 0.0, 1.0);
   ofFloatColor pantone163c = ofFloatColor(1.0, 0.61, 0.44, 1.0);
   ofFloatColor pantone712c = ofFloatColor(0.98, 0.80, 0.68, 1.0);


   // serial
   Arduino arduino;

};
