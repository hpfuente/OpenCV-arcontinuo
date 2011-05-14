#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include <windows.h>

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file


class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        ofxCvContourFinder 	contourFinder;

		int 				threshold;
		bool				bLearnBakground;

		ofxCvGrayscaleImage 	PhotoTest;
		ofxCvGrayscaleImage 	PhotoTest2;
		unsigned char           PixelesPhotoTest[320*240];

        ofSerial	serial;
        int nTimesRead  ;
        int nBytesRead  ;
        int nRead       ;
        int zoom        ;
        unsigned char		bytesRead[416];				// data from serial, we will be trying to read 3
		unsigned char		bytesReadString[416];			// a string needs a null terminator, so we need 3 + 1 bytes
		unsigned char		PixelsReadPhoto[416];			// a string needs a null terminator, so we need 3 + 1 bytes
        unsigned char		ReadPhoto[52][8];			// a string needs a null terminator, so we need 3 + 1 bytes

//        ofxCvGrayscaleImage 	PhotoTest3;
//		ofxCvGrayscaleImage 	PhotoTest4;
//		unsigned char		    PhotoTest3Pixels[4];
		unsigned char           FirstByteReceived;
		unsigned char           LastByteReceived;


};

#endif
