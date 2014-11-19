#include "KinectSensors.h"

class DepthProc : public KinectSensors {
	private:
		void thresholdDifference();
		
	public:
		DepthProc();
		void update(ofxKFW2::Device *);
};