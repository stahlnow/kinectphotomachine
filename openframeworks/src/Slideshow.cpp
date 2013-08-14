#include "Slideshow.h"

Slideshow::Slideshow(string image_dir, string shader_file, int transition_steps, int slide_duration)
   : image_dir (image_dir), shader_file (shader_file), transition_steps(transition_steps)
{
   
   iSlideDurationMilliseconds = slide_duration;

   loader.start(image_dir);

   currImage = NULL;
   nextImg = NULL;
   loadNextImg = false;
   transitionStarted = false;
   transitionEnded = false;
   progress = 0;
   step = (float)transition_steps / 100;
   temp_cur_texture.allocate(1080, 1920, GL_RGB); // CHANGE FOR THE SIZE OF THE IMAGES ...
   temp_next_texture.allocate(1080, 1920, GL_RGB);


}



void Slideshow::threadedFunction() {
   
   iStartTime = ofGetElapsedTimeMillis();
    
   while (isThreadRunning()) {
      
      float timer = ofGetElapsedTimeMillis() - iStartTime;

      if (timer >= iSlideDurationMilliseconds) {
         loadNextImg = true;
         transitionStarted = true;
         iStartTime = ofGetElapsedTimeMillis();
      }
   }
}

void Slideshow::reset() {
    lock();
    progress = 0;
    loadNextImg = true;
    transitionStarted = false;
    iStartTime = ofGetElapsedTimeMillis();
    unlock();
}

void Slideshow::update() {
   
   lock();

   if (currImage != NULL) {
      if (loadNextImg) {
         ofImage* temp_nextImg = loader.getNextTexture();
         nextImg = temp_nextImg;  
         loadNextImg = false;
      }
      if(transitionStarted) {
         progress = progress + step;
         if(transitionEnded) {
            transitionStarted = false;
            ofLog(OF_LOG_VERBOSE,"Animation ended, swapping");
            currImage=nextImg;
            loader.releaseCurrentTexture();
            transitionEnded = false;
            progress = 0;
         }
      }
   } else {
      ofImage* temp_nextImg = loader.getNextTexture();
      currImage = temp_nextImg;  
   }

   unlock();
}

void Slideshow::draw() {
   lock();
   ofSetVerticalSync(true);

   if (currImage!=NULL) {
      if (transitionStarted && nextImg != NULL) {
         transitionEnded = false;
         fadeInfadeOut(currImage, nextImg);
      } else {
         ofSetHexColor(0xffffff);
         temp_cur_texture.loadData(currImage->getPixels(), currImage->width, currImage->height, GL_RGB);
         temp_cur_texture.draw(getCenteredCoordinate(currImage), 0, currImage->width, currImage->height);
      }
   }

   unlock();
}

void Slideshow::fadeInfadeOut(ofImage* currImg, ofImage* nextImg) {
   glEnable(GL_BLEND);
   glColor4f(1.0f, 1.0f, 1.0f,1 - (progress * 2));   
   glBlendFunc(GL_SRC_ALPHA, GL_ZERO);   // takes src brightness (ignore dest color)	
   temp_cur_texture.loadData(currImg->getPixels(),currImg->width,currImg->height,GL_RGB);
   temp_cur_texture.draw(getCenteredCoordinate(currImg),0,currImg->width,currImg->height); //
   //If the second image disappeared
   if (progress >= 0.5) {
      //Blend della seconda immagine
      glColor4f(1.0f, 1.0f, 1.0f,(progress - 0.5) * 2);   
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);   // takes src brightness (ignore dest color)	
      temp_cur_texture.loadData(nextImg->getPixels(),nextImg->width,nextImg->height,GL_RGB);
      temp_cur_texture.draw(getCenteredCoordinate(nextImg),0,nextImg->width,nextImg->height); //
      if (progress >= 1.0) transitionEnded = true;
   }
}

int Slideshow::getCenteredCoordinate(ofImage* image) {
   int x = 0;
   if (image->width != 1080) x = 540 - (image->width / 2);
   return x;
}


