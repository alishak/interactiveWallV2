#include "ofApp.h"

//--------------------------------------------------------------
ofApp::~ofApp() {
	//delete references on exit to prevent any memory leaks
	delete[] orig_shorts_diff;
	*orig_shorts = NULL;
	*orig_shorts = NULL;
}

//--------------------------------------------------------------
void ofApp::setup(){
	//Instantiate the kinect reference
	kinect = new ofxKFW2::Device;
	kinect->open();
	kinect->initDepth();

	//Hardcoded values after sampling form the kinect
	kWidth = 512;// kinect->getDepth()->getPixelsRef().getWidth();
	kHeight = 424;// kinect->getDepth()->getPixelsRef().getHeight();

	//Source and destination points for warpIntoMe() openCV function
	src_cam_warp[0] = ofPoint(0, 0);
	src_cam_warp[1] = ofPoint(kWidth, 0);
	src_cam_warp[2] = ofPoint(kWidth, kHeight);
	src_cam_warp[3] = ofPoint(0, kHeight);

	dest_cam_warp[0] = ofPoint(0, 0);
	dest_cam_warp[1] = ofPoint(kWidth, 0);
	dest_cam_warp[2] = ofPoint(kWidth, kHeight);
	dest_cam_warp[3] = ofPoint(0, kHeight);

	//Allocating ofPixels and ofxCVGrayscaleImage instances
	normalPixels.allocate(kWidth, kHeight, OF_PIXELS_MONO);
	//diffPixels.allocate(kWidth, kHeight, OF_PIXELS_MONO); //not used for now

	grayScale.allocate(kWidth, kHeight);
	grayImage.allocate(kWidth, kHeight);
	CV_calc.allocate(kWidth, kHeight);
	CV_diff.allocate(kWidth, kHeight);

	//Allocated onto the heap for copying
	orig_shorts_diff = new unsigned short[kWidth * kHeight];

	//Modifiable attributes
	thresh_low = 60;
	thresh_high = 150;
	blur_amt = 1;
	blob_min_area = 50;
	blob_max_area = 700;
	blob_max_blobs = 2;
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect->update();
	grayScale.setFromPixels(kinect->getDepth()->getPixelsRef());

	//Wait for kinect to start sampling
	if (kinect->getDepth()->getPixels() != NULL) {
		orig_shorts = kinect->getDepth()->getPixels(); //update depth array
		
		//Original calibration on start
		if (first) {
			memcpy(orig_shorts_diff, kinect->getDepth()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
			first = false;
			cout << "first!\n";
		}

		//Recalibrate by pressing spacebar
		if (bLearnBackground == true) {
			memcpy(orig_shorts_diff, kinect->getDepth()->getPixels(), kWidth * kHeight * sizeof(unsigned short));
			ofLog(OF_LOG_NOTICE, "Pixels Captured!");
			bLearnBackground = false;
		}

		//Calculate the difference for each pixel and threshold the values
		for (int i = 0; i < kWidth * kHeight; i++) {
			int valtemp = orig_shorts[i] - orig_shorts_diff[i];

			//Debugging info
			/* 
			if (i < kWidth * kHeight / 1000) {
				cout << "orig_shorts_diff[i]: " << (int)orig_shorts_diff[i] << "\n";
				cout << "orig_shorts[i]: " << (int)orig_shorts[i] << "\n";
			}
			if (valtemp != 0)
				cout << "valtemp: " << valtemp << "\n";
			*/
			if (abs(valtemp) < thresh_low || abs(valtemp) > thresh_high){
				valtemp = 0;
			} else {
				valtemp = 1000;
			}

			//save into an ofPixels instance
			//NOTE: Auto-scaled down to 255 Grayscal values
			normalPixels[i] = (unsigned char)valtemp;
		}

		//Retreive threshold image
		CV_calc.setFromPixels(normalPixels);
		grayImage.warpIntoMe(CV_calc, src_cam_warp, dest_cam_warp);

		//Blur to reduce noise & find blobs of white pixels
		grayImage.blur(blur_amt);
		contours.findContours(grayImage, blob_min_area, blob_max_area, blob_max_blobs, true, true);

		for (int i = 0; i < contours.blobs.size(); i++) {
			centroid[i] = contours.blobs[i];
		}
	}

	//threshold to be able to see Raw depth image
	grayScale.adaptiveThreshold(3001);
}

//--------------------------------------------------------------
void ofApp::draw(){
	//kinect->getDepth()->draw(0, 0, ofGetWidth() / 2, ofGetHeight()); // note that the depth texture is RAW so may appear dark
	grayScale.draw(0, 0, ofGetWidth() / 2, ofGetHeight());
	CV_calc.draw(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	grayImage.draw(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);

	ofPushStyle();
	ofSetLineWidth(3);
	ofSetHexColor(0xFF0000);//Red
	ofNoFill();

	//Draw all available blobs
	for (int i = 0; i < contours.blobs.size(); i++) {
		
		ofRect((ofGetWidth() / 2) + ofMap(centroid[i].boundingRect.x, 0, grayImage.width, 0, ofGetWidth() / 2),
			(ofGetHeight() / 2) + ofMap(centroid[i].boundingRect.y, 0, grayImage.height, 0, ofGetHeight() / 2),
			ofMap(centroid[i].boundingRect.width, 0, grayImage.width, 0, ofGetWidth() / 2),
			ofMap(centroid[i].boundingRect.height, 0, grayImage.height, 0, ofGetHeight() / 2));
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	if (key == 'f') {
		ofToggleFullscreen();
	}

	//32 is the ascii decimal value for the space character
	if (key == 32) {
		bLearnBackground = true;
	}

	if (key == '+') {
		thresh_high += 5;
		cout << "ThesholdHigh = " << thresh_high << "\n";
	}

	if (key == '_') {
		thresh_high -= 5;
		cout << "ThesholdHigh = " << thresh_high << "\n";
	}

	if (key == '=') {
		thresh_low += 5;
		cout << "thresh_low = " << thresh_low << "\n";
	}

	if (key == '-') {
		thresh_low -= 5;
		cout << "thresh_low = " << thresh_low << "\n";
	}

	if (key == '>') {
		blur_amt += 2;
		cout << "blur_amt = " << blur_amt << "\n";
	}

	if (key == '<') {
		if (blur_amt != 1)
			blur_amt -= 2;
		cout << "blue_amt = " << blur_amt << "\n";
	}

	if (key == ']') {
		if (blob_min_area + 50 <= blob_max_area)
			blob_min_area += 50;
		cout << "blob_min_area = " << blob_min_area << "\n";
		
	}

	if (key == '[') {
		if (blob_min_area - 50 >= 0) 
			blob_min_area -= 50;
		cout << "blob_min_area = " << blob_min_area << "\n";
	}

	if (key == '}') {
		blob_max_area += 50;
		cout << "blob_max_area = " << blob_max_area << "\n";
	}

	if (key == '{') {
		if (blob_max_area - 50 >= blob_min_area) 
			blob_max_area -= 50;
		cout << "blob_max_area = " << blob_max_area << "\n";
	}

	//If a number from 0-9 is pressed
	if (key >= 48 && key <= 57) {
		blob_max_blobs = (int)key - 48;
		cout << "blob_max_blobs = " << blob_max_blobs << "\n";
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
