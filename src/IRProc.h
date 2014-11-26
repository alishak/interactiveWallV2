#include "KinectSensors.h"

class IRProc : public KinectSensors {
	private:
		void thresholdDifference();
		bool sampling(ofxKFW2::Device *);
		void updatePixels(ofxKFW2:Device *);
		void firstReference(ofxKFW2::Device *);
		void recalibrate(ofxKFW2::Device *);
	public:
		IRProc();
		void update(ofxKFW2::Device *);
};