#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window

#if 1
	ofxSurfing::setWindowAtMonitor(); // Center monitor / landscape.
	//ofxSurfing::setWindowAtMonitor(-1); // Move to left monitor / landscape.
	//ofxSurfing::setWindowAtMonitor(1,true); // Move to right monitor / portrait.
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

		//TODO:
		// Check if it's the first time opening the App
#if 0
	if (!pbr.getSettingsFileFound()) {
		ofLogWarning() << "ofxSurfingPBR Settings file not found!";

		// must fix crashing on callback..
		// Force startup settings
		pbr.bDrawBgAlt.set(true);
		pbr.bgAltColor.setWithoutEventNotifications(ofFloatColor::orange);
		pbr.planeGlobalColor.setWithoutEventNotifications(ofFloatColor::red);
		pbr.material.globalColor.setWithoutEventNotifications(ofFloatColor::yellow);
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