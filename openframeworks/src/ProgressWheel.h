#ifndef __PROGRESS_WHEEL_H__
#define __PROGRESS_WHEEL_H__

#include "Helper.h"
#include "ofxGradientShape.h"


class ProgressWheel : public ofThread  {

public:

   ProgressWheel() {
      
      ofVec3f centre = ofVec3f (0,0,0);
      // centre, BHGShapeType, BHGNumSides, BHGBlur, BHGThickness, BHGDiameter, color, BHGDegree
      addGradientShape(centre, 0, 12, 2, 1, 100, Helper::darkOrange, 0); // progress
      addGradientShape(centre, 1, 12, 2, 2, 120, Helper::darkOrange, 360); // outline

      iButtonEndTime = 50; // 50 milliseconds for one loading step (BHGNumSides)

   }

   void threadedFunction() {
      
      while(isThreadRunning()) {

         float timer = ofGetElapsedTimeMillis() - iButtonStartTime;

         if(timer >= iButtonEndTime) {
            shapes[0].run(); // advance loading animation one step
            iButtonStartTime = ofGetElapsedTimeMillis(); // reset start time
         }
      }
   }

   void addGradientShape(ofVec3f centre, int BHGShapeType, int BHGNumSides, float BHGBlur, float BHGThickness, float BHGDiameter, ofColor c_, int BHGDegree) {
      shapes.push_back(ofxGradientShape(shapeCount, centre, BHGShapeType, BHGNumSides, BHGBlur, BHGThickness,BHGDiameter, c_,BHGDegree));
      shapeCount++;
   }
   
   vector <ofxGradientShape> shapes;
   int shapeCount;
   bool bButtonTimerReached;
   int iButtonStartTime;
   int iButtonEndTime;


   
};

#endif // endof __PROGRESS_WHEEL_H__

