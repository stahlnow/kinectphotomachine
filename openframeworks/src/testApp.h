#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxKinect.h"
#include "ofxDelaunay.h"
#include "ofxPostProcessing.h"
#include "ofxFatLine.h"
#include "Arduino.h"
#include "imgLoader.h"
#include "ofxShader.h"
#include "ofxGradientShape.h"

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

   enum Mode {
      MODE_KINECT = 0,
      MODE_SCREENSAVER
   };

   Mode mode;



   // Screensaver
   ofImage * loadImage(string filePath);
   //void crossFadeTransition(ofImage* currImg,ofImage* nextImg);
   void fadeInfadeOut(ofImage* currImg,ofImage* nextImg);
   void shaderTransition(ofImage* currImg,ofImage* nextImg);
   //ofVideoPlayer * loadVideo(string filePath);
   //ofTexture* buildSlideTexture(ofImage* im);
   //ofVideoPlayer  textureMovie;
   //ParamsLoader loaderConfig;
   ofImage * currImage;
   ofImage * nextImg;
   ofTexture temp_cur_texture;
   ofxShader transitionShader;
   ofTexture temp_next_texture;
   imgLoader   loader;
   bool        loadNextImg;
   bool transitionEnded;
   bool transitionStarted;
   float		step;
   float		progress;




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

   // settings
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

   string image_dir;
   string shader_file;
   int transition_steps;

   // meshes
   ofMesh convertedMesh;
   ofMesh wireframeMesh;
   ofxDelaunay del;
   ofImage blob;

   ofFloatColor pantone165c_f = ofFloatColor(1.0, 0.37, 0.0, 1.0);
   ofFloatColor pantone163c_f = ofFloatColor(1.0, 0.61, 0.44, 1.0);
   ofFloatColor pantone712c_f = ofFloatColor(0.98, 0.80, 0.68, 1.0);

   ofColor pantone165c = ofColor(255, 95, 0);
   ofColor pantone163c = ofColor(255, 156, 113);
   ofColor pantone712c = ofColor(250, 205, 174);
   


   // serial
   Arduino arduino;

   // screensaver
   int getCenteredCoordinate(ofImage* image);

   // button loading animation
   void addGradientShape(ofVec3f centre, int BHGShapeType, int BHGNumSides, float BHGBlur, float BHGThickness, float BHGDiameter, ofColor color, int BHGDegree);
   vector <ofxGradientShape> shapes;
   int shapeCount;
   bool bButtonTimerReached;
   int iButtonStartTime;
   int iButtonEndTime;

};
