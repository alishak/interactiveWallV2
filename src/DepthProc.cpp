#include "DepthProc.h"

DepthProc::DepthProc() {
	//Modifiable attributes
	thresh_low = 60;
	thresh_high = 150;

	blur_amt = 3;
	blob_min_area = 50;
	blob_max_area = 700;
	blob_max_blobs = 3;
}

void DepthProc::update(ofxKFW2::Device *kinect) {
	//Wait for kinect to start sampling
	if (sampling(kinect)) {
		
		updatePixels(kinect);

		//Original calibration on start
		if (first) {
			firstReference(kinect);
		}

		//Recalibrate by pressing spacebar
		if (bLearnBackground == true) {
			recalibrate(kinect);
		}

		thresholdDifference();

		retreiveAndBlur();

		findContours();

		//threshold to be able to see Raw depth image
		grayScale.adaptiveThreshold(3001);
	}
}

void DepthProc::thresholdDifference() {
	//Calculate the difference for each pixel and threshold the values
		for (int i = 0; i < kWidth * kHeight; i++) {
			int valtemp = orig_shorts[i] - orig_shorts_diff[i];

			diffPixels[i] = (unsigned char)valtemp;// (unsigned char)ofMap(valtemp, 0, 65536, 0, 255, true);
			
			if (abs(valtemp) < thresh_low || abs(valtemp) > thresh_high){
				valtemp = 0;
			}
			else {
				valtemp = 1000;
			}

			//save into an ofPixels instance
			//NOTE: Auto-scaled down to 255 Grayscal values
			normalPixels[i] = (unsigned char)valtemp;
		}
}