#include "KinectSensors.h"

class DepthProc : KinectSensors {
	private:
		void thresholdDifference();
		
	public:
		DepthProc();
		void update(ofxKFW2::Device *);
};