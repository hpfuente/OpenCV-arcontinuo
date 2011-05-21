#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	threshold = 25;

	PhotoTest.allocate(8,52);
	PhotoTest2.allocate(8*16,52*16);
    nTimesRead  = 0;
	nBytesRead  = 0;
	nRead  = 0;
	FirstByteReceived = 0;
	LastByteReceived = 0;
	zoom = 14;
	test=1000;

	serial.enumerateDevices();
    serial.setup("\\\\.\\COM15",115200);
    memset(bytesReadString, 0, 416);
    memset(PixelsReadPhoto, 0, 416);
    memset(ReadPhoto, 0, 52*8);

    serial.flush(true,true);

    sender.setup( HOST, PORT );

    blobTracker.setListener( this );

    memset(FingersBlobs, 0, 10*2*sizeof FingersBlobs[0][0]);
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
            Sleep(20);         //windows sleep in milliseconds
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



        for(int i=0;i<52*8;i++)
        {

            bytesReadString[232]=0;
            PixelsReadPhoto[i]=bytesReadString[4*(i%104)+i/104]; //Sorting the data into the photo
            if(PixelsReadPhoto[i]>200)test=(unsigned char)i;
        }
        PixelsReadPhoto[94]=0;

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
        //cout << contourFinder.nBlobs << endl;
    //}

        blobTracker.trackBlobs( contourFinder.blobs );

//    //Send_OSC
//    //{
//        if (contourFinder.nBlobs)
//        {
//            float xPos,yPos,zPos;
//            ofxOscMessage m;
//            m.setAddress( "/updateBlob" );
//            m.addIntArg( contourFinder.nBlobs );
//            for (int i=0; i < contourFinder.nBlobs; i++)
//            {
//                xPos = contourFinder.blobs[i].centroid.x;
//                yPos = contourFinder.blobs[i].centroid.y;
//                zPos = contourFinder.blobs[i].area;
//                cout << xPos << endl;
//                cout << yPos << endl;
//                cout << zPos << endl;
//                m.addFloatArg( xPos );
//                m.addFloatArg( yPos );
//                m.addFloatArg( zPos );
//            }
//            sender.sendMessage( m );
//        }
//        else
//        {
//            cout << "noBlobFound" << endl;
//            ofxOscMessage m;
//            m.setAddress( "/noBlobFound" );
//            sender.sendMessage( m );
//        }
//    //}

}

//--------------------------------------------------------------
void testApp::draw(){


	ofSetColor(0xffffff);
	PhotoTest2.draw(20,20);

	ofFill();
	ofSetColor(0x333333);
	ofRect(zoom*10,20,8*zoom,52*zoom);
	ofSetColor(0xffffff);
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(zoom*10,20);
    }

    ofSetColor(0x000000);
	ofRect(zoom*20,20,8*zoom,52*zoom);
	ofSetColor(0xffffff);
    blobTracker.draw(zoom*20,20);

	// finally, a report:

	ofSetColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr,  "threshold %i (press: +/-)\n X num blobs found:%i\n X fps:%f\n X first Byte:%i\n X Last Byte:%i\n X Buffer Bytes extra readed:%i\n X #Image Bytes:%i\n X #Image Bytes Total:%i\n Pos:%i"
                        , threshold
                        , contourFinder.nBlobs
                        , ofGetFrameRate()
                        , FirstByteReceived
                        , LastByteReceived
                        , nRead
                        , nTimesRead
                        , nBytesRead
                        , test
                        );
	ofDrawBitmapString(reportStr, 30*zoom, 20);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	switch (key){
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

void testApp::blobOn( int x, int y, int id, int order ) {
    cout << "blobOn() - id:" << id << " order:" << order << endl;
    for(int i=0;i<10;i++)
    {
        if(FingersBlobs[i][0]==0)
        {
            FingersBlobs[i][0]=1;
            FingersBlobs[i][1]=id;

            ofxCvTrackedBlob blob = blobTracker.getById( id );

            float xPos,yPos,zPos,area;

            const char base[] = "/addBlob";
            char filename [7];
            sprintf(filename, "%s %d", base, i);
            cout << filename << endl;

            ofxOscMessage m;

            m.setAddress("/addBlob");


            xPos = blob.centroid.x/(8*zoom);
            yPos = blob.centroid.y/(52*zoom);
            zPos = blob.centroid.z;
            area = blob.area;

            m.addIntArg( i );
//            m.addFloatArg( xPos );
//            m.addFloatArg( yPos );
//            m.addFloatArg( zPos );
//            m.addFloatArg( area );

            sender.sendMessage( m );
            break;
        }
    }
}
void testApp::blobMoved( int x, int y, int id, int order) {
    cout << "blobMoved() - id:" << id << " order:" << order << endl;

    // full access to blob object ( get a reference)
    ofxCvTrackedBlob blob = blobTracker.getById( id );
    for(int i=0;i<10;i++)
    {
        if(FingersBlobs[i][1]==id)
        {

            float xPos,yPos,zPos,area;

            const char base[] = "/updateBlob";
            char filename [7];
            sprintf(filename, "%s %d", base, i);

            ofxOscMessage m;

            m.setAddress("/updateBlob");

            xPos = blob.centroid.x/(8*zoom);
            yPos = blob.centroid.y/(52*zoom);
            zPos = 0.5;//blob.centroid.z;
            area = 1.5*blob.area/(zoom*8*zoom*52);

            m.addIntArg ( i );
            m.addFloatArg( xPos );
            m.addFloatArg( yPos );
            m.addFloatArg( area );
            m.addFloatArg( area );
            cout << filename <<" centroide: x=" << xPos << " y=" << yPos << " z=" << zPos << " area=" << area << endl;
            sender.sendMessage( m );
            break;
        }
    }

}
void testApp::blobOff( int x, int y, int id, int order ) {
    cout << "blobOff() - id:" << id << " order:" << order << endl;
    for(int i=0;i<10;i++)
    {
        if(FingersBlobs[i][1]==id)
        {

            float xPos,yPos,zPos,area;

            const char base[] = "/removeBlob";
            char filename [7];
            sprintf(filename, "%s %d", base, i);
            cout << filename << endl;


            ofxOscMessage m;

            m.setAddress("/removeBlob");

            xPos = 0;
            yPos = 0;
            zPos = 0;
            area = 0;

            m.addIntArg( i );
//            m.addFloatArg( xPos );
//            m.addFloatArg( yPos );
//            m.addFloatArg( zPos );
//            m.addFloatArg( area );

            sender.sendMessage( m );

            FingersBlobs[i][0]=0;
            FingersBlobs[i][1]=0;
            break;
        }
    }
}

