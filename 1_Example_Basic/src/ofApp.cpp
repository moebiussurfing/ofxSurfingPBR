#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

#if 1
	// App window
	{
		ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.

		// Move and shape the window app.
		// Customize settings: 60fps and vSync off.
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

	// Plane floor (Material A / simple)
	// don't need to begin/end material manually.
	pbr.drawPlane();

	// For other objects (Material B / full parameters)
	pbr.beginMaterial();
	{
		drawYourScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawYourScene() {
	// Draws an internal bundled testing scene with the three prims
	pbr.drawTestScene();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	pbr.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit(); // Only required to save camera on exit
}