#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofLogNotice(__FUNCTION__);

	//--

#if 1
	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
	ofxSurfing::setWindowAtMonitor(-1);
	ofSetLogLevel(OF_LOG_VERBOSE);
#endif

	setupCamera();
	setupParams();
	setupObjects();
	setupScene();
	setupGui();

	startup();
}

//--------------------------------------------------------------
void ofApp::setupCamera() {
	ofLogNotice(__FUNCTION__);

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

//--------------------------------------------------------------
void ofApp::setupParams() {
	ofLogNotice(__FUNCTION__);

	bGui.set("Gui", true);

	transformParams.add(scale);
	transformParams.add(yRotate);

	cameraParams.add(bMouseCam);
	cameraParams.add(bRotate);
	cameraParams.add(speedRotate);
	cameraParams.add(vResetCam);

	drawParams.add(bDrawOriginal);
	drawParams.add(bDrawParts);
	drawParams.add(bDrawTestBox);
	drawParams.add(bDrawGrid);

	parameters.add(drawParams);
	parameters.add(transformParams);
	parameters.add(cameraParams);

	parameters.add(materialOriginal.bGui);
	
	ofAddListener(parameters.parameterChangedE(), this, &ofApp::Changed);
	ofAddListener(drawParams.parameterChangedE(), this, &ofApp::ChangedDraw);
}

//--------------------------------------------------------------
void ofApp::setupObjects() {
	ofLogNotice(__FUNCTION__);

	pathModel = "models\\nike\\original\\nike1.fbx";

	// Original Simple
	loadModelOriginalSimple();

	// Original
	loadModelOriginalMeshed();

	// By parts
	loadModelParts();
}

//--------------------------------------------------------------
void ofApp::setupScene() {
	ofLogNotice(__FUNCTION__);

	materialOriginal.setup("Material");

	//--

	// Pass render functions required to do the shadow passes!
	callback_t f = std::bind(&ofApp::renderScene, this);
	sceneManager.setFunctionRenderScene(f);

	// Queue one material per parts/model
	// Queue one color per model
	for (size_t i = 0; i < models.size(); i++) {

		string n = namesModels[i];
		sceneManager.addMaterial(n);

		ofFloatColor c = palette[i];
		sceneManager.addColor(c);
	}

	//--

	sceneManager.setup();
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	ofLogNotice(__FUNCTION__);

	gui.setup(parameters);

	gui.getGroup(cameraParams.getName()).minimize();
	gui.getGroup(transformParams.getName()).minimize();
}

//--

//--------------------------------------------------------------
void ofApp::startup() {
	ofLogNotice(__FUNCTION__);

	//--

	load();
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
void ofApp::drawGui() {
	if (!bGui) return;

	ofDisableDepthTest();

	gui.draw(); //anchor

	ofxSurfing::setGuiPositionRightTo(sceneManager.gui, gui);
	sceneManager.drawGui(); //right linked

	ofxSurfing::setGuiPositionBelowTo(materialOriginal.gui, gui, true);
	materialOriginal.drawGui(); //below linked
}

//--------------------------------------------------------------
void ofApp::drawScene() {

	ofEnableDepthTest();

	sceneManager.updateLights();

	beginCamera();
	{
		sceneManager.beginLights();
		{
			renderScene();

			sceneManager.drawDebugLights();
		}
		sceneManager.endLights();
	}
	endCamera();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	float s = ofMap(scale, 0, 1, 50, 700, true);
	//--

	// Parts

	if (bDrawParts) {
		ofPushMatrix();
		ofRotateYDeg(yRotate + 180);
		ofScale(s);
		{
			for (size_t i = 0; i < models.size(); i++) {

				// Begin material
				sceneManager.beginMaterial(i);
				{
#if 1
					// Meshes
					for (auto & mesh_ : meshesParts[i]) {
						mesh_.drawFaces();
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
		ofPopMatrix();
	}

	//----

	materialOriginal.begin();
	{
		//--

		// TestBox

		if (bDrawTestBox) {
			ofRotateYDeg(yRotate + 90);
			ofScale(s);

			ofBox(0, 0, 0, 2);
		}

		//--

		// Original

		if (bDrawOriginal) {
			ofPushMatrix();
			ofRotateYDeg(yRotate + 180);
			ofScale(s);
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
			ofPopMatrix();
		}

		//--
	}
	materialOriginal.end();
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

	// Floor grid with one single rectangle

	float yr = 0.3;
	int sz = 2 * 1000;

	ofPushStyle();
	ofPushMatrix();
	ofTranslate(0, 3, 0);

	ofSetColor(0, 0, 0, 255);
	//ofSetColor(0, 255, 0, 255);

	ofNoFill();
	ofSetLineWidth(1.f);
	ofRotateXDeg(90);
	ofTranslate(-sz / 2, -sz / 2, yr * sz);
	ofDrawRectangle(0, 0, sz, sz);
	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::beginCamera() {
	cam.begin();

	//--

	if (bDrawGrid) {
		drawGrid();
	}

	//--

	{
		ofPushMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::endCamera() {
	{
		ofPopMatrix();
	}
	cam.end();
}

//--

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	ofLogNotice(__FUNCTION__) << key;

	if (key == 'g')
		bGui = !bGui;

	else if (key == 'm') {
		bMouseCam = !bMouseCam;
		if (bMouseCam)
			cam.enableMouseInput();
		else
			cam.disableMouseInput();
	}

	else if (key == 'r')
		bRotate = !bRotate;

	if (key == OF_KEY_BACKSPACE) {
		doRandomPalette();
		//colorOriginal = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
	}
}

//--------------------------------------------------------------
void ofApp::loadModelOriginalSimple() {
	ofLogNotice(__FUNCTION__);

	bool b = modelOriginal.loadModel(pathModel, false);
	if (b) {
		ofLogNotice(__FUNCTION__) << "Loaded modelOriginal: " << pathModel;
	} else {
		ofLogError(__FUNCTION__) << "Unable to load modelOriginal: " << pathModel;
	}

	modelOriginal.setRotation(0, 180, 1, 0, 0);
	modelOriginal.setScale(1.f, 1.f, 1.f);
	modelOriginal.setScaleNormalization(false);
	modelOriginal.enableColors();
}

//--------------------------------------------------------------
void ofApp::loadModelOriginalMeshed() {
	ofLogNotice(__FUNCTION__);

	ofxAssimpModelLoader model;

	bool b = model.loadModel(pathModel);
	if (b) {
		ofLogNotice(__FUNCTION__) << "Loaded modelOriginal: " << pathModel;
	} else {
		ofLogError(__FUNCTION__) << "Unable to load modelOriginal: " << pathModel;
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
	nameModel = pathModel;
}

//--------------------------------------------------------------
void ofApp::loadModelParts() {
	ofLogNotice(__FUNCTION__);

	string pathParts = "models\\nike\\parts";
	dir.allowExt("fbx");
	dir.open(pathParts); // parts folder
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

	// Load parts
	for (auto & file : dir.getFiles()) {
		ofLogNotice(__FUNCTION__) << file.getFileName();
		string _path = file.getAbsolutePath();

		std::unique_ptr<ofxAssimpModelLoader> m = std::make_unique<ofxAssimpModelLoader>();

		bool b = m->loadModel(_path, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);
		if (b) {
			ofLogNotice(__FUNCTION__) << "Loaded modelOriginal: " << _path;
		} else {
			ofLogError(__FUNCTION__) << "Unable to load modelOriginal: " << _path;
		}

		m->setRotation(0, 180, 1, 0, 0);
		m->setScale(1.f, 1.f, 1.f);
		m->setScaleNormalization(false);
		m->enableColors();

		models.push_back(std::move(m));

		//--

		string n = "C" + ofToString(models.size() - 1) + " " + file.getBaseName();
		//string n = "C " + ofToString(models.size() - 1);
		//string n = file.getBaseName();

		namesModels.push_back(n);

		// Palette colors
		ofParameter<ofFloatColor> c { n,
			ofFloatColor(0, 0, 0, 0),
			ofFloatColor(0, 0, 0, 0),
			ofFloatColor(1, 1, 1, 1) };

		palette.emplace_back(c);
		paletteParams.add(c);
	}

	ofLogNotice(__FUNCTION__) << "Added " << models.size() << " parts from model: " << pathModel;

	ofAddListener(paletteParams.parameterChangedE(), this, &ofApp::ChangedPalette);

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
void ofApp::ChangedPalette(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice() << "ChangedPalette:" << name << ": " << e;

	//for (int i = 0; i < palette.size(); i++) {
	//	string _name = "Color" + ofToString(i);
	//	if (name == _name) {
	//		sceneManager.setColor(palette[i].get(), i);
	//	}
	//}
}

//--------------------------------------------------------------
void ofApp::ChangedDraw(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice() << "ChangedDraw:" << name << ": " << e;

	//--

	// Select what to render. one single only.

	if (name == bDrawParts.getName()) {
		if (bDrawParts) {
			if (bDrawOriginal) bDrawOriginal = false;
			if (bDrawTestBox) bDrawTestBox = false;
		}
	}

	else if (name == bDrawOriginal.getName()) {
		if (bDrawOriginal) {
			if (bDrawParts) bDrawParts = false;
			if (bDrawTestBox) bDrawTestBox = false;
		}
	}

	else if (name == bDrawTestBox.getName()) {
		if (bDrawTestBox) {
			if (bDrawParts) bDrawParts = false;
			if (bDrawOriginal) bDrawOriginal = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice() << "Changed:" << name << ": " << e;

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

	ofRemoveListener(parameters.parameterChangedE(), this, &ofApp::Changed);
	ofRemoveListener(drawParams.parameterChangedE(), this, &ofApp::ChangedDraw);

	ofRemoveListener(paletteParams.parameterChangedE(), this, &ofApp::ChangedPalette);

	save();
}
