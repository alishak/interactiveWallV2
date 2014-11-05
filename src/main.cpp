#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	// ofPtr<ofBaseRenderer> renderer(new ofGLProgrammableRenderer(true));
	ofSetupOpenGL(800,600,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
