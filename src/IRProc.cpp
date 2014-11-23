#include "IRProc.h"

IRProc::IRProc() {
	//Modifiable attributes
	thresh_low = 650;
	thresh_high = 2000;

	//blur_amt = 1;
	blob_min_area = 40;
	blob_max_area = 200;
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
		
		//Recalibrate by pressing spacebar
		if (bLearnBackground == true) {
			recalibrate(kinect);
		}

		thresholdDifference();

		retreiveAndBlur();

		findContours();

		//sendTouch();
	}
	
	if (calibrating) {
		if (blobTracker.size() == 1) {
			if (blobTracker[0].id == lastID) {
				
				if (inCrosshair == false) {
					startTime = ofGetElapsedTimeMillis();
					inCrosshair = true;
				}
				else {
					elapsedTime = ofGetElapsedTimeMillis() - startTime;
					color = GREEN;
					if (elapsedTime > 2000) {
						temp_dest[corner - 1] = ofPoint(blobTracker[0].centroid.x * kWidth, blobTracker[0].centroid.y * kHeight);
						corner = corner + 1 == 5 ? 5 : (corner % 4) + 1;
						inCrosshair = false;
						elapsedTime = 0;
					}

				}
				
			}
			else {
				inCrosshair = false;
				color = RED;
				elapsedTime = 0;
			}

			lastID = blobTracker[0].id;
		}
		else { color = RED; }

		if (corner == 5) {
			calibrating = false;
			memcpy(src_cam_warp, temp_dest, 4 * sizeof(ofPoint));
		}
	
	}
}

void IRProc::thresholdDifference() {
	//Calculate the difference for each pixel and threshold the values
	for (int i = 0; i < kWidth * kHeight; i++) {
		int valtemp = orig_shorts[i] - orig_shorts_diff[i];
		
		diffPixels[i] = (unsigned char)(valtemp <= 400 ? 0 : ofMap(valtemp, 401, 1000, 0, 255, true));
		
		//int scaled_diff = ofMap(valtemp, 500, 4000, 0, 1000, true);
		if (valtemp > thresh_low && valtemp < thresh_high) {
			valtemp = 1000;
		}
		else {
			valtemp = 0;
		}
		//save into an ofPixels instance
		//NOTE: Auto-scaled down to 255 Grayscale values
		normalPixels[i] = (unsigned char)valtemp;
	}
}