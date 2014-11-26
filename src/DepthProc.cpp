#include "DepthProc.h"

DepthProc::DepthProc() {
	//Modifiable attributes
	thresh_low = 10;
	thresh_high = 80;

//	blur_amt = 3;
	blob_min_area = 150;
	blob_max_area = 1800;
	blob_max_blobs = 2;
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

		sendTouch();
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

void DepthProc::thresholdDifference() {
	/*
	int xmin = src_cam_warp[0].x;// < src_cam_warp[3].x ? src_cam_warp[0].x : src_cam_warp[3].x;
	int xmax = src_cam_warp[1].x;// > src_cam_warp[2].x ? src_cam_warp[1].x : src_cam_warp[2].x;

	int ymin = src_cam_warp[0].y;// < src_cam_warp[1].y ? src_cam_warp[0].y : src_cam_warp[1].y;
	int ymax = src_cam_warp[2].y;// > src_cam_warp[3].y ? src_cam_warp[2].y : src_cam_warp[3].y;
	*/
	//Calculate the difference for each pixel and threshold the values
		for (int i = 0; i < kWidth * kHeight; i++) {
			int valtemp = orig_shorts_diff[i] - orig_shorts[i];

			diffPixels[i] = (unsigned char)abs(valtemp);// (unsigned char)ofMap(valtemp, 0, 65536, 0, 255, true);
			//diffPixels[i] = (unsigned char)ofMap(valtemp, 0, 65536, 0, 255, true);

			if (valtemp > thresh_low && valtemp < thresh_high) {
				valtemp = 1000;
			}
			else {
				valtemp = 0;
			}

			/*
			if (i % kWidth < xmin || i % kWidth > xmax || i / kWidth < ymin || i / kWidth > ymax) {
				valtemp = 0;
			}*/
			//save into an ofPixels instance
			//NOTE: Auto-scaled down to 255 Grayscale values
			normalPixels[i] = (unsigned char)valtemp;
		}
}

bool DepthProc::sampling(ofxKFW2::Device *kinect) {
	return (kinect->getDepth()->getPixels() != NULL);
}

void DepthProc::updatePixels(ofxKFW2::Device *kinect) {
	grayScale.setFromPixels(kinect->getDepth()->getPixelsRef());
	orig_shorts = kinect->getDepth()->getPixels(); //update depth array
}

void DepthProc::firstReference(ofxKFW2::Device *kinect) {
	memcpy(orig_shorts_diff, kinect->getDepth()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
	first = false;
	cout << "first!\n";
}

void DepthProc::recalibrate(ofxKFW2::Device *kinect) {
	memcpy(orig_shorts_diff, kinect->getDepth()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
	ofLog(OF_LOG_NOTICE, "Pixels Captured!");
	bLearnBackground = false;
}