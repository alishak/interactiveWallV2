#include "Sensors.h"

class DepthProc : Sensors {
	public:
		DepthProc();
		void update(ofxKFW2::Device *);
};