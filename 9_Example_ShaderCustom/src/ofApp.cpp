#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// Customization of the app window.

	// App window
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.
#if 0
	ofxSurfing::setOfxGuiTheme(); // Customize ofxGui theme.
	// Auto customizes settings to 60fps and vSync off.
	//ofxSurfing::setWindowAtMonitor(1); // Move to right display and set landscape.
	ofxSurfing::setWindowAtTV(1);
	//ofxSurfing::setWindowMaxFrameRate(); // Unlock framerate to max.
#endif

	ofSetLogLevel(OF_LOG_VERBOSE);

	//--

	// Pass the local camera
	//pbr.setup(camera);
	pbr.setup();

	// Pass the scene renderer to be processed
	callback_t f = std::bind(&ofApp::renderScene, this);
	pbr.setFunctionRenderScene(f);

	//--

#if (SURFING_ENABLE_MATERIAL == 1)
	material.setup();
#endif

#if (SURFING_ENABLE_MESH == 1)
	mesh.setup();
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	// We can access the internal camera pointer if required:
	// Here, we will draw the xyz axis when bDebug is enabled
	// and inside the camera view. Also we will have some benefits,
	// like could be saving camera state, or control mouse edit and more.
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

#if (SURFING_ENABLE_MATERIAL == 1)
	material.drawGui();
#endif

#if (SURFING_ENABLE_MESH == 1)
	mesh.drawGui();
#endif
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
	///* Put your scene drawing here! */
	//ofDrawSphere(50);

	// We can easy draw an internal
	// testing scene with three prims:
	pbr.drawTestScene();

	//--

#if (SURFING_ENABLE_MATERIAL == 1)
	material.draw();
#endif

#if (SURFING_ENABLE_MESH == 1)
	mesh.draw();
#endif
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit(); // Only required to save camera on exit
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
#if (SURFING_ENABLE_MATERIAL == 1)
	material.keyPressed(key);
#endif
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
#if (SURFING_ENABLE_MESH == 1)
	mesh.keyReleased(key);
#endif
}