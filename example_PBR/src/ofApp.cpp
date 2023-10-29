#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef OF_APP_DEFINED_ofxSurfingHelpers
	ofxSurfingHelpers::setWindowTitleAsProjectName();
	ofxSurfingHelpers::setMonitorsLayout(0, false, true);
#endif

	pbr.setup();
	pbr.setCameraPtr(&camera);

	// render scene
	callback_t myFunctionDraw = std::bind(&ofApp::renderScene, this);
	pbr.setFunction_renderScene(myFunctionDraw);
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	pbr.drawGui();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// floor plane
	pbr.drawPlane();

	// objects
	pbr.beginMaterial();
	{
		ofDrawBox(120, 80, 0, 100);

		ofPushMatrix();
		static float spd = 240;
		ofRotateYDeg(359.f * (ofGetFrameNum() % (int)spd) / spd);
		ofDrawBox(0, 0, 0, 100);
		ofPopMatrix();

		ofDrawBox(-120, 80, 0, 100);
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();
}