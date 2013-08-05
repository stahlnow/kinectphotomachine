#ifndef _TEXTURELOADER
#define _TEXTURELOADER

#include "ofMain.h"
#include "ofxDirList.h"

class imgLoader : public ofThread {
    public:
        imgLoader();
        ~imgLoader();

        void    start(string pathToImages);
        void    stop();
        void    threadedFunction();

        void    fetchTextureNames(string path);
        void    loadNextTexture();
        ofImage * getNextTexture();
        void    goForIt();
		void    releaseCurrentTexture();

    private:
        ofxDirList  dirList;
        string*     textures;
        int         numTextures;
        int         textureIndex;
        bool        go;

        ofImage * currTexture;
        ofImage * nextTexture;
	ofImage * to_delete_img;
        bool imgLoaded;

};

#endif
