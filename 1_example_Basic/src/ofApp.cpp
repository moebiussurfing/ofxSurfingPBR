#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef OF_APP_DEFINED_ofxSurfingHelpers
	ofxSurfingHelpers::setWindowTitleAsProjectName();
	ofxSurfingHelpers::setMonitorsLayout(-1, false, true);
#endif

	//--

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
		drawMyScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawMyScene() {

	ofPushMatrix();
	ofTranslate(-200, 100, 0);
	ofRotateXDeg(180);
	ofDrawCone(0,0,0, 65, 100);
	ofPopMatrix();

	ofPushMatrix();
	static float spd = 240;
	ofRotateYDeg(360.f * (ofGetFrameNum() % (int)spd) / spd);
	ofDrawBox(0, 100, 0, 100);
	ofPopMatrix();

	ofDrawSphere(200, 100, 0, 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();
}