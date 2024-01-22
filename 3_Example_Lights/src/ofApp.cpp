#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// Surfing material
	material.setup();

	//--

	// Surfing lights combo
	lights.setup();

	// Pass render functions required to do the shadow passes!
	callback_t f = std::bind(&ofApp::renderScene, this);
	lights.setFunctionRenderScene(f);

	//--

	setupGui();

	//--

#if 1
	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
	ofxSurfing::setWindowAtMonitor(-1);
#endif

	//--

	// Startup settings
	ofxSurfing::load(g);
	if (!ofxLoadCamera(camera, pathCamera)) {
		doResetCamera();
	}
	camera.setControlArea({ 0, 0, 1920, 1080 }); //small fix
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	bAutoLayout.set("AutoLayout", true);

	vResetCamera.set("Reset Camera");
	listenerResetCamera = vResetCamera.newListener([this](void) {
		doResetCamera();
	});

	g.setName("ofApp");
	g.add(lights.bGui);
	g.add(material.bGui);
	g.add(bAutoLayout);
	g.add(vResetCamera);
	gui.setup(g);

	refreshGui(true);
}

//--------------------------------------------------------------
void ofApp::refreshGui(bool bHard) {
	if (bHard) ofxSurfing::setGuiPositionToLayout(gui, ofxSurfing::SURFING_LAYOUT_TOP_LEFT);
	ofxSurfing::setGuiPositionRightTo(lights.gui, gui);
	ofxSurfing::setGuiPositionBelowTo(material.gui, gui, true);
}

//--------------------------------------------------------------
void ofApp::draw() {

	drawScene();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawScene() {

	ofEnableDepthTest();

	lights.updateLights();

	camera.begin();
	{
		lights.begin();
		{
			renderScene();
		}
		lights.end();

		lights.drawDebugLights();
	}
	camera.end();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	material.begin();
	{
		ofPushStyle();
		ofSetConeResolution(50, 10, 2);
		ofSetSphereResolution(50);

		// floor thin box
		float szf = SURFING__PBR__SCENE_SIZE_UNIT * 2.0f;
		ofPushMatrix();
		ofRotateXDeg(90);
		ofDrawBox(glm::vec3(0, 0, 0), szf, szf, 1);
		ofPopMatrix();

		//--

		// A three prims simple test scene
		float sz = 300;

		// Cone
		ofPushMatrix();
		ofTranslate(-2 * sz, sz, 0);
		ofRotateXDeg(180);
		ofDrawCone(0, 0, 0, sz * 0.65f, sz);
		ofPopMatrix();

		// Box
		ofPushMatrix();
		float spd = 240;
		ofRotateYDeg(360.f * (ofGetFrameNum() % (int)spd) / spd);
		ofDrawBox(0, sz, 0, sz);
		ofPopMatrix();

		// Sphere
		ofDrawSphere(2 * sz, sz, 0, sz / 2.f);

		ofPopStyle();
	}
	material.end();
}

//--------------------------------------------------------------
void ofApp::doResetCamera() {

	camera.setControlArea(ofGetCurrentViewport()); //small fix for ofCamerSaveLoad..
	camera.reset();
	camera.setPosition({ 10, 500, 2000 });
	camera.setOrientation({ 0, 0, 0 });

	// Save
	ofxSaveCamera(camera, pathCamera);
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	ofDisableDepthTest();

	if (bAutoLayout) refreshGui();

	gui.draw();

	lights.drawGui();
	material.drawGui();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	refreshGui();
}
//--------------------------------------------------------------
void ofApp::exit() {
	ofxSurfing::save(g);
	ofxSaveCamera(camera, pathCamera);

	//material.exit();//not required
	//lights.exit();//not required
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	lights.keyPressed(key);
}