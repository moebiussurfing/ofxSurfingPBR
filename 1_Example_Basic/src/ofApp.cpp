#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef OF_APP_DEFINED_ofxSurfingHelpers
	ofxSurfingHelpers::setWindowTitleAsProjectName();
	ofxSurfingHelpers::setMonitorsLayout(-1, false, true);
#endif

	//--

	pbr.setup();

	// Pass the local camera
	pbr.setCameraPtr(&camera);

	// Pass the scene renderer to be processed
	callback_t myFunctionDraw = std::bind(&ofApp::renderScene, this);
	pbr.setFunction_renderScene(myFunctionDraw);

	//--

	// Optional

	// Set log level
	if (0) pbr.setLogLevel(OF_LOG_VERBOSE);

#if 0
	// Check if it's the first time opening the App
	bool b = pbr.getSettingsFileFound();
	if (!b) {
		ofLogWarning() << "ofxSurfingPBR Settings file not found!";

		// Force startup settings
		pbr.bDrawBgAlt = true;
		pbr.bgAltColor = ofFloatColor::orange;
		pbr.planeGlobalColor = ofFloatColor::orange;
		pbr.material.globalColor = ofFloatColor::blue;
	}
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	pbr.drawGui();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// plane floor
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