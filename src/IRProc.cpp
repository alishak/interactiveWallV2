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
	if (kinect->getInfrared()->getPixels() != NULL) {
		grayScale.setFromPixels(kinect->getInfrared()->getPixelsRef());
		orig_shorts = kinect->getInfrared()->getPixels(); //update depth array

		//Original calibration on start
		if (first) {
			memcpy(orig_shorts_diff, kinect->getInfrared()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
			first = false;
			cout << "first!\n";
		}

		//Recalibrate by pressing spacebar
		if (bLearnBackground == true) {
			memcpy(orig_shorts_diff, kinect->getInfrared()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
			ofLog(OF_LOG_NOTICE, "Pixels Captured!");
			bLearnBackground = false;
		}

		int max = 0;
		//Calculate the difference for each pixel and threshold the values
		for (int i = 0; i < kWidth * kHeight; i++) {
			int valtemp = orig_shorts[i] - orig_shorts_diff[i];

			diffPixels[i] = (unsigned char)ofMap(valtemp, 500, 4000, 0, 255, true);
			//Debugging info
			/*
			if (i < kWidth * kHeight / 1000) {
			cout << "orig_shorts_diff[i]: " << (int)orig_shorts_diff[i] << "\n";
			cout << "orig_shorts[i]: " << (int)orig_shorts[i] << "\n";
			}
			if (valtemp != 0)
			cout << "valtemp: " << valtemp << "\n";
			*/

			int scaled_diff = ofMap(valtemp, 500, 4000, 0, 1000, true);
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

		//Retreive difference Image
		CV_diff.setFromPixels(diffPixels);

		//Retreive threshold image
		CV_calc.setFromPixels(normalPixels);
		grayImage.warpIntoMe(CV_calc, src_cam_warp, dest_cam_warp);

		//Blur to reduce noise & find blobs of white pixels
		grayImage.blur(blur_amt);
		contours.findContours(grayImage, blob_min_area, blob_max_area, blob_max_blobs, true, false);

		for (int i = 0; i < contours.blobs.size(); i++) {
			centroid[i] = contours.blobs[i];
		}
	}
}
