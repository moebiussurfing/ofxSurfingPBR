#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window

	//ofxSurfing::setWindowAtMonitor(); // Center monitor landscape 
	//ofxSurfing::setWindowAtMonitor(-1); // Move to left monitor landscape 
	ofxSurfing::setWindowAtMonitor(1,true); // Move to right monitor portrait 

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

	// Check if it's the first time opening the App
	if (0)
		if (!pbr.getSettingsFileFound()) {
			ofLogWarning() << "ofxSurfingPBR Settings file not found!";

			// Force startup settings
			pbr.bDrawBgAlt = true;
			pbr.bgAltColor = ofFloatColor::orange;
			pbr.planeGlobalColor = ofFloatColor::red;
			pbr.material.globalColor = ofFloatColor::yellow;
		}
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

	if (key == 'q') ofxSurfing::setWindowSquared(1080);
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();
}