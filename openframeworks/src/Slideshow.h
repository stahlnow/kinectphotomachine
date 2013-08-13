#ifndef __SLIDE_SHOW_H__
#define __SLIDE_SHOW_H__

#include "ofThread.h"
#include "ofImage.h"
#include "imgLoader.h"
#include "ofxShader.h"

class Slideshow : public ofThread  {

public:

   Slideshow(string image_dir, string shader_file, int transition_steps, int slide_duration);
   void threadedFunction();
   void update();
   void draw();
   
private:
   ofImage* loadImage(string filePath);
   void fadeInfadeOut(ofImage* currImg,ofImage* nextImg);
   //void shaderTransition(ofImage* currImg,ofImage* nextImg);
   int getCenteredCoordinate(ofImage* image);

   string image_dir;
   string shader_file;
   int transition_steps;

   ofImage * currImage;
   ofImage * nextImg;
   ofTexture   temp_cur_texture;
   ofTexture   temp_next_texture;
   imgLoader   loader;
   bool        loadNextImg;
   bool        transitionEnded;
   bool        transitionStarted;
   float       step;
   float       progress;

   int iStartTime;
   int iSlideDurationMilliseconds;


};

#endif // endof __SLIDE_SHOW_H__

