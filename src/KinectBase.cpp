#include "KinectBase.h"
#define PORT 12345

KinectBase::KinectBase() {
	//Hardcoded values after sampling from the kinect
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
	diffPixels.allocate(kWidth, kHeight, OF_PIXELS_MONO); //not used for now

	grayScale.allocate(kWidth, kHeight);
	grayImage.allocate(kWidth, kHeight);
	CV_calc.allocate(kWidth, kHeight);
	CV_diff.allocate(kWidth, kHeight);

	//Allocated onto the heap for copying
	orig_shorts_diff = new unsigned short[kWidth * kHeight];

	sender.setup("localhost", PORT);

	corner = 1;
	red.loadImage("red.png");
	green.loadImage("green.png");
	cross_x = 0;
	cross_y = 0;
}

KinectBase::~KinectBase() {
	//delete references on exit to prevent any memory leaks
	delete[] orig_shorts_diff;
	orig_shorts_diff = NULL;
	orig_shorts = NULL;
}

void KinectBase::draw() {
	/*
	grayScale.draw(0, 0, ofGetWidth() / 2, ofGetHeight());
	CV_diff.draw(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	grayImage.draw(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);
	drawBlobs(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);
	*/
	
	//grayScale.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	grayImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	drawBlobs(0, 0, ofGetWidth(), ofGetHeight());
	
	if(calibrating) drawCrosshair();
	
}

void KinectBase::keyPressed(int key) {
	switch (key) {
		
	//32 is the ascii decimal value for the space character
	case 32:	
		bLearnBackground = true; break;

	case '+':		
		thresh_high += 10;
		cout << "ThesholdHigh = " << thresh_high << "\n"; break;

	case '_':
		if (thresh_high - 10 >= thresh_low)
			thresh_high -= 10;
		cout << "ThesholdHigh = " << thresh_high << "\n"; break;

	case '=':
		if (thresh_low + 10 <= thresh_high)
			thresh_low += 10;
		cout << "ThesholdLow = " << thresh_low << "\n"; break;

	case '-':
		thresh_low -= 10;
		cout << "ThesholdLow = " << thresh_low << "\n"; break;

	case '>':
		//blur_amt += 2;
		//cout << "blur_amt = " << blur_amt << "\n"; break;

	case '<':
		//if (blur_amt != 1)
			//blur_amt -= 2;
		//cout << "blue_amt = " << blur_amt << "\n"; break;

	case ']':
		if (blob_min_area + 20 <= blob_max_area)
			blob_min_area += 20;
		cout << "blob_IR_min_area = " << blob_min_area << "\n"; break;

	case '[':
		if (blob_min_area - 20 >= 0)
			blob_min_area -= 20;
		cout << "blob_min_area = " << blob_min_area << "\n"; break;

	case '}':
		blob_max_area += 20;
		cout << "blob_max_area = " << blob_max_area << "\n"; break;

	case '{':
		if (blob_max_area - 20 >= blob_min_area)
			blob_max_area -= 20;
		cout << "blob_max_area = " << blob_max_area << "\n"; break;
	}

	//If a number from 0-9 is pressed
	if (key >= 48 && key <= 57) {
		blob_max_blobs = (int)key - 48;
		cout << "blob_max_blobs = " << blob_max_blobs << "\n";
	}
}

void KinectBase::drawBlobs(float origin_x, float origin_y, float size_x, float size_y) {
	/*
	ofPushStyle();
	ofSetLineWidth(3);
	ofSetHexColor(0xFF0000);//Red
	ofNoFill();

	//Draw all available blobs
	
	for (int i = 0; i < contours.blobs.size(); i++) {
		ofxCvBlob centroid = contours.blobs[i];
		ofRect(origin_x + ofMap(centroid.boundingRect.x, 0, grayImage.width, 0, size_x),
			origin_y + ofMap(centroid.boundingRect.y, 0, grayImage.height, 0, size_y),
			ofMap(centroid.boundingRect.width, 0, grayImage.width, 0, size_x),
			ofMap(centroid.boundingRect.height, 0, grayImage.height, 0, size_y));
	}
	ofPopStyle();*/

	blobTracker.draw(origin_x, origin_y, size_x, size_y);
}

void::KinectBase::drawCrosshair() {
	int length = ofGetWidth() / 8;
	int x, y;

	switch (corner) {

	case 1: cross_y = 0;
		x = cross_x - length / 2 + 5;
		y = cross_y - length / 2 + 5;
		break;
	case 2: cross_x = ofGetWidth();
		x = cross_x - length / 2 - 5;
		y = cross_y - length / 2 + 5;
		break;
	case 3: cross_y = ofGetHeight();
		x = cross_x - length / 2 + 5;
		y = cross_y - length / 2 - 5;
		break;
	case 4: cross_x = 0;
		x = cross_x - length / 2 + 5;
		y = cross_y - length / 2 - 5;
		break;
	}

	if (color == RED) {
		red.draw(x, y, length, length);
	}
	else if(color == GREEN) {
		green.draw(x, y, length, length);
	}
}

void KinectBase::retrieveAndBlur() {
	//Retreive difference Image
	CV_diff.setFromPixels(diffPixels);

	//Retreive threshold image
	CV_calc.setFromPixels(normalPixels);
	grayImage.warpIntoMe(CV_calc, src_cam_warp, dest_cam_warp);

	//Blur to reduce noise & find blobs of white pixels
	grayImage.erode();	
	//grayImage.dilate();
}

void KinectBase::findContours() {
	//contours.findContours(grayImage, blob_min_area, blob_max_area, blob_max_blobs, true, false);
	blobTracker.update(grayImage, -1, blob_min_area, blob_max_area, blob_max_blobs);
}

void KinectBase::sendTouch() {
	/*
	if (last_size == 0) {
		message_rate = 75;
	}
	else {
		message_rate = 50;
	}*/

	//send every 50 ms to not overload the touch reciever
	if (ofGetElapsedTimeMillis() - lastMessage > message_rate) {
		ofxOscBundle bundle;
		ofxOscMessage alive;
		ofxOscMessage fseq;

		alive.setAddress("/tuio/2Dcur");
		alive.addStringArg("alive");

		for (int i = 0; i < blobTracker.size(); i++) {
			alive.addIntArg(blobTracker[i].id);
		}
		
		sender.sendMessage(alive);
		//bundle.addMessage(alive);

		for (int i = 0; i < blobTracker.size(); i++) {
			//cout << "x: " << contours.blobs[i].centroid.x << "		y: " << contours.blobs[i].centroid.y << "\n";

			ofxOscMessage set;
			set.setAddress("/tuio/2Dcur");
			set.addStringArg("set");
			set.addIntArg(blobTracker[i].id); //s_id
			set.addFloatArg(blobTracker[i].centroid.x); //xpos
			set.addFloatArg(blobTracker[i].centroid.y); //ypos
			set.addFloatArg(blobTracker[i].D.x); //xspeed
			set.addFloatArg(blobTracker[i].D.y); //yspeed
			set.addFloatArg(blobTracker[i].maccel); //maccel

			sender.sendMessage(set);
			//bundle.addMessage(set);
		}

		fseq.setAddress("/tuio/2Dcur");
		fseq.addStringArg("fseq");
		fseq.addIntArg(frame);

		sender.sendMessage(fseq);

		//bundle.addMessage(fseq);

		//sender.sendBundle(bundle);
		lastMessage = ofGetElapsedTimeMillis();
		last_size = blobTracker.size();
		frame += 1;
	}
}

void KinectBase::calibrate() {
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