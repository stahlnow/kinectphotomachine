#include "testApp.h"
#include "Arduino.h"


//--------------------------------------------------------------
void testApp::setup() {

   //arduino.startThread(true, false);
   
   ofSetFrameRate(30);
   kinect.init();

   kinect.open();
   kinect.setRegistration(true);

   blob.allocate(640, 480, OF_IMAGE_GRAYSCALE);

   // Gui
   gui.setup();
   gui.setPosition(ofPoint(10,10));
   gui.add(noiseAmount.setup("Noise Amount", 0.0, 0.0, 20.0));
   gui.add(pointSkip.setup("Point Skip", 3, 1, 20));
   gui.add(useRealColors.setup("Real Colors", true));
   gui.add(colorAlpha.setup("Color Alpha", 255, 0, 255));
   gui.add(screenRotation.setup("Screen Rotation", 0, 0, 270));
   gui.loadFromFile("settings.xml");
   showGui = false;

   // load other settings
   ofxXmlSettings settings;
   settings.loadFile("settings.xml");
   serial_port = settings.getValue("settings:serial_port", "/dev/ttyACM0");
   mobile_printer_name = settings.getValue("settings:mobile_printer_name", "");
   mobile_printer_format = settings.getValue("settings:mobile_printer_format", "");
   mobile_printer_quality = settings.getValue("settings:mobile_printer_quality", "");
   mobile_printer_media_type = settings.getValue("settings:mobile_printer_media_type", "");
    
    printer_name = settings.getValue("settings:printer_name", "");
    printer_format = settings.getValue("settings:printer_format", "");
    printer_quality = settings.getValue("settings:printer_quality", "");
    printer_media_type = settings.getValue("settings:printer_media_type", "");


   cout << "Settings:" << endl;
    cout << "Serial Port" << endl;
   cout << serial_port << endl;
    cout << "Mobile Printer:" << endl;
   cout << mobile_printer_name << endl;
   cout << mobile_printer_format << endl;
   cout << mobile_printer_quality << endl;
   cout << mobile_printer_media_type << endl;
    cout << "Printer Station:" << endl;
    cout << printer_name << endl;
    cout << printer_format << endl;
    cout << printer_quality << endl;
    cout << printer_media_type << endl;

   // fx
   postFx.init(ofGetWidth(), ofGetHeight());

   postFx.createPass<BloomPass>();
   postFx.createPass<FxaaPass>();
   

}



//--------------------------------------------------------------
void testApp::update(){

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
            c = ofColor(255,0,0);

         ofColor o = ofColor(255, 95, 0);
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

      delete pix;
      delete gpix;

   }


}


//--------------------------------------------------------------
void testApp::draw(){
   ofBackground(0, 0, 0);
   //glEnable(GL_DEPTH_TEST);

   ofPushMatrix();

   cam.begin();
   cam.setScale(1,-1,1);

   ofSetColor(255, 255, 255);
   ofTranslate(0, -80, 1100);
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
   wireframeMesh.drawWireframe();
   ofPopMatrix();
   cam.end();
   ofPopMatrix();

   postFx.end();

   if(showGui) {
      ofPushStyle();
      ofSetColor(255,255,255,255);
      gui.draw();
      ofPopStyle();
   }
   ofSetColor(255, 255, 255);



   // loading animation
   if (!isPrinting && isLoading) {

       /*
      ofPushMatrix();
      ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

      int resolution = 1024;
      int radius = 100;
      double w = 2;
      float loaded = ofMap(mouseX, 0, ofGetWidth(), 0, 1);

      ofxFatLineOptions opt;
      opt.feather = 20.0;

      const int size_of_v = 2;

      ofVec2f v[size_of_v];
      v[0].x=0; v[0].y=0;
      v[1].x=0; v[1].y=radius;

      ofFloatColor color[size_of_v];
      color[0].set(pantone712c);
      color[1].set(pantone165c);

      double weight[size_of_v];
      weight[0] = w;
      weight[1] = w;

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      //glEnable(GL_BLEND);


      float minTheta = 0, maxTheta = loaded;
      for (int i = 0; i < resolution; i++) {
         float theta  = ofMap(i, 0, resolution - 1, minTheta, maxTheta);
         v[1].rotate(theta);
         ofxFatLine(v, color, weight, size_of_v, &opt, true);
      }

      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);

      ofPopMatrix();
    */
   }

}


//--------------------------------------------------------------
void testApp::keyPressed(int key) {

   if (key == 'l') {
      if (!isLoading)
         isLoading = true;
      else
         isLoading = false;
   }

   if (key == 'p') {
      
      isPrinting = true;

      string file = "photo.png";

      ofImage photo;
      cout << ofGetWidth() << "x"<< ofGetHeight() << endl;
      photo.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
      
      // adjust aspect ratio to 15:10 (photo paper)
      if (ofGetWidth() == 1600)
         photo.crop(125, 0, 1350, 900);
      else if (ofGetWidth() == 1920)
         photo.crop(150, 0, 1620, 1080);
      else if (ofGetWidth() == 1080)
          photo.crop(0, 150, 1080, 1620);
      photo.saveImage(file);	

    
      // print photo on mobile printer
      string mobile_command("lp -d " + mobile_printer_name + " -o media=" + mobile_printer_format + "," + mobile_printer_media_type + " /Users/zaak/Documents/of_v0.7.4_osx_release/apps/myApps/emex2013/bin/data/" + file);
      system(mobile_command.c_str());
       
      string command("lp -d " + printer_name + " -o media=" + printer_format + "," + printer_media_type + " /Users/zaak/Documents/of_v0.7.4_osx_release/apps/myApps/emex2013/bin/data/" + file);
      system(command.c_str());
     

   }
   if (key == ' ') {
      showGui = !showGui;
   }
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

void testApp::exit() {
   
   // close serial port and stop thread
   //arduino.lock();
   //arduino.serial.close();
   //arduino.unlock();
   arduino.stopThread();

   // close kinect
   kinect.close();

   // save settings
   gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
