#include "KinectSensors.h"

class IRProc : public KinectSensors {
	public:
		IRProc();
		void update(ofxKFW2::Device *);
};