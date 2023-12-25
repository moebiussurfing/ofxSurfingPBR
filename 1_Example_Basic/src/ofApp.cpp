#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

#if 1
	ofxSurfing::setOfxGuiTheme(); // Customize ofxGui theme.

	// App window
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.
	// Auto customizes settings to 60fps and vSync off.
	ofxSurfing::setWindowAtMonitor(-1); // Move to left display and set landscape.
	//ofxSurfing::setWindowMaxFrameRate(); // Unlock framerate to max.
#endif

	//--

	// Pass the local camera
	pbr.setup(camera);

	// Pass the scene renderer to be processed
	callback_t f = std::bind(&ofApp::renderScene, this);
	pbr.setFunctionRenderScene(f);
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	pbr.drawGui();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// The camera is auto handled internally.

	// Floor
	// plane or box modes.
	// draw without requiring to use begin/end material manually, 
	pbr.drawFloor();

	// Material B
	// for other objects
	pbr.beginMaterial();
	{
		drawScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawScene() {
	/* Put your scene drawing here! */

	// we can easy draw an internal
	// testing scene with three prims:
	pbr.drawTestScene();
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit(); // Only required to save camera on exit
}