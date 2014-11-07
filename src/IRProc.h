#include "Sensors.h"

class IRProc : Sensors {
	public:
		IRProc();
		void update(ofxKFW2::Device *);
};