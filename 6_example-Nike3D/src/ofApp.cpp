#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofLogNotice(__FUNCTION__);

	ofBackground(128);

	//--

#if 1
	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
	ofxSurfing::setWindowAtMonitor(-1);
	ofSetLogLevel(OF_LOG_VERBOSE);
#endif

	//--

	// Camera
	{
		cameraOrbit = 0;
		cam.setupPerspective();
		cam.reset();
		cam.setVFlip(false);
		cam.setDistance(SURFING__SCENE_SIZE_UNIT);

		if (bMouseCam)
			cam.enableMouseInput();
		else
			cam.disableMouseInput();
	}

	//--

	setupParams();

	gui.setup(parameters);

	//--

	path_Model = "models\\original\\nike1.fbx";

	//// Load the first model
	//index = 0;
	//loadModel(index);

	// Simple
	loadModelSimple();

	// Original
	loadModelOriginal();

	// By parts
	loadModelParts();

	//----

	setupScene();

	//----

	startup();
}

//--------------------------------------------------------------
void ofApp::setupParams() {
	ofLogNotice(__FUNCTION__);

	//panelsParams.add(bLights);
	//panelsParams.add(bParts);
	//panelsParams.add(bMaterials);
	//sceneParams.add(panelsParams);

	objectsParams.add(bDrawShoeParts);
	objectsParams.add(bDrawOriginal);
	objectsParams.add(bDrawTestBox);
	sceneParams.add(objectsParams);

	//sceneExtraParams.add(sceneManager.surfingLights.bAnimLights);
	//sceneExtraParams.add(sceneManager.surfingLights.bAnimLightsMouse);

	originalParams.add(colorOriginal);
	originalParams.add(shininessOriginal);

	sceneExtraParams.add(originalParams);
	sceneParams.add(sceneExtraParams);

	cameraParams.add(bRotate);
	cameraParams.add(speedRotate);
	cameraParams.add(bMouseCam);
	cameraParams.add(vResetCam);
	cameraParams.add(bDrawGrid);

	parameters.add(cameraParams);
	parameters.add(sceneParams);

	ofAddListener(parameters.parameterChangedE(), this, &ofApp::ChangedParameters);
}

//--------------------------------------------------------------
void ofApp::startup() {
	ofLogNotice(__FUNCTION__);

	//--

	load();
}

//--------------------------------------------------------------
void ofApp::setupScene() {
	ofLogNotice(__FUNCTION__);

	//sceneManager.clearColors();
	//sceneManager.clearMaterials();

	//for (int i = 0; i < palette.size(); i++) {
	//	sceneManager.addColor(palette[i].get());
	//}

	sceneManager.setup();

	ofxSurfing::setGuiPositionRightTo(gui, sceneManager.gui);
}

//--------------------------------------------------------------
void ofApp::update() {

	// Camera
	if (bRotate) {
		float r = ofMap(speedRotate, 0, 1, 1.f, 100.f, true);
		cameraOrbit += ofGetLastFrameTime() * r; // r degrees per second;
		cam.orbitDeg(cameraOrbit, 0.f, cam.getDistance(), { 0.f, 0.f, 0.f });
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

	// Parts

	if (bDrawShoeParts) {
		for (size_t i = 0; i < models.size(); i++) {

			// Begin material
			sceneManager.beginMaterial(i);
			{
#if 1
				// Meshes
				for (auto & _m : meshesParts[i]) {
					_m.drawFaces();
				}
#else
				// Models
				models[i].drawFaces(); // do not works the material..
#endif
			}
			// End material
			sceneManager.endMaterial(i);
		}
	}

	//--

	// Original

	if (bDrawOriginal) {
		ofPushStyle();
		ofFill();
		ofSetColor(colorOriginal.get());

		materialOriginal.begin();
		{
#if 1
			// Meshes
			for (size_t i = 0; i < meshesModelOriginal.size(); i++) {
				meshesModelOriginal[i].drawFaces();
			}
#else
			// Model
			modelOriginal.drawFaces();
#endif
		}
		materialOriginal.end();

		ofPopStyle();
	}

	//--

	// Test box

	if (bDrawTestBox) {
		materialOriginal.begin();
		{
			if (bRotate) {
				ofRotateXDeg(ofNoise(-1.f * ofGetElapsedTimeMillis(), -0.5f));
				ofRotateYDeg(ofNoise(1.f * ofGetElapsedTimeMillis()));
				ofRotateZDeg(ofNoise(0.01f * ofGetElapsedTimeMillis(), -1.f));
			}
			ofBox(0, 0, 0, 2);
		}
		materialOriginal.end();
	}
}

//--

//--------------------------------------------------------------
void ofApp::drawGui() {
	ofDisableDepthTest();

	gui.draw();

	ofxSurfing::setGuiPositionRightTo(sceneManager.gui, gui);
	sceneManager.drawGui();
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
		sceneManager.addColor(c);
	}
}

//--

//--------------------------------------------------------------
void ofApp::drawGrid() {
	// Floor grid
	// with one single rectangle
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

//--------------------------------------------------------------
void ofApp::beginCam() {
	cam.begin();
	{
		ofPushMatrix();
		ofScale(70.f);
	}

	if (bDrawGrid) {
		drawGrid();
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
	sceneManager.beginLights();
}

//--------------------------------------------------------------
void ofApp::endLights() {
	sceneManager.endLights();
}

//--

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	ofLogNotice(__FUNCTION__) << key;

	if (key == OF_KEY_BACKSPACE) {
		doRandomPalette();
		//colorOriginal = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
	}

	//// original
	//else if (key == OF_KEY_RETURN) {
	//	path_Model = "models\\original\\nike1.fbx";
	//	loadModelOriginal();
	//}

	//// next
	//else if (key == ' ') {
	//	index++;
	//	index = index % dir.size();
	//	loadModel(index);
	//	path_Model = dir.getFile(index).getAbsolutePath();
	//	sNameModel = path_Model;
	//}

	else if (key == 'c' || key == 'm' || key == OF_KEY_SHIFT) {
		bMouseCam = !bMouseCam;
		if (bMouseCam)
			cam.enableMouseInput();
		else
			cam.disableMouseInput();
	}

	else if (key == 'o')
		bDrawOriginal = !bDrawOriginal;
	else if (key == 'r')
		bRotate = !bRotate;
	else if (key == 'p')
		bDrawShoeParts = !bDrawShoeParts;

	//else if (key == OF_KEY_F1) {
	//	bDrawShoeParts = !bDrawShoeParts;
	//	bDrawOriginal = !bDrawShoeParts;
	//}

	//else if (key == ' ') {
	//	loadModelOriginal();
	//}
	//else if (key == '0') {
	//	path_Model = "original/nike1.fbx";
	//	loadModelOriginal();
	//}
	//else if (key == '2') {
	//	path_Model = "nike_01_sole.fbx";
	//	loadModelOriginal();
	//}
	//else if (key == '3') {
	//	path_Model = "nike_09_Cords.fbx";
	//	loadModelOriginal();
	//}
	//else if (key == '4') {
	//	path_Model = "nike_06_latSup.fbx";
	//	loadModelOriginal();
	//}
}

//--------------------------------------------------------------
void ofApp::loadModelSimple() {
	ofLogNotice(__FUNCTION__);

	bool b = modelOriginal.loadModel(path_Model, false);
	if (b) {
		ofLogNotice(__FUNCTION__) << "Loaded modelOriginal: " << path_Model;
	} else {
		ofLogError(__FUNCTION__) << "Unable to load modelOriginal: " << path_Model;
	}

	modelOriginal.setRotation(0, 180, 1, 0, 0);
	modelOriginal.setScale(1.f, 1.f, 1.f);
	modelOriginal.setScaleNormalization(false);
	modelOriginal.enableColors();
}

//--------------------------------------------------------------
void ofApp::loadModelOriginal() {
	ofLogNotice(__FUNCTION__);

	bool b = model.loadModel(path_Model);
	if (b) {
		ofLogNotice(__FUNCTION__) << "Loaded modelOriginal: " << path_Model;
	} else {
		ofLogError(__FUNCTION__) << "Unable to load modelOriginal: " << path_Model;
	}

	model.setRotation(0, 180, 1, 0, 0);
	model.setScale(1.f, 1.f, 1.f);
	model.setScaleNormalization(false);
	model.enableColors();

	//--

	meshesModelOriginal.clear();

	for (size_t i = 0; i < model.getMeshCount(); i++) {

		auto & m = model.getMesh(i);

		//for (size_t j = 0; j < m.getNumColors(); j++) {
		//	m.removeColor(i);
		//}
		//for (size_t j = 0; j < m.getVertices().size(); j++) {
		//	m.addColor(colorOriginal.get());
		//}

		meshesModelOriginal.push_back(m);
	}

	// model info
	sNameModel = path_Model;
}

//--------------------------------------------------------------
void ofApp::loadModelParts() {
	ofLogNotice(__FUNCTION__);

	dir.allowExt("fbx");
	dir.open("models\\nike"); // parts folder
	dir.listDir();

	if (dir.size() == 0)
		ofLogError(__FUNCTION__) << "Files folder not found or empty of .fbx files!";
	else
		ofLogNotice(__FUNCTION__) << "Found " << dir.size() << " files in the folder.";

	//--

	models.clear();
	meshesParts.clear();

	palette.clear();
	paletteParams.clear();

	//--

	// clear
	sceneManager.clearColors();
	sceneManager.clearMaterials();

	// load parts
	for (auto & file : dir.getFiles()) 
	{
		ofLogNotice(__FUNCTION__) << file.getFileName();
		string _path = file.getAbsolutePath();

		std::unique_ptr<ofxAssimpModelLoader> m = std::make_unique<ofxAssimpModelLoader>();
		
		bool b = m->loadModel(_path, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);
		if (b) {
			ofLogNotice(__FUNCTION__) << "Loaded modelOriginal: " << path_Model;
		} else {
			ofLogError(__FUNCTION__) << "Unable to load modelOriginal: " << path_Model;
		}
		
		m->setRotation(0, 180, 1, 0, 0);
		m->setScale(1.f, 1.f, 1.f);
		m->setScaleNormalization(false);
		m->enableColors();

		models.push_back(std::move(m));

		//--

		// Queue one material per model
		sceneManager.addMaterial(file.getBaseName());

		// Palette colors
		string _name = "Color" + ofToString(models.size() - 1);
		ofParameter<ofFloatColor> _col { _name, 
			ofFloatColor(0, 0, 0, 0), 
			ofFloatColor(0, 0, 0, 0), 
			ofFloatColor(1, 1, 1, 1) };
		palette.emplace_back(_col);
		paletteParams.add(_col);

		// Queue one color per model
		sceneManager.addColor(_col);
	}

	ofAddListener(paletteParams.parameterChangedE(), this, &ofApp::ChangedPaletteParams);

	//--

	// Create parts meshes. like refreshModels

	size_t k = 0;
	for (auto & model_ : models) {
		vector<ofMesh> meshes_;

		for (size_t i = 0; i < model_->getMeshCount(); i++) {
			auto & m = model_->getMesh(i);

			//for (size_t j = 0; j < m.getNumColors(); j++) {
			//	m.removeColor(i);
			//}

			//for (size_t j = 0; j < m.getVertices().size(); j++) {
			//	m.addColor(palette[k].get());
			//}

			meshes_.push_back(m);
		}

		meshesParts.push_back(meshes_);

		k++;
	}

	ofLogNotice(__FUNCTION__) << "added " << models.size() << " parts";
	ofLogNotice(__FUNCTION__) << "added " << sceneManager.colors.size() << " colors";
	ofLogNotice(__FUNCTION__) << "added " << sceneManager.materials.size() << " materials";
	ofLogNotice(__FUNCTION__) << "Done";
}

//--------------------------------------------------------------
void ofApp::doRefreshColorsOriginal() {
	ofLogNotice(__FUNCTION__);

	//for (size_t i = 0; i < modelOriginal.getMeshCount(); i++) {
	//	auto & m = meshesModelOriginal[i];

	//	for (size_t j = 0; j < m.getVertices().size(); j++) {
	//		m.setColor(i, colorOriginal.get());
	//	}
	//}
}

//--------------------------------------------------------------
void ofApp::ChangedPaletteParams(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice() << "ChangedPaletteParams:" << name << ": " << e;

	for (int i = 0; i < palette.size(); i++) {
		string _name = "Color" + ofToString(i);
		if (name == _name) {
			sceneManager.setColor(palette[i].get(), i);
		}
	}
}

//--------------------------------------------------------------
void ofApp::ChangedParameters(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice() << "ChangedParameters:" << name << ": " << e;

	//--

	if (name == bMouseCam.getName()) {
		if (bMouseCam)
			cam.enableMouseInput();
		else
			cam.disableMouseInput();
	}

	else if (name == vResetCam.getName()) {
		cam.setupPerspective();
		cam.reset();
		cam.setVFlip(false);
		cam.setDistance(SURFING__SCENE_SIZE_UNIT);
	}

	//--

	// Pick elements to render, Which scene type..

	//else if (name == bDrawShoeParts.getName()) {
	//	if (bDrawShoeParts) {
	//		bDrawOriginal = bDrawTestBox = false;
	//	}
	//}

	//else if (name == bDrawOriginal.getName()) {
	//	if (bDrawOriginal) {
	//		bDrawShoeParts = bDrawTestBox = false;
	//	}
	//}

	//else if (name == bDrawTestBox.getName()) {
	//	if (bDrawTestBox) {
	//		bDrawShoeParts = bDrawOriginal = false;
	//	}
	//}

	else if (name == colorOriginal.getName()) {
		materialOriginal.setAmbientColor(colorOriginal.get());
		materialOriginal.setDiffuseColor(colorOriginal.get());
		doRefreshColorsOriginal();
	}

	else if (name == shininessOriginal.getName()) {
		materialOriginal.setShininess(shininessOriginal.get());
		doRefreshColorsOriginal();
	}
}

//--------------------------------------------------------------
void ofApp::save() {
	ofLogNotice(__FUNCTION__);

	ofxSurfing::saveGroup(parameters);
	ofxSurfing::saveGroup(paletteParams);
	ofxSaveCamera(cam, path_CameraSettings);
}

//--------------------------------------------------------------
void ofApp::load() {
	ofLogNotice(__FUNCTION__);

	ofxSurfing::loadGroup(parameters);
	ofxSurfing::loadGroup(paletteParams);
	ofxLoadCamera(cam, path_CameraSettings);
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofLogNotice(__FUNCTION__);

	ofRemoveListener(parameters.parameterChangedE(), this, &ofApp::ChangedParameters);
	ofRemoveListener(paletteParams.parameterChangedE(), this, &ofApp::ChangedPaletteParams);

	save();
}

////--------------------------------------------------------------
//void ofApp::loadModel(int pos) {
//	model = models[index];
//
//	meshesModelOriginal.clear();
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
//		meshesModelOriginal.push_back(m);
//	}
//
//	// model info
//	sNameModel = path_Model;
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
//		sceneManager.setColor(palette[i], i);
//	}
//}