#include "KinectSensors.h"

class DepthProc : KinectSensors {
	public:
		DepthProc();
		void update(ofxKFW2::Device *);
};