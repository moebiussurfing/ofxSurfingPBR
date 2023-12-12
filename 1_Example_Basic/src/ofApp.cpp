#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

#if 1
	// Theme
	ofxSurfing::setOfxGuiTheme(); // Customize ofxGui theme.

	// App window
	{
		ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.

		// Auto customizes settings to 60fps and vSync off.
		// Move and shape the window app.
		ofxSurfing::setWindowAtMonitor(-1); // Move to left display and set landscape.
		//ofxSurfing::setWindowAtMonitor(1, true); // Move to right display and set portrait.
		//ofxSurfing::setWindowAtMonitor(); // Stay at main display (center in my setup) and landscape.
	}
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
	// The camera is internal!

	// Floor 
	// could be selected plane or box modes.
	// Material A / simple parameters
	// don't need to begin/end material manually.
	pbr.drawFloor();

	// For other objects 
	// Material B / full parameters
	pbr.beginMaterial();
	{
		drawScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawScene() {
	/* Put your scene drawing here! */

	// Draws an internal bundled 
	// testing scene with the three prims:
	pbr.drawTestScene();
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit(); // Only required to save camera on exit
}