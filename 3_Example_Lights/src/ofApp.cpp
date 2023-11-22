#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
#if 1
	ofxSurfing::setWindowAtMonitor(-1);
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
#if 0
	// we can insert objectS into the camera view:
	camera.begin();
	pbr.beginMaterial();
	ofDrawBox(0, 500, 0, 100);
	pbr.endMaterial();
	camera.end();
#endif

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
	pbr.drawTestScene();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	pbr.keyPressed(key);
}