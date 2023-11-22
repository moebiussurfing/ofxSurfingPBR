#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofLogNotice(__FUNCTION__);

	ofBackground(155);

#if 1
	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
	ofxSurfing::setWindowAtMonitor(-1);
	ofSetLogLevel(OF_LOG_VERBOSE);
#endif

	//--

	//// Load the first model
	//index = 0;
	//loadModel(index);

	// Simple original
	path_Objects = "models\\original\\nike1.fbx";
	bool b = modelOriginal.loadModel(path_Objects, false);
	modelOriginal.setRotation(0, 180, 1, 0, 0);
	modelOriginal.setScale(1., 1., 1.);
	modelOriginal.setScaleNormalization(false);
	modelOriginal.enableColors();

	loadModelOriginal();

	//--

	// Camera

	cameraOrbit = 0;
	cam.setupPerspective();
	cam.reset();
	cam.setVFlip(false);
	cam.setDistance(SURFING__SCENE_SIZE_UNIT);

	if (bMouseCam)
		cam.enableMouseInput();
	else
		cam.disableMouseInput();

	bHelpText = true;

	//--

	setupParams();

	gui.setup(parameters);

	//--

	loadModelFiles();

	//--

	startup();
}

//--------------------------------------------------------------
void ofApp::setupParams() {
	ofLogNotice(__FUNCTION__);

	params_Panels.add(bLights);
	params_Panels.add(bParts);
	params_Panels.add(bMaterials);
	params_Scene.add(params_Panels);

	params_Objects.add(bShoeParts);
	params_Objects.add(bSimpleOriginal);
	params_Objects.add(bTestBox);
	params_Scene.add(params_Objects);

	params_SceneExtra.add(scenePBR.surfingLights.bAnimLights);
	params_SceneExtra.add(scenePBR.surfingLights.bAnimLightsMouse);
	params_SceneExtra.add(bInCam);

	params_SimpleOriginal.add(colorOriginal);
	params_SimpleOriginal.add(shininessOriginal);

	params_SceneExtra.add(params_SimpleOriginal);
	params_Scene.add(params_SceneExtra);

	params_Camera.add(bRotate);
	params_Camera.add(speedRotate);
	params_Camera.add(bMouseCam);
	params_Camera.add(bResetCam);
	params_Camera.add(bGrid);

	parameters.add(params_Camera);
	parameters.add(params_Scene);

	ofAddListener(parameters.parameterChangedE(), this, &ofApp::ChangedParameters);
}

//--------------------------------------------------------------
void ofApp::startup() {
	ofLogNotice(__FUNCTION__);

	load();

	//--

	scenePBR.clearColors();
	scenePBR.clearMaterials();

	for (int i = 0; i < palette.size(); i++) {
		scenePBR.addColor(palette[i].get());
	}

	scenePBR.setup();
}

//--------------------------------------------------------------
void ofApp::loadModelFiles() {
	ofLogNotice(__FUNCTION__);

	dir.allowExt("fbx");
	dir.open("models\\nike");
	dir.listDir();

	if (dir.size() == 0)
		ofLogError(__FUNCTION__) << "Files folder not found or empty of .fbx files!";
	else
		ofLogNotice(__FUNCTION__) << "Found " << dir.size() << " files in the folder.";

	models.clear();
	meshParts.clear();

	palette.clear();
	params_Palette.clear();

	scenePBR.setup();

	for (auto & file : dir.getFiles()) {
		ofLogNotice(__FUNCTION__) << file.getFileName();
		string _path = file.getAbsolutePath();

		std::unique_ptr<ofxAssimpModelLoader> m = std::make_unique<ofxAssimpModelLoader>();
		bool b = m->loadModel(_path, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);
		m->setRotation(0, 180, 1, 0, 0);
		m->setScale(1.f, 1.f, 1.f);
		m->setScaleNormalization(false);
		m->enableColors();
		models.push_back(std::move(m));

		//--

		// Queue one material per model
		scenePBR.addMaterial(file.getBaseName());

		// Palette colors
		string _name = "Color" + ofToString(models.size() - 1);
		ofParameter<ofFloatColor> _col { _name, ofFloatColor(0, 0, 0, 0), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) };
		palette.emplace_back(_col);
		params_Palette.add(_col);

		scenePBR.addColor(_col);
	}

	ofAddListener(params_Palette.parameterChangedE(), this, &ofApp::ChangedPalette);

	//--

	// Create parts meshes. like refreshModels

	int count = 0;
	for (auto & _model : models) {
		vector<ofMesh> _meshes;

		for (int i = 0; i < _model->getMeshCount(); i++) {
			auto & m = _model->getMesh(i);

			for (int j = 0; j < m.getNumColors(); j++) {
				m.removeColor(i);
			}
			for (int j = 0; j < m.getVertices().size(); j++) {
				m.addColor(palette[count].get());
			}

			_meshes.push_back(m);
		}
		meshParts.push_back(_meshes);

		count++;
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	// Camera
	if (bRotate) {
		float r = ofMap(speedRotate, 0, 1, 5.0, 360., true);
		cameraOrbit += ofGetLastFrameTime() * r; // r degrees per second;
		cam.orbitDeg(cameraOrbit, 0., cam.getDistance(), { 0., 0., 0. });
	}
}

//--

//--------------------------------------------------------------
void ofApp::draw() {

	drawScene();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawScene() {
	if (models.size() == 0) return;

	ofEnableDepthTest();

	beginLights();
	{
		beginCam();
		{
			drawObjects();
		}
		endCam();
	}
	endLights();
}

//--------------------------------------------------------------
void ofApp::drawObjects() {

	if (bShoeParts) {
		for (int i = 0; i < models.size(); i++) {

			// begin material
			scenePBR.beginMaterial(i);
			{
#if 1
				// meshes
				for (auto & _m : meshParts[i]) {
					_m.drawFaces();
				}
#else
				// models
				models[i].drawFaces(); // do not works the material..
#endif
			}
			// end material
			scenePBR.endMaterial(i);
		}
	}

	if (bSimpleOriginal) {
		ofPushStyle();
		ofSetColor(colorOriginal.get());
		ofFill();
		materialOriginal.begin();
#if 1
		// meshes
		for (int i = 0; i < meshesOriginal.size(); i++) {
			meshesOriginal[i].drawFaces();
		}
#else
		// model
		modelOriginal.drawFaces();
#endif

		materialOriginal.end();
		ofPopStyle();
	}

	if (bTestBox) {
		materialOriginal.begin();

		if (bRotate) {
			ofRotateXDeg(ofNoise(-1.f * ofGetElapsedTimeMillis(), -0.5f));
			ofRotateYDeg(ofNoise(1.f * ofGetElapsedTimeMillis()));
			ofRotateZDeg(ofNoise(0.01f * ofGetElapsedTimeMillis(), -1.f));
		}
		ofBox(0, 0, 0, 2);

		materialOriginal.end();
	}
}

//--

//--------------------------------------------------------------
void ofApp::drawGui() {
	ofDisableDepthTest();

	gui.draw();
}

//--

//--------------------------------------------------------------
void ofApp::doRandomPalette() {
	ofLogNotice(__FUNCTION__);

	for (int i = 0; i < palette.size(); i++) {
		int hue = ofRandom(255);
		ofColor c_;
		c_.setHsb(hue, 255, 255);
		ofFloatColor c = c_;

		palette[i].set(c);
		scenePBR.addColor(c);
	}
}

//--

//--------------------------------------------------------------
void ofApp::beginCam() {
	cam.begin();
	{
		ofPushMatrix();
		ofScale(70.f);

		// Grid
		if (bGrid) {
			//ofPushMatrix();
			//ofTranslate(0, -3.5, 0);
			//ofDrawAxis(1.f);
			//ofDrawGrid(5.f, 1, false, false, true, false);
			//ofPopMatrix();

			// Floor grid with one single rectangle
			int sz = 10;
			ofPushStyle();
			ofPushMatrix();
			ofTranslate(0, 3, 0);
			ofSetColor(0, 255, 0, 255);
			ofNoFill();
			ofSetLineWidth(1.f);
			ofRotateXDeg(90);
			ofTranslate(-sz / 2, -sz / 2, 0.7 * sz);
			ofDrawRectangle(0, 0, sz, sz);
			ofPopMatrix();
			ofPopStyle();
		}
	}
}

//--------------------------------------------------------------
void ofApp::endCam() {
	{
		ofPopMatrix();
	}
	cam.end();
}

//--

//--------------------------------------------------------------
void ofApp::beginLights() {
	scenePBR.beginLights();
}

//--------------------------------------------------------------
void ofApp::endLights() {
	scenePBR.endLights();
}

//--

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	ofLogNotice(__FUNCTION__) << key;

	if (key == OF_KEY_BACKSPACE) {
		doRandomPalette();
		//colorOriginal = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
	}

	// original
	else if (key == OF_KEY_RETURN) {
		path_Objects = "models\\original\\nike1.fbx";
		loadModelOriginal();
	}

	//// next
	//else if (key == ' ') {
	//	index++;
	//	index = index % dir.size();
	//	loadModel(index);
	//	path_Objects = dir.getFile(index).getAbsolutePath();
	//	curFileInfo = path_Objects;
	//}

	else if (key == 'c' || key == 'm' || key == OF_KEY_SHIFT) {
		bMouseCam = !bMouseCam;
		if (bMouseCam)
			cam.enableMouseInput();
		else
			cam.disableMouseInput();
	}

	else if (key == 'o')
		bSimpleOriginal = !bSimpleOriginal;
	else if (key == 'r')
		bRotate = !bRotate;
	else if (key == 'p')
		bShoeParts = !bShoeParts;

	//else if (key == OF_KEY_F1) {
	//	bShoeParts = !bShoeParts;
	//	bSimpleOriginal = !bShoeParts;
	//}

	//else if (key == ' ') {
	//	loadModelOriginal();
	//}
	//else if (key == '0') {
	//	path_Objects = "original/nike1.fbx";
	//	loadModelOriginal();
	//}
	//else if (key == '2') {
	//	path_Objects = "nike_01_sole.fbx";
	//	loadModelOriginal();
	//}
	//else if (key == '3') {
	//	path_Objects = "nike_09_Cords.fbx";
	//	loadModelOriginal();
	//}
	//else if (key == '4') {
	//	path_Objects = "nike_06_latSup.fbx";
	//	loadModelOriginal();
	//}
}

//--------------------------------------------------------------
void ofApp::loadModelOriginal() {
	ofLogNotice(__FUNCTION__);

	model.loadModel(path_Objects);
	model.setRotation(0, 180, 1, 0, 0);
	model.setScale(1.f, 1.f, 1.f);
	model.setScaleNormalization(false);
	model.enableColors();

	meshesOriginal.clear();

	for (int i = 0; i < model.getMeshCount(); i++) {

		auto & m = model.getMesh(i);

		for (int j = 0; j < m.getNumColors(); j++) {
			m.removeColor(i);
		}
		for (int j = 0; j < m.getVertices().size(); j++) {
			m.addColor(colorOriginal.get());
		}

		meshesOriginal.push_back(m);
	}

	// model info
	curFileInfo = path_Objects;
}

//--------------------------------------------------------------
void ofApp::doRefreshColorsOriginal() {
	ofLogNotice(__FUNCTION__);

	for (int i = 0; i < modelOriginal.getMeshCount(); i++) {
		auto & m = meshesOriginal[i];

		for (int j = 0; j < m.getVertices().size(); j++) {
			m.setColor(i, colorOriginal.get());
		}
	}
}

//--------------------------------------------------------------
void ofApp::ChangedPalette(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice() << "ChangedPalette:" << name << ": " << e;

	for (int i = 0; i < palette.size(); i++) {
		string _name = "Color" + ofToString(i);
		if (name == _name) {
			scenePBR.setColor(palette[i].get(), i);
		}
	}
}

//--------------------------------------------------------------
void ofApp::ChangedParameters(ofAbstractParameter & e) {

	string name = e.getName();

	if (name != mx.getName() && name != my.getName())
		ofLogNotice() << "ChangedParameters:" << name << ": " << e;

	//--

	if (name == bMouseCam.getName()) {
		if (bMouseCam)
			cam.enableMouseInput();
		else
			cam.disableMouseInput();
	}

	else if (name == bResetCam.getName() && bResetCam) {
		bResetCam = false;

		cam.setupPerspective();
		cam.reset();
		cam.setVFlip(false);
		cam.setDistance(SURFING__SCENE_SIZE_UNIT);
		//TODO:
	}

	//--

	// Pick elements to render, Which scene type..

	else if (name == bShoeParts.getName()) {
		if (bShoeParts) {
			bSimpleOriginal = bTestBox = false;
		}
	}

	else if (name == bSimpleOriginal.getName()) {
		if (bSimpleOriginal) {
			bShoeParts = bTestBox = false;
		}
	}

	else if (name == bTestBox.getName()) {
		if (bTestBox) {
			bShoeParts = bSimpleOriginal = false;
		}
	}

	else if (name == colorOriginal.getName()) {
		materialOriginal.setAmbientColor(colorOriginal.get());
		materialOriginal.setDiffuseColor(colorOriginal.get());
		doRefreshColorsOriginal();
	}

	else if (name == shininessOriginal.getName()) {
		materialOriginal.setShininess(shininessOriginal.get());
		//loadModelOriginal();
		doRefreshColorsOriginal();
	}
}

//--------------------------------------------------------------
void ofApp::save() {
	ofLogNotice(__FUNCTION__);

	ofxSurfing::saveGroup(params_Palette);
	ofxSurfing::saveGroup(parameters);

	ofxSaveCamera(cam, path_CamSettings);
}

//--------------------------------------------------------------
void ofApp::load() {
	ofLogNotice(__FUNCTION__);

	ofxSurfing::loadGroup(parameters);
	ofxSurfing::loadGroup(params_Palette);

	ofxLoadCamera(cam, path_CamSettings);
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofLogNotice(__FUNCTION__);

	ofRemoveListener(parameters.parameterChangedE(), this, &ofApp::ChangedParameters);
	ofRemoveListener(params_Palette.parameterChangedE(), this, &ofApp::ChangedPalette);

	save();
}

////--------------------------------------------------------------
//void ofApp::loadModel(int pos) {
//	model = models[index];
//
//	meshesOriginal.clear();
//	for (int i = 0; i < model.getMeshCount(); i++) {
//
//		auto m = model.getMesh(i);
//
//		for (int j = 0; j < m.getNumColors(); j++) {
//			m.removeColor(i);
//		}
//		for (int j = 0; j < m.getVertices().size(); j++) {
//			m.addColor(colorOriginal.get());
//		}
//
//		meshesOriginal.push_back(m);
//	}
//
//	// model info
//	curFileInfo = path_Objects;
//}

////--------------------------------------------------------------
//void ofApp::doRefreshPalette() {
//	ofLogNotice(__FUNCTION__);
//
//	//return;
//
//	if (palette2.size() == 0 || palette.size() == 0) return;
//
//	//-
//
//	for (int i = 0; i < palette.size(); i++) {
//		int i2;
//		if (palette.size() > palette2.size()) {
//			i2 = i % palette2.size();
//			palette[i].set(palette2[i2]);
//		} else {
//			palette[i].set(palette2[i]);
//		}
//
//		scenePBR.setColor(palette[i], i);
//	}
//}