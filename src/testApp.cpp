#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	bLearnBakground = true;
	threshold = 25;

	PhotoTest.allocate(8,52);
	PhotoTest2.allocate(8*16,52*16);
    nTimesRead  = 0;
	nBytesRead  = 0;
	nRead  = 0;
	FirstByteReceived = 0;
	LastByteReceived = 0;
	zoom = 14;

	serial.enumerateDevices();
    serial.setup("\\\\.\\COM15",115200);
    memset(bytesReadString, 0, 416);
    memset(PixelsReadPhoto, 0, 416);
    memset(ReadPhoto, 0, 52*8);

    serial.flush(true,true);

    sender.setup( HOST, PORT );

}

//--------------------------------------------------------------
void testApp::update(){

    //Initialization
    //{
        ofBackground(100,100,100);
        unsigned char bytesReturned[416];
        int base_index=0;
        int i=0;
        int j=0;
        nTimesRead = 0;
        nBytesRead = 0;
        //nRead  = 0;
        memset(bytesReturned, 0, 416);
    //}

    //Get_Image
    //{
        //serial.flush(true,true);
        serial.writeByte('s');
        #ifdef TARGET_WIN32
            Sleep(15);         //windows sleep in milliseconds
        #else
            usleep(15 * 1000);   //mac sleep in microseconds - cooler :)
        #endif
        while((FirstByteReceived=serial.readByte())!=255)nRead++;

        int ByteReceived = 0;

        while(  nTimesRead < 416)
        {
            ByteReceived=serial.readByte();
            if(ByteReceived!=-1)// && ByteReceived!=0)
            {
                //bytesReturned[nTimesRead] = ByteReceived;
                if(ByteReceived<128) ByteReceived=128;
                bytesReturned[nTimesRead]=(unsigned char)2*(ByteReceived-128);
                //bytesReturned[nTimesRead]=(unsigned char)ByteReceived;
                nTimesRead++;
            }
            nBytesRead++;
        }
        LastByteReceived=serial.readByte();

        memcpy(bytesReadString, bytesReturned, 416);
        for(int i=0;i<52*8;i++) PixelsReadPhoto[i]=bytesReadString[4*(i%104)+i/104]; //Sorting the data into the photo
        PhotoTest.setFromPixels(PixelsReadPhoto,8,52);
    //}

    //Zooming
    //{
        PhotoTest2.resize(PhotoTest.width,PhotoTest.height);
        PhotoTest2=PhotoTest;
        PhotoTest2.resize(8*zoom,52*zoom);
    //}

    //Find_Blobs
    //{
        PhotoTest2.threshold(threshold);
        contourFinder.findContours(PhotoTest2, 20, (52*zoom*8*zoom)/3, 10, true);	// find holes
        cout << contourFinder.nBlobs << endl;
    //}

    //Send_OSC
    //{
        if (contourFinder.nBlobs)
        {
            float xPos,yPos;
            ofxOscMessage m;
            m.setAddress( "/blobFound" );
            m.addIntArg( contourFinder.nBlobs );
            for (int i=0; i < contourFinder.nBlobs; i++)
            {
                xPos = contourFinder.blobs[i].centroid.x;
                yPos = contourFinder.blobs[i].centroid.y;
                cout << xPos << endl;
                cout << yPos << endl;
                m.addFloatArg( xPos );
                m.addFloatArg( yPos );
            }
            sender.sendMessage( m );
        }
        else
        {
            cout << "noBlobFound" << endl;
            ofxOscMessage m;
            m.setAddress( "/noBlobFound" );
            sender.sendMessage( m );
        }
    //}

}

//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetColor(0xffffff);
	PhotoTest2.draw(20,20); //main
	//PhotoTest.draw(20,20+52*20+20);

    // then draw the contours:
	ofFill();
	ofSetColor(0x333333);
	ofRect(zoom*10,20,8*zoom,52*zoom);
	ofSetColor(0xffffff);

	// we could draw the whole contour finder
	//contourFinder.draw(360,540);

	// or, instead we can draw each blob individually,
	// this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(zoom*10,20);
    }

	// finally, a report:

	ofSetColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr,  "threshold %i (press: +/-)\n X num blobs found:%i\n X fps:%f\n X first Byte:%i\n X Last Byte:%i\n X Buffer Bytes extra readed:%i\n X #Image Bytes:%i\n X #Image Bytes Total:%i\n X bytesReadString[4*(1%104+1/104)]:%i\n X bytesReadString[4*(21%104+21/104)]:%i\n X bytesReadString[4*(49%104+49/104)]:%i\n X bytesReadString[4*(113%104+113/104)]:%i\n X bytesReadString[4*(189%104+189/104)]:%i\n X bytesReadString[4*(234%104+234/104)]:%i\n X Pixels[1]:%i\n X Pixels[21]:%i\n X Pixels[49]:%i\n X Pixels[113]:%i\n X Pixels[189]:%i\n X Pixels[234]:%i"
                        , threshold
                        , contourFinder.nBlobs
                        , ofGetFrameRate()
                        , FirstByteReceived
                        , LastByteReceived
                        , nRead
                        , nTimesRead
                        , nBytesRead
                        , bytesReadString[4*(1%104+1/104)]
                        , bytesReadString[4*(21%104+21/104)]
                        , bytesReadString[4*(49%104+49/104)]
                        , bytesReadString[4*(113%104+113/104)]
                        , bytesReadString[4*(189%104+189/104)]
                        , bytesReadString[4*(234%104+234/104)]
                        , PixelsReadPhoto[1]
                        , PixelsReadPhoto[21]
                        , PixelsReadPhoto[49]
                        , PixelsReadPhoto[113]
                        , PixelsReadPhoto[189]
                        , PixelsReadPhoto[234]
                        );
	ofDrawBitmapString(reportStr, 20*zoom, 20);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
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

