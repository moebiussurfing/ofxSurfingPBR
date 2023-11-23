#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	material.setup();

	lights.setup();

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

	ofxSurfing::load(g);

	if (!ofxLoadCamera(camera, pathCamera)) {
		doResetCamera();
	}
	camera.setControlArea({ 0, 0, 1920, 1080 }); //small fix
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	bRefreshGui.set("Refresh Gui", false);

	vResetCamera.set("Reset Camera");
	listenerResetCamera = vResetCamera.newListener([this](void) {
		doResetCamera();
	});

	g.setName("ofApp");
	g.add(lights.bGui);
	g.add(material.bGui);
	g.add(bRefreshGui);
	g.add(vResetCamera);
	gui.setup(g);

	refreshGui();
}

//--------------------------------------------------------------
void ofApp::refreshGui() {

	ofxSurfing::setGuiPositionToLayout(gui, ofxSurfing::SURFING_LAYOUT_TOP_LEFT);
	ofxSurfing::setGuiPositionRightTo(lights.gui, gui);
	ofxSurfing::setGuiPositionToLayout(material.gui, ofxSurfing::SURFING_LAYOUT_TOP_RIGHT);
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {

	drawScene();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	ofDisableDepthTest();

	if (bRefreshGui) refreshGui();

	gui.draw();
	lights.drawGui();
	material.drawGui();
}

//--------------------------------------------------------------
void ofApp::drawScene() {

	ofEnableDepthTest();

	lights.updateLights();
	lights.drawLights();

	camera.begin();
	{
		lights.begin();
		{
			renderScene();
		}
		lights.end();

		//--

#if 0
		// floor lines
		ofPushMatrix();
		ofTranslate(0, 1, 0);
		ofRotateXDeg(90);
		ofRotateYDeg(90);
		ofPushStyle();
		ofNoFill();
		ofSetColor(ofFloatColor(0, .5));
		ofDrawGridPlane(1000, 1, false);
		ofPopStyle();
		ofPopMatrix();
#endif

		//--

		//ofDisableDepthTest();

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
		ofPushMatrix();
		ofRotateXDeg(90);
		ofDrawBox(glm::vec3(0, 0, 0), 2000, 2000, 1);
		ofPopMatrix();

		//--

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

	camera.setControlArea(ofGetCurrentViewport()); //small fix

	//camera.setupPerspective(false);
	camera.reset();

	camera.setPosition({ 10, 500, 2000 });
	camera.setOrientation({ 0, 0, 0 });

	// save
	ofxSaveCamera(camera, pathCamera);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	refreshGui();
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofxSurfing::save(g);
	ofxSaveCamera(camera, pathCamera);

	material.exit();
	lights.exit();
}
