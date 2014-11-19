#include "KinectSensors.h"

class IRProc : public KinectSensors {
	private:
		void thresholdDifference();
	public:
		IRProc();
		void update(ofxKFW2::Device *);
};