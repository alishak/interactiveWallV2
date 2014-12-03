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