#include "IRProc.h"

IRProc::IRProc() {
	//Modifiable attributes
	thresh_low = 250;
	thresh_high = 1000;

	//blur_amt = 1;
	blob_min_area = 60;
	blob_max_area = 2000;
	blob_max_blobs = 2;
}

void IRProc::update(ofxKFW2::Device *kinect) {
	//Wait for kinect to start sampling
	if (sampling(kinect)) {

		updatePixels(kinect);

		//Get the first pixels reference
		if (first) {
			firstReference(kinect);
		}
		
		//Record reference image again by pressing spacebar
		if (bLearnBackground == true) {
			recalibrate(kinect);
		}

		thresholdDifference();

		retrieveAndBlur();

		findContours();

		sendTouch();
	}
	
	if (calibrating) {
		calibrate();
	}
}

void IRProc::thresholdDifference() {
	//Calculate the difference for each pixel and threshold the values
	for (int i = 0; i < kWidth * kHeight; i++) {
		int valtemp = orig_shorts[i] - orig_shorts_diff[i];
		
		diffPixels[i] = (unsigned char)abs(valtemp);// (valtemp <= 400 ? 0 : ofMap(valtemp, 401, 1000, 0, 255, true));
		
		
		//int scaled_diff = ofMap(valtemp, 500, 4000, 0, 1000, true);
		if (valtemp > thresh_low && valtemp < thresh_high) {
			valtemp = 1000;
		}
		else {
			valtemp = 0;
		}
		/*
		int diffval = valtemp;// diffPixels[i];
		if (diffval > thresh_low) {// && diffval < thresh_high) {
			diffval = 0;
		}
		else {
			diffval = 1000;
		}*/

		/*
		if ( (i % kWidth > kHeight / 2 - kWidth / 7 && i % kWidth < kHeight / 2 + kWidth / 7)
			&& (i % kHeight > kWidth / 2 - kHeight / 5 && i % kHeight < kWidth / 2 + kHeight / 5 )) {
			valtemp = 1000;
		}*/
		//save into an ofPixels instance
		//NOTE: Auto-scaled down to 255 Grayscale values
		normalPixels[i] = (unsigned char)valtemp; //diffval;
	}
}

bool IRProc::sampling(ofxKFW2::Device *kinect) {
	return (kinect->getInfrared()->getPixels() != NULL);
}

void IRProc::updatePixels(ofxKFW2::Device *kinect) {
	grayScale.setFromPixels(kinect->getInfrared()->getPixelsRef());
	orig_shorts = kinect->getInfrared()->getPixels(); //update depth array
}

void IRProc::firstReference(ofxKFW2::Device *kinect) {
	//Original calibration on start
	memcpy(orig_shorts_diff, kinect->getInfrared()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
	first = false;
	cout << "first!\n";
}

void IRProc::recalibrate(ofxKFW2::Device *kinect) {
	memcpy(orig_shorts_diff, kinect->getInfrared()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
	ofLog(OF_LOG_NOTICE, "Pixels Captured!");
	bLearnBackground = false;
}