#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// Customizes the app window 
	// with some helpers
#if 1
	ofxSurfing::setOfxGuiTheme(); // Customize ofxGui theme.

	// App window
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.

	// Auto customizes layout and settings to 60fps and vSync off.
	//ofxSurfing::setWindowAtMonitor(-1); // Move to left display and set landscape.
	#if 1
		//ofxSurfing::setWindowAtMonitor(1); // Move to right display and set landscape.
		//ofSetWindowShape(768, 1360);
		ofxSurfing::setWindowAtTV(1);
	#endif
	//ofxSurfing::setWindowMaxFrameRate(); // Unlock framerate to max.
#endif

	//--

	ofSetLogLevel(OF_LOG_VERBOSE);

	// Pass the local camera
	//pbr.setup(camera);
	pbr.setup();

	// Pass the scene renderer to be processed
	callback_t f = std::bind(&ofApp::renderScene, this);
	pbr.setFunctionRenderScene(f);
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	// These draws will not been PBR rendered.
	// We can access the internal camera pointer if required:
	// we will draw the axis when bDebug is enabled and inside the camera view.
	if (pbr.bDebug) {
		pbr.getOfEasyCamPtr()->begin();
		{
			ofPushMatrix();
			ofTranslate(0, pbr.getFloorZeroHeight(), 0);
			ofDrawAxis(25);
			ofPopMatrix();
		}
		pbr.getOfEasyCamPtr()->end();
	}

	//--

	pbr.drawGui();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// The camera is auto handled internally.

	// Floor
	// Plane or box modes.
	// Draw without requiring to use begin/end material manually,
	pbr.drawFloor();

	// Material
	// One pre created material for other objects
	pbr.beginMaterial();
	{
		drawScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawScene() {
	/* Put your scene drawing here! */

	// We can easy draw an internal
	// testing scene with three prims:
	pbr.drawTestScene();
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit(); // Only required to save camera on exit
}