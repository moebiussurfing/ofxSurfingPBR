#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef OF_APP_DEFINED_ofxSurfingHelpers
	ofxSurfingHelpers::setWindowTitleAsProjectName();
	ofxSurfingHelpers::setMonitorsLayout(-1, false, true);
#endif

	//--

	pbr.setup();
	pbr.setCameraPtr(&camera);// Pass the local camera

	// Pass the scene to render
	callback_t myFunctionDraw = std::bind(&ofApp::renderScene, this);
	pbr.setFunction_renderScene(myFunctionDraw);

	// Set log level
	ofSetLogLevel("ofxSurfingPBR", OF_LOG_VERBOSE);
	//ofSetLogLevel("ofxSurfingPBR", OF_LOG_ERROR);
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	pbr.drawGui();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// plane/floor
	pbr.drawPlane();

	// for other objects
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

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();
}