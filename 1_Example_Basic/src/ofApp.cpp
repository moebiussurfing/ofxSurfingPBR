#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window

#if 0
	//ofxSurfing::setWindowAtMonitor(); // Center monitor / landscape.
	ofxSurfing::setWindowAtMonitor(-1); // Move to left monitor / landscape.
	//ofxSurfing::setWindowAtMonitor(1,true); // Move to right monitor / portrait.
#endif

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