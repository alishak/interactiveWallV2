#include "IRProc.h"

IRProc::IRProc() {
	//Modifiable attributes
	thresh_low = 650;
	thresh_high = 2000;

	blur_amt = 3;
	blob_min_area = 100;
	blob_max_area = 300;
	blob_max_blobs = 3;
}

void IRProc::update(ofxKFW2::Device *kinect) {
	//Wait for kinect to start sampling
	if (sampling()) {

		updatePixels(kinect);

		//Get the first pixels reference
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
	}
}

void IRProc::thresholdDifference() {
	//Calculate the difference for each pixel and threshold the values
	for (int i = 0; i < kWidth * kHeight; i++) {
		int valtemp = orig_shorts[i] - orig_shorts_diff[i];

		diffPixels[i] = (unsigned char)valtemp;// (unsigned char)ofMap(valtemp, 0, 65536, 0, 255, true);

		//int scaled_diff = ofMap(valtemp, 500, 4000, 0, 1000, true);
		if (valtemp > thresh_low && valtemp < thresh_high){
			valtemp = 0;
		}
		else {
			valtemp = 1000;
		}
		//save into an ofPixels instance
		//NOTE: Auto-scaled down to 255 Grayscale values
		normalPixels[i] = (unsigned char)valtemp;
	}
}