#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "DepthProc.h"
#include "IRProc.h"
#include "ofxOpenCv.h"
#include "ofxTuio.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp {
	public:
		~ofApp();
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void updateDepth();
		void updateIR();
		void drawDepthIR();

		/*
		void tuioAdded(ofxTuioCursor & tuioCursor);
		void tuioRemoved(ofxTuioCursor & tuioCursor);
		void tuioUpdated(ofxTuioCursor & tuioCursor);
		*/

		ofxKFW2::Device *kinect;
		DepthProc *depth;
		IRProc *IR;

		//ofxTuioClient tuioClient;
};
