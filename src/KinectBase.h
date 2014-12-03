#pragma once 

#include "ofxKinectForWindows2.h"
#include "ofxOpenCv.h"
#include "ofxBlobTracker.h"
#include "ofxOsc.h"

/* This is the base class for all custom objects created to retrieve
 * kinect sensor data and process them.
 */
class KinectBase {
    private:
		void drawBlobs(float, float, float, float);
		void drawCrosshair();
		//void injectTouch(int, int);
		
		int frame = 1;
	public:
		KinectBase();
		~KinectBase();

		void draw();
		void keyPressed(int);

	protected:
		void retrieveAndBlur();
		void findContours();
		void sendTouch();
		void calibrate();
			
		int kWidth;
		int kHeight;
		bool bLearnBackground = false;
		bool first = true;

		int thresh_low;
		int thresh_high;

		//int blur_amt;
		int blob_min_area;
		int blob_max_area;
		int blob_max_blobs;

		ofPoint					dest_cam_warp[4];
		ofPoint					src_cam_warp[4];
		ofPoint					temp_dest[4];

		ofPixels				normalPixels;
		ofPixels				diffPixels;

		unsigned short *		orig_shorts; //reference
		unsigned short *		orig_shorts_diff; //deep copy

		ofxCvGrayscaleImage		grayScale;
		ofxCvGrayscaleImage		grayImage;
		ofxCvGrayscaleImage		CV_calc;
		ofxCvGrayscaleImage		CV_diff;

		//ofxCvContourFinder		contours;
		//blob Tracking and touch sending
		ofxBlobTracker			blobTracker;
		ofxOscSender			sender;

		//Calibration variables
		ofImage					red;
		ofImage					green;
		int						cross_x;
		int						cross_y;
		int						corner;
		int						lastID = -1;
		unsigned long long		startTime;
		unsigned long long		elapsedTime;
		unsigned long long		lastMessage = 0;
		bool					calibrating = true;
		bool					inCrosshair = false;
		int						last_size = 0;
		int						message_rate = 50;
		enum COLOR { RED, GREEN };
		COLOR color = RED;
};