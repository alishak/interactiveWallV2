/*
InteractiveWallV2, Multi Touch on Flexible Wall Displays with Kinect V2
Copyright (C) 2014  Ali ElShakankiry

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
