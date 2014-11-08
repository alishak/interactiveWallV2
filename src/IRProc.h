#include "KinectSensors.h"

class IRProc : public KinectSensors {
	private:
		bool sampling();
		void updatePixels(ofxKFW2::Device *kinect);
		void firstReference(ofxKFW2::Device *kinect);
		void recalibrate();
		void thresholdDifference();
		void retreiveAndBlur();
		void findContours();

	public:
		IRProc();
		void update(ofxKFW2::Device *);
};