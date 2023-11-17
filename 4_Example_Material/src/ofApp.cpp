#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
	ofxSurfing::setWindowAtMonitor(-1);

	//--

	pbr.setup(camera);
	pbr.setFunctionRenderScene(std::bind(&ofApp::renderScene, this));

	sphereDisp.setup();
	sphereDisp.gui.add(pbr.testSceneParams);
	ofxSurfing::setGuiPositionToLayout(sphereDisp.gui, 2);

	material.setup("_MAT_1");
	ofxSurfing::setGuiPositionToLayout(material.gui, 5);
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofEnableDepthTest();

	pbr.draw();

	//--

	ofDisableDepthTest();

	sphereDisp.drawGui();
	material.drawGui();
	pbr.drawGui();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
#if 0
	pbr.drawPlane();

	pbr.beginMaterial();
	{
		pbr.drawTestScene();
	}
	pbr.endMaterial();
#endif

	material.begin();
	{
		drawYourScene();
	}
	material.end();
}

//--------------------------------------------------------------
void ofApp::drawYourScene() {

	pbr.pushTestSceneTRansform(); //use internal scene transforms
	{
		sphereDisp.draw();
	}
	pbr.popTestSceneTRansform();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	pbr.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();
}