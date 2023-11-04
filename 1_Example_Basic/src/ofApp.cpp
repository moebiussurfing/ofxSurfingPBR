#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// App window
	{
		ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.
#if 1
		// Move and shape the window app.
		// Customize settings: 60fps and vSync off.
		//ofxSurfing::setWindowAtMonitor(); // Stay at main display (center in my setup) and landscape.
		ofxSurfing::setWindowAtMonitor(-1); // Move to left display and set landscape.
		//ofxSurfing::setWindowAtMonitor(1, true); // Move to right display and set portrait.
#endif
	}

	//--

	pbr.setup();

	// Pass the local camera
	pbr.setCameraPtr(&camera);

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
	// Plane floor
	pbr.drawPlane();

	// For other objects
	pbr.beginMaterial();
	{
		drawYourScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawYourScene() {
	pbr.drawTestScene(); //use an internal bundled testing scene
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	pbr.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();
}