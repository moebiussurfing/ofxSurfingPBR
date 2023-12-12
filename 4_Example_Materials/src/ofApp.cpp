#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

#if 1
	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
	ofxSurfing::setWindowAtMonitor(-1);
#endif

	//--

	// create and queue materials
	for (size_t i = 0; i < OFAPP_NUM_ITEMS; i++) {
		std::unique_ptr<SurfingMaterial> m;
		m = std::make_unique<SurfingMaterial>();
		string s = "MATERIAL_" + ofToString(i);
		m->setup(s);
		materials.push_back(std::move(m));
	}

	// create and queue materials
	for (size_t i = 0; i < OFAPP_NUM_ITEMS; i++) {
		std::unique_ptr<SurfingMeshSphereDisplaced> o;
		o = std::make_unique<SurfingMeshSphereDisplaced>();
		string s = ofToString(i);
		o->setup(s);
		objects.push_back(std::move(o));
	}

	refreshGuiAnchors();

	//--

	// gui
	g.setName("ofApp");
	for (size_t i = 0; i < OFAPP_NUM_ITEMS; i++) {
		ofParameterGroup g_ { "OBJECT " + ofToString(i) };
		g_.add(objects[i]->bDraw);
		g_.add(materials[i]->bGui);
		g_.add(objects[i]->bGui);
		g.add(g_);
	}
	gui.setup(g);
	ofxSurfing::load(g);

	//--

#ifdef OF_APP__USE__LIGHTS
	setupLights();
#endif

	if (1) // set to 0 to reset camera settings
		ofxLoadCamera(camera, pathCamera);
}

//--------------------------------------------------------------
void ofApp::update() {

#ifdef OF_APP__USE__LIGHTS
	updateLights();
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofEnableDepthTest();
	drawScene();

	//--

	ofDisableDepthTest();
	drawGui();
	gui.draw();
}

#ifdef OF_APP__USE__LIGHTS
//--------------------------------------------------------------
void ofApp::buildHelp() {

	bool b1 = pointLights[0]->getIsEnabled();
	bool b2 = pointLights[1]->getIsEnabled();
	bool b3 = pointLights[2]->getIsEnabled();
	bool b4 = pointLights[3]->getIsEnabled();
	bool b5 = pointLights[4]->getIsEnabled();

	string s = "";
	s += "KEYS\n\n";
	s += "HELP\n";
	s += "H " + string(bHelp ? "ON" : "OFF") + "\n\n";
	s += "DEBUG LIGHTS\n";
	s += "L " + string(bEnableLights ? "ON" : "OFF") + "\n\n";
	s += "ENABLE LIGHT\n";
	s += "1 " + string(b1 ? "ON" : "OFF") + "\n";
	s += "2 " + string(b2 ? "ON" : "OFF") + "\n";
	s += "3 " + string(b3 ? "ON" : "OFF") + "\n";
	s += "4 " + string(b4 ? "ON" : "OFF") + "\n";
	s += "5 " + string(b5 ? "ON" : "OFF");

	sHelp = s;
}
#endif

//--------------------------------------------------------------
void ofApp::drawGui() {
	refreshGuiLinks();

	// draw gui panels

	for (size_t i = 0; i < materials.size(); i++) {
		materials[i]->drawGui();
	}

	for (size_t i = 0; i < objects.size(); i++) {
		objects[i]->drawGui();
	}

#ifdef OF_APP__USE__LIGHTS
	if (bHelp) {
		buildHelp();
		ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT);
	}
#endif
}

//--------------------------------------------------------------
void ofApp::refreshGuiAnchors() {
	if (materials.size() == 0 || objects.size() == 0) return;

	// layout gui panels
	ofxSurfing::SURFING_LAYOUT l;

	l = ofxSurfing::SURFING_LAYOUT_TOP_CENTER;
	ofxSurfing::setGuiPositionToLayoutPanelsCentered(materials[0]->gui, OFAPP_NUM_ITEMS, l);

	l = ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER;
	ofxSurfing::setGuiPositionToLayoutPanelsCentered(objects[0]->gui, OFAPP_NUM_ITEMS, l);
}

//--------------------------------------------------------------
void ofApp::refreshGuiLinks() {
	if (materials.size() == 0 || objects.size() == 0) return;

	// materials on the top
	for (size_t i = 1; i < materials.size(); i++) {
		ofxSurfing::setGuiPositionRightTo(materials[i]->gui, materials[i - 1]->gui);
	}

	// object settings at the bottom
	for (size_t i = 1; i < objects.size(); i++) {
		ofxSurfing::setGuiPositionRightTo(objects[i]->gui, objects[i - 1]->gui);
	}
}

//--------------------------------------------------------------
void ofApp::drawScene() {

#ifdef OF_APP__USE__LIGHTS
	if (bEnableLights) ofEnableLighting();
#endif

	camera.begin();
	{
		renderScene();

		//--

#ifdef OF_APP__USE__LIGHTS
		if (bEnableLights) {

			ofDisableLighting();

			ofPushStyle();
			for (int i = 0; i < (int)pointLights.size(); i++) {
				ofSetColor(pointLights[i]->getDiffuseColor());
				if (!pointLights[i]->getIsEnabled()) {
					ofSetColor(40);
				}
				pointLights[i]->draw();
			}
			ofPopStyle();
		}
#endif
	}
	camera.end();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	ofPushMatrix();

	// hardcoded offset layout for 4 items
	float r = 2.5f;
	float d = SURFING__OBJ_SIZE_MAG * r;
	ofTranslate(-d * 1.5f, 0, 0);

	for (size_t i = 0; i < objects.size(); i++) {
		if (i > 0) ofTranslate(d, 0, 0);

		materials[i]->begin();
		{
			objects[i]->draw();
		}
		materials[i]->end();
	}

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	refreshGuiAnchors();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
#ifdef OF_APP__USE__LIGHTS
	if (key == 'h' || key == 'H') {
		bHelp = !bHelp;
		return;
	}
	if (key == 'l' || key == 'L') {
		bEnableLights = !bEnableLights;
		return;
	}

	int lindex = -1;
	if (key == '1') {
		lindex = 0;
	} else if (key == '2') {
		lindex = 1;
	} else if (key == '3') {
		lindex = 2;
	} else if (key == '4') {
		lindex = 3;
	} else if (key == '5') {
		lindex = 4;
	}
	if (lindex > -1 && lindex < (int)pointLights.size()) {
		if (pointLights[lindex]->getIsEnabled()) {
			pointLights[lindex]->disable();
		} else {
			pointLights[lindex]->enable();
		}
	}
#endif
}

#ifdef OF_APP__USE__LIGHTS
//--------------------------------------------------------------
void ofApp::setupLights() {
	ofSetSmoothLighting(true);

	float xdist = 600.0;
	float ty = 300;
	float tz = 300;

	for (int i = 0; i < OFAPP_NUM_LIGHTS; i++) {
		auto pointLight = make_shared<ofLight>();
		pointLight->setup();
		pointLight->enable();
		pointLight->setPointLight();
		pointLight->setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		pointLight->setAmbientColor(ofFloatColor(0.2f));
		pointLight->setSpecularColor(ofColor(255.f, 255.f, 255.f));
		float tx = ofMap(i, 0, OFAPP_NUM_LIGHTS - 1, -xdist * 0.5, xdist * 0.5f);
		pointLight->setPosition(tx, ty, tz);
		pointLight->setAttenuation(1.0, 0.00001, 0.00001);
		pointLights.push_back(pointLight);
	}

	colorHue = ofRandom(0, 250);

	lightColor.setBrightness(180.f);
	lightColor.setSaturation(150.f);
}

//--------------------------------------------------------------
void ofApp::updateLights() {
	float h = 150;

	float deltaTime = ofClamp(ofGetLastFrameTime(), 1.f / 5000.f, 1.f / 5.f);
	angle += deltaTime;

	colorHue += 1.f;
	if (colorHue >= 255) {
		colorHue = 0.f;
	}
	lightColor.setHue(colorHue);

	for (int i = 0; i < (int)pointLights.size(); i++) {
		auto & l = pointLights[i];
		lightColor.setHue(ofWrap(colorHue + (float)i * 12.5, 0, 255.f));
		l->setDiffuseColor(lightColor);
		float positionY = sin(angle + (float)i * cos(angle * 0.05) * 1.9f) * h;
		l->setPosition(l->getX(), positionY, 200.0);
	}
}
#endif

//--------------------------------------------------------------
void ofApp::exit() {
	ofxSurfing::save(g);

	ofxSaveCamera(camera, pathCamera);
}