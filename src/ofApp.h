#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{
	private:
		int kWidth;
		int kHeight;
		bool bLearnBackground = false;
		bool first = true;
		int thresh_low;
		int thresh_high;
		int thresh_IR_low;
		int thresh_IR_high;

		int blur_amt;
		int blob_min_area;
		int blob_max_area;
		int blob_max_blobs;

		int blob_IR_min_area;
		int blob_IR_max_area;

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
		void drawDepth();
		
		ofxKFW2::Device *kinect;

		ofPoint					dest_cam_warp[4];
		ofPoint					src_cam_warp[4];
		ofxCvBlob					centroid[10];
		
		ofPixels				normalPixels;
		ofPixels				diffPixels;

		unsigned short *		orig_shorts; //reference
		unsigned short *		orig_shorts_diff; //deep copy

		ofxCvGrayscaleImage		grayScale;
		ofxCvGrayscaleImage		grayImage;
		ofxCvGrayscaleImage		CV_calc;
		ofxCvGrayscaleImage		CV_diff;
		ofxCvGrayscaleImage			IR_image;

		ofxCvContourFinder		contours;
};
