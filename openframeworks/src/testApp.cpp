#include "testApp.h"

void testApp::setup() {

   ofSetFrameRate(30);
   ofHideCursor();
   
   // gui
   gui.setup();
   gui.setPosition(ofPoint(10,40));
   gui.add(noiseAmount.setup("Noise Amount", 0.0, 0.0, 20.0));
   gui.add(pointSkip.setup("Point Skip", 3, 1, 20));
   gui.add(useRealColors.setup("Real Colors", true));
   gui.add(drawWireMesh.setup("Wire Mesh", false));
   gui.add(colorAlpha.setup("Color Alpha", 255, 0, 255));
    gui.add(kinectRadius.setup("Camera Radius", 100, 0, 1000));
   gui.add(kinectTilt.setup("Camera Orbit Y", 0, -45, 45));
   gui.add(kinectYTranslation.setup("Camera Y", 200, -1000, 1000));
   gui.add(kinectZTranslation.setup("Camera Z", 1920, -3000, 3000));
   gui.add(kinectRed.setup("Red", 255, 0, 255));
   gui.add(kinectGreen.setup("Green", 255, 0, 255));
   gui.add(kinectBlue.setup("Blue", 255, 0, 255));
   gui.add(screenRotation.setup("Screen Rotation", 0, 0, 270));
   gui.add(hasSlideshow.setup("Slideshow", false));
   gui.add(slideDuration.setup("Slide Duration", 5, 2, 120));
   gui.add(slideShowSwitchTimeout.setup("Kinect Timeout", 2, 1, 20));
   gui.add(sensorMinimum.setup("Sensor Minimum", 300, 0, 100000));
   gui.add(sensorMaximum.setup("Sensor Maximum", 15000, 0, 100000));
   gui.loadFromFile("settings.xml");    
   showGui = false;

   // load settings
   ofxXmlSettings settings;
   settings.loadFile("settings.xml");

   log_level = settings.getValue("settings:log_level", "verbose");
   serial_port = settings.getValue("settings:serial_port", "/dev/ttyACM0");
   mobile_printer_name = settings.getValue("settings:mobile_printer_name", "");
   mobile_printer_format = settings.getValue("settings:mobile_printer_format", "");
   mobile_printer_quality = settings.getValue("settings:mobile_printer_quality", "");
   mobile_printer_media_type = settings.getValue("settings:mobile_printer_media_type", "");

   printer_name = settings.getValue("settings:printer_name", "");
   printer_format = settings.getValue("settings:printer_format", "");
   printer_quality = settings.getValue("settings:printer_quality", "");
   printer_media_type = settings.getValue("settings:printer_media_type", "");

   slides_dir = settings.getValue("settings:slides_dir", "slides");
   shader_file = settings.getValue("settings:shader_file", "SlideIn");
   transition_steps = settings.getValue("settings:transition_steps", 2);
    
   photo_dir = settings.getValue("settings:photo_dir", "photos");

   if (log_level == "verbose") {
      ofSetLogLevel(OF_LOG_VERBOSE);
   }

   ofLog(OF_LOG_NOTICE, "### Slides directory: " + slides_dir);
   ofLog(OF_LOG_NOTICE, "### Shader: " + shader_file);
   ofLog(OF_LOG_NOTICE, "### Transition steps: %i", transition_steps);
   ofLog(OF_LOG_NOTICE, "### Serial Port: " + serial_port);

   ofLog(OF_LOG_NOTICE, "### Mobile Printer: " + mobile_printer_name);
   ofLog(OF_LOG_NOTICE, "\tFormat: " + mobile_printer_format);
   ofLog(OF_LOG_NOTICE, "\tQuality: " + mobile_printer_quality);
   ofLog(OF_LOG_NOTICE, "\tMedia Type: " + mobile_printer_media_type); 
   ofLog(OF_LOG_NOTICE, "### Station Printer: " + printer_name);
   ofLog(OF_LOG_NOTICE, "\tFormat: " + printer_format);
   ofLog(OF_LOG_NOTICE, "\tQuality: " + printer_quality);
   ofLog(OF_LOG_NOTICE, "\tMedia Type: " + printer_media_type); 


    // font for countdown
    font.loadFont("EurosBlaDTC", 200); // must be placed in /data folder
    // font for text
    fontText.loadFont("EurosBlaDTC", 30);
    
   // start arduino serial
   arduino = new Arduino(serial_port, sensorMinimum, sensorMaximum);
   arduino->startThread(true, false);

   // kinect
   kinect.init();
   kinect.open();
   kinect.setRegistration(true);
   blob.allocate(640, 480, OF_IMAGE_GRAYSCALE);
   postFx.init(ofGetWidth(), ofGetHeight());
   postFx.createPass<BloomPass>();
   postFx.createPass<FxaaPass>();

   // slideshow
   slideshow = new Slideshow(slides_dir, shader_file, transition_steps, slideDuration * 1000);
   slideshow->startThread(true, false);  

    
   // select mode
    if (hasSlideshow) {
       switchMode(MODE_SLIDESHOW);
    }
    else {
       switchMode(MODE_KINECT);
    }
    
    
    cam.begin();
       
    
    cam.end();
    
}


//--------------------------------------------------------------
void testApp::update(){

  
   float timer = ofGetElapsedTimeMillis() - slideShowSwitchStartTime;
    
   switch(_mode) {

   case MODE_KINECT:
      
      if (timer >= slideShowSwitchTimeout * 1000 * 60) {
          switchMode(MODE_SLIDESHOW);
          slideShowSwitchStartTime = ofGetElapsedTimeMillis();
      }
           
      if (!freeze)
          kinect.update();
           
      if(kinect.isFrameNew()) {
         del.reset();

         unsigned char* pix = new unsigned char[640*480];
         unsigned char* gpix = new unsigned char[640*480];

         for(int x=0;x<640;x+=1) {
            for(int y=0;y<480;y+=1) {
               float distance = kinect.getDistanceAt(x, y);

               int pIndex = x + y * 640;
               pix[pIndex] = 0;

               if(distance > 100 && distance < 1100) {
                  pix[pIndex] = 255;
               }

            }
         }

         blob.setFromPixels(pix, 640, 480, OF_IMAGE_GRAYSCALE);

         int numPoints = 0;

         for(int x=0;x<640;x+=pointSkip*2) {
            for(int y=0;y<480;y+=pointSkip*2) {
               int pIndex = x + 640 * y;

               if(blob.getPixels()[pIndex]> 0) {
                  ofVec3f wc = kinect.getWorldCoordinateAt(x, y);

                  wc.x = x - 320.0;
                  wc.y = y - 240.0;

                  if(abs(wc.z) > 100 && abs(wc.z ) < 2000) {

                     wc.z = -wc.z;

                     wc.x += ofSignedNoise(wc.x,wc.z)*noiseAmount;
                     wc.y += ofSignedNoise(wc.y,wc.z)*noiseAmount;

                     wc.x = ofClamp(wc.x, -320,320);
                     wc.y = ofClamp(wc.y, -240,240);

                     del.addPoint(wc);
                  }
                  numPoints++;
               }

            }
         }


         if(numPoints >0)
            del.triangulate();

         for(int i=0;i<del.triangleMesh.getNumVertices();i++) {
            del.triangleMesh.addColor(ofColor(0,0,0));
         }

         for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {
            ofVec3f v = del.triangleMesh.getVertex(del.triangleMesh.getIndex(i*3));

            v.x = ofClamp(v.x, -319,319);
            v.y = ofClamp(v.y, -239, 239);

            ofColor c = kinect.getColorAt(v.x+320.0, v.y+240.0);

            if(!useRealColors)
               c = ofColor(255,250,250);

            // multiply by other color
            ofColor o = ofColor(kinectRed, kinectGreen, kinectBlue);  // Helper::pantone712c; // ofColor(255, 95, 0);
            c *= o;

            c.a = colorAlpha;

            del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3),c);
            del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+1),c);
            del.triangleMesh.setColor(del.triangleMesh.getIndex(i*3+2),c);
         }




         convertedMesh.clear();
         wireframeMesh.clear();
         wireframeMesh.setMode(OF_PRIMITIVE_TRIANGLES);
         for(int i=0;i<del.triangleMesh.getNumIndices()/3;i+=1) {

            int indx1 = del.triangleMesh.getIndex(i*3);
            ofVec3f p1 = del.triangleMesh.getVertex(indx1);
            int indx2 = del.triangleMesh.getIndex(i*3+1);
            ofVec3f p2 = del.triangleMesh.getVertex(indx2);
            int indx3 = del.triangleMesh.getIndex(i*3+2);
            ofVec3f p3 = del.triangleMesh.getVertex(indx3);

            ofVec3f triangleCenter = (p1+p2+p3)/3.0;
            triangleCenter.x += 320;
            triangleCenter.y += 240;

            triangleCenter.x = floor(ofClamp(triangleCenter.x, 0,640));
            triangleCenter.y = floor(ofClamp(triangleCenter.y, 0, 480));

            int pixIndex = triangleCenter.x + triangleCenter.y * 640;
            if(pix[pixIndex] > 0) {

               convertedMesh.addVertex(p1);
               convertedMesh.addColor(del.triangleMesh.getColor(indx1));

               convertedMesh.addVertex(p2);
               convertedMesh.addColor(del.triangleMesh.getColor(indx2));

               convertedMesh.addVertex(p3);
               convertedMesh.addColor(del.triangleMesh.getColor(indx3));

               wireframeMesh.addVertex(p1);
               wireframeMesh.addVertex(p2);
               wireframeMesh.addVertex(p3);
            }
         }

         delete []pix;
         delete []gpix;

      }

      break;

   case MODE_SLIDESHOW:
      slideshow->update();
      break;

   default:
      break;
   };
    
    
    if (!progressWheel.isThreadRunning() && Helper::isButtonActive && Helper::canSwitch) {
        progressWheel.startThread(true, false);
        showWheel = true;
    } else if (!Helper::isButtonActive) {
        progressWheel.shapes[0].BHGDegree = 0;
        progressWheel.stopThread();
        showWheel = false;
    }


}


//--------------------------------------------------------------
void testApp::draw(){

   // black background
   ofBackground(0, 0, 0);
   
   switch(_mode) {

   case MODE_KINECT:
   
      //glEnable(GL_DEPTH_TEST);

      
      ofPushMatrix();

      cam.begin();
      cam.resetTransform();
      cam.setScale(-1,-1,1);
           
      cam.orbit(0, -kinectTilt, kinectRadius);
     
      cam.move(0, kinectYTranslation, kinectZTranslation);
      
      ofSetColor(255, 255, 255);
    
      cam.setNearClip(1) ;
      cam.setFarClip(10000);
           
      
      ofFill();

      postFx.begin();

      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glShadeModel(GL_FLAT);
      glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
      ofRotate(screenRotation); // rotate for portrait mode
      convertedMesh.drawFaces();
      glShadeModel(GL_SMOOTH);
      glPopAttrib();

      if(useRealColors) {
         ofSetColor(10, 10, 10, 15);
      } else
         ofSetColor(124, 136, 128, 255);

      ofPushMatrix();
      ofTranslate(0, 0, 0.5);
      if (drawWireMesh)
         wireframeMesh.drawWireframe();
      ofPopMatrix();
      cam.end();
      ofPopMatrix();

      postFx.end();

      break;

   case MODE_SLIDESHOW:
      slideshow->draw();

      break;

   default:
      break;
   };


   // draw gui
   if(showGui) {
      ofPushStyle();
      ofSetColor(255,255,255,255);
      gui.draw();
       /*
       arduino->lock();
       ostringstream v; v << arduino->getValue();
       font.drawString(v.str(), ofGetWidth() / 2, ofGetHeight() - 100);
       arduino->unlock();
       */
      ofPopStyle();
   }
   
   
    if (startPhotoCountdown) {
        
        Helper::canSwitch = false;
        
        ofPushMatrix();
        ofSetColor(255,255,255,255);
        ofEnableAlphaBlending();
        ofEnableBlendMode(OF_BLENDMODE_ADD);

        countdown.lock();
        
        if (countdown.getSeconds() == 0) { // freeze
            
            freeze = true;
            
            // print photo (must be after freeze=true)
            printPhoto();
        }
        
        else if (countdown.getSeconds() < 0 && countdown.getSeconds() > -40) {
            fontText.drawString("Printing in process...",
                                (ofGetWidth() - fontText.stringWidth("Printing in process..."))/2,
                                ofGetHeight() - 150);

        }
        
        else if (countdown.getSeconds() <= -40 ) { // switch mode
            
            Helper::canSwitch = true;
            
            freeze = false;
            
            countdown.stopThread();
            startPhotoCountdown = false;
            
            if (hasSlideshow) {
                switchMode(MODE_SLIDESHOW);
            } else {
                switchMode(MODE_KINECT);
            }
        
        } else if (countdown.getSeconds() > 0) {                                       // show countdown
            ostringstream sec; sec << countdown.getSeconds();
            float dx = font.stringWidth(sec.str());
            float dy = font.stringHeight(sec.str());
            font.drawString(sec.str(), (ofGetWidth() - dx) / 2, ofGetHeight() - 300);
        }
        
        countdown.unlock();
        
        ofDisableAlphaBlending();
        ofPopMatrix();
        
    }

   // show loading wheel
    if (showWheel) {
      ofPushMatrix();
      ofTranslate(ofGetWidth() / 2, ofGetHeight() - (2*progressWheel.getRadius()) - 10);
      ofEnableAlphaBlending();
      ofEnableBlendMode(OF_BLENDMODE_ALPHA); // _ADD for transperancy
      glEnableClientState(GL_COLOR_ARRAY);
      glEnableClientState(GL_VERTEX_ARRAY);
      for(int i=0;i<progressWheel.shapes.size();i++) {
          progressWheel.lock();
         (progressWheel.shapes)[i].renderBHGradients();
          progressWheel.unlock();
      }
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
      ofDisableAlphaBlending();
      ofPopMatrix();
      
      // check if loaded
        progressWheel.lock();
      if (progressWheel.shapes[0].BHGDegree == 360) {
         progressWheel.shapes[0].BHGDegree = 0;
         progressWheel.stopThread();
         Helper::canSwitch = false;
         showWheel = false;
         
          switch(_mode) {
                  
              case MODE_SLIDESHOW:
                  switchMode(MODE_KINECT);
                  break;
                  
              case MODE_KINECT:
                  slideShowSwitchStartTime = ofGetElapsedTimeMillis();
                  countdown.setSeconds(5);
                  countdown.startThread(true, false);
                  startPhotoCountdown = true;
                  
                  break;
              default:
                  break;
          
          };
      }
        progressWheel.unlock();
    }
    
    
    if (_mode == MODE_KINECT && !freeze) {
        fontText.drawString("Touch the button\nto take a picture",
                            (ofGetWidth() - fontText.stringWidth("Touch the button\nto take a picture"))/2,
                            ofGetHeight() - 150);
    }
    
    
}

void testApp::switchMode(int mode) {
    if (mode == MODE_KINECT) {
        slideShowSwitchStartTime = ofGetElapsedTimeMillis();
        _mode = MODE_KINECT;
    } else if (mode == MODE_SLIDESHOW) {
        if (hasSlideshow) {
            slideshow->reset();
            _mode = MODE_SLIDESHOW;
        }
    }
}

void testApp::printPhoto() {
    // create filename
    time_t rawtime;
    struct tm * timeinfo;
    char file [80];
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (file, 80, "photo-%F_%H-%M-%S.png", timeinfo);
    
    string fullpath = ofToDataPath(photo_dir + "/" + file);
    
    // grab screen
    ofImage photo;
    //cout << ofGetWidth() << "x"<< ofGetHeight() << endl;
    photo.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    
    // adjust aspect ratio to 15:10 (photo paper), crop(x,y,width,height), where x,y is position.
    if (ofGetWidth() == 1920)
        photo.crop(150, 0, 1620, 1080);
    
    else if (ofGetWidth() == 1600)
        photo.crop(125, 0, 1350, 900);
    
    // portrait modes
    else if (ofGetWidth() == 1080)
        photo.crop(0, 150, 1080, 1620);
    
    else if (ofGetWidth() == 900)
        photo.crop(0, 125, 900, 1350);
    
    // save image
    photo.saveImage(fullpath);
    
#ifdef __APPLE__
    // convert to cmyk
    string conversion_command("sips -m " + ofToDataPath("CMYK.icc") + " " + fullpath);
    system(conversion_command.c_str());
#endif
    
    // print photo on mobile printer
    string mobile_command("lp -d " + mobile_printer_name + " -o media=" + mobile_printer_format + "," + mobile_printer_media_type +  " " + fullpath);
    system(mobile_command.c_str());
    
    string command("lp -d " + printer_name + " -o media=" + printer_format + "," + printer_media_type + " " + fullpath);
    system(command.c_str());
}


//--------------------------------------------------------------
void testApp::keyPressed(int key) {


   switch(key) {
   case 'm':
      if (_mode == MODE_KINECT) {
         switchMode(MODE_SLIDESHOW);
      } else {
         switchMode(MODE_KINECT);
      }
      break;

  
   case 'p':
      {
          printPhoto();
      }
      break;

   case '1':
      iKinectServoAngle++;
      if(iKinectServoAngle > 30) iKinectServoAngle = 30;
      kinect.setCameraTiltAngle(iKinectServoAngle);
      break;

   case '2':
      iKinectServoAngle--;
      if (iKinectServoAngle < -30) iKinectServoAngle = -30;
      kinect.setCameraTiltAngle(iKinectServoAngle);
      break;

   case ' ':
      showGui = !showGui;
      if (showGui)
          ofShowCursor();
      else
          ofHideCursor();
               
      break;

   default:
      break;

   };
}


void testApp::exit() {

   // auto save gui settings
   gui.saveToFile("settings.xml");
  
   // close serial port and stop thread
   if (arduino != 0) {
      arduino->lock();
      arduino->serial.close();
      arduino->unlock();
      if (arduino->isThreadRunning()) arduino->waitForThread(true);
      delete arduino;
      arduino = 0;
   }

   progressWheel.waitForThread(true);

   // remove slideshow
   if (slideshow->isThreadRunning()) slideshow->waitForThread(true); // stop thread
   delete slideshow;
   slideshow = 0;

   // close kinect
   kinect.close();

}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
