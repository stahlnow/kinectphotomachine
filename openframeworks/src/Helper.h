#ifndef __HELPER_H__
#define __HELPER_H__

#include "ofColor.h"

class Helper {

public:
    
    static bool isLoading;      
    static bool isButtonActive; // true, if user touches button
    static bool canSwitch; // true, if user can switch mode
    
   static const ofColor darkOrange;

   static const ofColor pantone165c;
   static const ofColor pantone163c;
   static const ofColor pantone712c;

   static const ofFloatColor pantone165c_f;
   static const ofFloatColor pantone163c_f;
   static const ofFloatColor pantone712c_f;
 

};

#endif // __HELPER_H_
