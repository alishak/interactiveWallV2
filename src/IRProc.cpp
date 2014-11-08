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

bool IRProc::sampling() {
	return (kinect->getInfrared()->getPixels() != NULL);
}

void IRProc::updatePixels(ofxKFW2::Device *kinect) {
	grayScale.setFromPixels(kinect->getInfrared()->getPixelsRef());
	orig_shorts = kinect->getInfrared()->getPixels(); //update depth array
}

void IRProx::firstReference(ofxKFW2::Device *kinect) {
	//Original calibration on start
	memcpy(orig_shorts_diff, kinect->getInfrared()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
	first = false;
	cout << "first!\n";
}

void IRProc::recalibrate() {
	memcpy(orig_shorts_diff, kinect->getInfrared()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
	ofLog(OF_LOG_NOTICE, "Pixels Captured!");
	bLearnBackground = false;
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

void IRProc::retreiveAndBlur() {
	//Retreive difference Image
	CV_diff.setFromPixels(diffPixels);

	//Retreive threshold image
	CV_calc.setFromPixels(normalPixels);
	grayImage.warpIntoMe(CV_calc, src_cam_warp, dest_cam_warp);

	//Blur to reduce noise & find blobs of white pixels
	grayImage.blur(blur_amt);
}

void IRProc::findContours() {
	contours.findContours(grayImage, blob_min_area, blob_max_area, blob_max_blobs, true, false);

	for (int i = 0; i < contours.blobs.size(); i++) {
		centroid[i] = contours.blobs[i];
	}
}