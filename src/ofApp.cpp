#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	kinect = new ofxKFW2::Device;
	kinect->open();
	kinect->initDepth();

	kWidth = 512;// kinect->getDepth()->getPixelsRef().getWidth();
	kHeight = 424;// kinect->getDepth()->getPixelsRef().getHeight();

	src_cam_warp[0] = ofPoint(0, 0);
	src_cam_warp[1] = ofPoint(kWidth, 0);
	src_cam_warp[2] = ofPoint(kWidth, kHeight);
	src_cam_warp[3] = ofPoint(0, kHeight);

	dest_cam_warp[0] = ofPoint(0, 0);
	dest_cam_warp[1] = ofPoint(kWidth, 0);
	dest_cam_warp[2] = ofPoint(kWidth, kHeight);
	dest_cam_warp[3] = ofPoint(0, kHeight);

	normalPixels.allocate(kWidth, kHeight, OF_PIXELS_MONO);

	grayScale.allocate(kWidth, kHeight);
	grayImage.allocate(kWidth, kHeight);
	CV_calc.allocate(kWidth, kHeight);
	CV_diff.allocate(kWidth, kHeight);

	//Allocated onto the heap for copying
	orig_shorts_diff = new unsigned short[kWidth * kHeight];

	thresh_low = 60;
	thresh_high = 150;
	blur_amt = 1;
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect->update();
	grayScale.setFromPixels(kinect->getDepth()->getPixelsRef());

	if (kinect->getDepth()->getPixels() != NULL) {
		orig_shorts = kinect->getDepth()->getPixels();
		
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

		CV_calc.setFromPixels(normalPixels);
		grayImage.warpIntoMe(CV_calc, src_cam_warp, dest_cam_warp);

		grayImage.blur(blur_amt);
		contours.findContours(grayImage, 50, 700, 2, true, true);

		for (int i = 0; i < contours.blobs.size(); i++) {
			centroid[i] = contours.blobs[i];
		}
	}
	grayScale.adaptiveThreshold(3001);
}

//--------------------------------------------------------------
void ofApp::draw(){
	//kinect->getDepth()->draw(0, 0, ofGetWidth() / 2, ofGetHeight()); // note that the depth texture is RAW so may appear dark
	grayScale.draw(0, 0, ofGetWidth() / 2, ofGetHeight());
	CV_calc.draw(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	grayImage.draw(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);

	ofPushStyle();
	ofSetHexColor(0xFF0000);//Red
	ofNoFill();
	for (int i = 0; i < contours.blobs.size(); i++) {
		//ofRect(centroid[i].centroid.x, centroid[i].centroid.y, centroid[i].boundingRect.width, centroid[i].boundingRect.height);
		//centroid[i].draw(500 + 10*i, 500+10*i);
		ofRect((ofGetWidth() / 2) + ofMap(centroid[i].boundingRect.x, 0, grayImage.width, 0, ofGetWidth() / 2),
			(ofGetHeight() / 2) + ofMap(centroid[i].boundingRect.y, 0, grayImage.height, 0, ofGetHeight() / 2),
			ofMap(centroid[i].boundingRect.width, 0, grayImage.width, 0, ofGetWidth() / 2),
			ofMap(centroid[i].boundingRect.height, 0, grayImage.height, 0, ofGetHeight() / 2));
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
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
