#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofLogNotice(__FUNCTION__);

#if 1
	ofxSurfing::setOfxGuiTheme();
	ofxSurfing::setWindowTitleAsProjectName();
	ofxSurfing::setWindowAtMonitor(-1);
	//ofSetLogLevel(OF_LOG_VERBOSE);
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

	rotateSpeed = 0.5;
	bAnimRotate = false;

	camera.setupPerspective();
	camera.reset();
	camera.setVFlip(false);
	camera.setDistance(SURFING__PBR__SCENE_SIZE_UNIT);
}

//--------------------------------------------------------------
void ofApp::setupParams() {
	ofLogNotice(__FUNCTION__);

	bGui.set("Gui", true);

	drawModeParams.add(bDrawOriginal);
	drawModeParams.add(bDrawParts);
	drawModeParams.add(bDrawOnePiece);
	drawParams.add(drawModeParams);
	drawParams.add(bDrawTestBox);

	flootParams.add(szFloor);
	flootParams.add(bDrawFloor);
	flootParams.add(bDrawGrid);
	drawParams.add(flootParams);

	transformsParams.add(scale);
	transformsParams.add(yPos);
	transformsParams.add(yRotate);
	transformsParams.add(vResetTransforms);
	listenerReset = vResetTransforms.newListener([this](void) {
		doResetTransforms();
	});

	cameraParams.add(bMouseCamera);
	cameraParams.add(rotateSpeed);
	cameraParams.add(bAnimRotate);
	cameraParams.add(vResetCam);

	parameters.add(cameraParams);
	parameters.add(transformsParams);
	parameters.add(drawParams);
	parameters.add(materialAux.bGui);
	parameters.add(bHelp);

	ofAddListener(parameters.parameterChangedE(), this, &ofApp::Changed);
	ofAddListener(drawParams.parameterChangedE(), this, &ofApp::ChangedDraw);
}

//--------------------------------------------------------------
void ofApp::setupObjects() {
	ofLogNotice(__FUNCTION__);

	pathModel = pathModel_DEFAULT;

	// Originally textured into the fbx and his textures folder.
	loadModelOriginal();

	// Original Meshed
	loadModelOriginalMeshed();

	// By parts
	loadModelParts();
}

//--------------------------------------------------------------
void ofApp::setupScene() {
	ofLogNotice(__FUNCTION__);

	materialAux.setup("Material Aux");

	//--

	// Pass render functions required to do the shadow passes!
	callback_t f = std::bind(&ofApp::renderScene, this);
	sceneManager.setFunctionRenderScene(f);

	// Queue one material per parts/model
	// Queue one color per model
	// but mainly to be accessible to load palettes on the fly easily.
	for (size_t i = 0; i < models.size(); i++) {
		string n = namesModels[i];

		string nm = "M" + n;
		sceneManager.addMaterial(nm);
	}

	//--

	sceneManager.setupBuild();
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	ofLogNotice(__FUNCTION__);

	gui.setup(parameters);

	//refresh gui
	gui.getGroup(cameraParams.getName()).minimize();
	gui.getGroup(transformsParams.getName()).minimize();
	gui.getGroup(drawParams.getName()).getGroup(flootParams.getName()).minimize();

	buildHelp();
}

//--

//--------------------------------------------------------------
void ofApp::startup() {
	ofLogNotice(__FUNCTION__);

	// Defaults
	bDrawParts = true;
	doResetTransforms();
	doResetCamera();

	//--

	// Settings
	load();
}

//--------------------------------------------------------------
void ofApp::update() {
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

	ofxSurfing::setGuiPositionBelowTo(materialAux.gui, gui);
	materialAux.drawGui(); //below linked

	drawHelp();
}

//--------------------------------------------------------------
void ofApp::buildHelp() {
	sHelp = "";
	sHelp += "HELP\n";
	sHelp += ofxSurfing::getProjectName() + "\n";
	sHelp += "\n";
	sHelp += "g Gui\n";
	sHelp += "h Help\n";
	sHelp += "m Mouse Camera\n";
	sHelp += "r Rotate\n";
	sHelp += "\n";
	sHelp += "DRAW\n";
	sHelp += "1 Original\n";
	sHelp += "2 Parts\n";
	sHelp += "3 OnePiece (Aux Material)\n";
}

//--------------------------------------------------------------
void ofApp::drawHelp() {

	if (bHelp) {
		string s = sHelp;
		s += "\n" + ofToString(ofGetFrameRate(), 1) + " FPS";
		ofxSurfing::ofDrawBitmapStringBox(s, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
	}
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
void ofApp::pushTransforms() {
	ofPushMatrix();

	// Position
	float yUnit = SURFING__PBR__SCENE_SIZE_UNIT / 2.f;
	float y = ofMap(yPos, -1.f, 1.f, -yUnit, yUnit, true);

	// Scale
	float s = ofMap(scale, -1.f, 1.f,
		1.f / SURFING__PBR__SCENE_SIZE_UNIT, SURFING__PBR__SCENE_TEST_UNIT_SCALE, true);
	s *= (SURFING__PBR__SCENE_SIZE_UNIT * 0.05f); //tweak here to set max zoom.

	// Rotation anim
	int tmax = 30;
	// 30 seconds to complete 360deg
	// at 60 fps, for the slower speed.
	// faster speed is one second per 360deg.
	int f = ofMap(rotateSpeed, 0.f, 1.f, 60 * tmax, 60, true);
	float d = ofMap(ofGetFrameNum() % f, 0, f, 0.f, 360.f);

	ofTranslate(0, y, 0);
	ofScale(s);
	ofRotateYDeg(yRotate);
	if (bAnimRotate) ofRotateYDeg(d);
}

//--------------------------------------------------------------
void ofApp::popTransforms() {
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::renderScene() {

	pushTransforms();
	{

		//--

		ofRotateYDeg(180); //flip

		//--

		// Parts

		if (bDrawParts) {
			for (size_t i = 0; i < models.size(); i++) {

				// Begin i material
				sceneManager.beginMaterial(i);
				{
					// Meshes
					for (auto & mesh_ : meshesParts[i]) {
						mesh_.drawFaces();
					}

					//// Models
					//models[i]->drawFaces(); // do not works when applying the material?
				}
				// End i material
				sceneManager.endMaterial(i);
			}
		}

		//--

		// Original

		if (bDrawOriginal) {
			modelOriginal.drawFaces();
		}

		//--

		materialAux.begin();
		{
			//--

			// One Piece

			if (bDrawOnePiece) {
				for (size_t i = 0; i < meshesModelOriginal.size(); i++) {
					meshesModelOriginal[i].drawFaces();
				}
			}

			//----

			// TestBox

			if (bDrawTestBox) {
				ofRotateYDeg(45);
				ofBox(0, 0, 0, 5);
			}

			//--
		}
		materialAux.end();

		//--
	}

	popTransforms();
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
	}
}

//--

//--------------------------------------------------------------
void ofApp::drawFloor() {

	materialAux.begin();
	{
		// Floor thin box

		float r = ofMap(szFloor, 0, 1, 1, 2, true);
		float sz = SURFING__PBR__SCENE_SIZE_UNIT * r;

		ofPushMatrix();
		{
			ofRotateXDeg(90);
			ofTranslate(0, 0, yOffsetFloor * sz);
			ofDrawBox(glm::vec3(0, 0, 0), sz, sz, 1);
		}
		ofPopMatrix();
	}
	materialAux.end();
}

//--------------------------------------------------------------
void ofApp::drawGrid() {

	// Floor grid with one single rectangle

	ofPushStyle();
	ofSetColor(0, 255);
	ofNoFill();
	ofSetLineWidth(2.f);

	float r = ofMap(szFloor, 0, 1, 1, 2, true);
	float sz = SURFING__PBR__SCENE_SIZE_UNIT * r;

	ofPushMatrix();
	{
		ofRotateXDeg(90);
		ofTranslate(-sz / 2, -sz / 2, yOffsetFloor * sz);
		ofDrawRectangle(0, 0, sz, sz);
	}
	ofPopMatrix();

	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::beginCamera() {

	camera.begin();

	//--

	if (bDrawFloor) {
		drawFloor();
	}

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
	camera.end();
}

//--

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	ofLogNotice(__FUNCTION__) << key;

	if (key == 'g')
		bGui = !bGui;

	else if (key == 'h')
		bHelp = !bHelp;

	else if (key == 'm') {
		bMouseCamera = !bMouseCamera;
	}

	else if (key == 'r')
		bAnimRotate = !bAnimRotate;

	if (key == OF_KEY_BACKSPACE) {
		doRandomPalette();
		//colorOriginal = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
	}

	else if (key == '1') {
		bDrawOriginal = true;
	}
	else if (key == '2') {
		bDrawParts = true;
	}
	else if (key == '3') {
		bDrawOnePiece = true;
	}
}

//--------------------------------------------------------------
void ofApp::loadModelOriginal() {
	ofLogNotice(__FUNCTION__);

	bool b = modelOriginal.loadModel(pathModel, false);

	if (b) {
		ofLogNotice(__FUNCTION__) << "Loaded modelOriginal: " << pathModel;
	} else {
		ofLogError(__FUNCTION__) << "Unable to load modelOriginal: " << pathModel;
	}

	modelOriginal.setRotation(0, 180, 1, 0, 0);
	modelOriginal.setRotation(1, 180, 0, 1, 0);
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

	model.enableColors();

	//--

	meshesModelOriginal.clear();

	for (size_t i = 0; i < model.getMeshCount(); i++) {

		meshesModelOriginal.push_back(model.getMesh(i));
	}

	// model info
	nameModel = pathModel;
}

//--------------------------------------------------------------
void ofApp::loadModelParts() {
	ofLogNotice(__FUNCTION__);

	string pathParts = pathParts_DEFAULT;

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
		string path_ = file.getAbsolutePath();

		std::unique_ptr<ofxAssimpModelLoader> m = std::make_unique<ofxAssimpModelLoader>();

		bool b = m->loadModel(path_, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);

		if (b) {
			ofLogNotice(__FUNCTION__) << "Loaded model part: " << path_;
		} else {
			ofLogError(__FUNCTION__) << "Unable to load model part: " << path_;
		}

		m->enableColors();

		models.push_back(std::move(m));

		//--

		// Set a name to be used on gui groups too
		string n = ofToString(models.size() - 1) + " " + file.getBaseName();
		namesModels.push_back(n);

		//--

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
		vector<ofVboMesh> meshes_;
		for (size_t i = 0; i < model_->getMeshCount(); i++) {
			ofVboMesh m = model_->getMesh(i);
			meshes_.push_back(m);
		}
		meshesParts.push_back(meshes_);
		k++;
	}

	ofPopMatrix();
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

	// Select what to render. Exclusive / only one enabled.

	if (name == bDrawOriginal.getName()) {
		if (bDrawOriginal) {
			if (bDrawParts) bDrawParts = false;
			if (bDrawOnePiece) bDrawOnePiece = false;
			if (bDrawTestBox) bDrawTestBox = false;
		}
	}

	else if (name == bDrawParts.getName()) {
		if (bDrawParts) {
			if (bDrawOriginal) bDrawOriginal = false;
			if (bDrawOnePiece) bDrawOnePiece = false;
			if (bDrawTestBox) bDrawTestBox = false;
		}
	}

	else if (name == bDrawOnePiece.getName()) {
		if (bDrawOnePiece) {
			if (bDrawOriginal) bDrawOriginal = false;
			if (bDrawParts) bDrawParts = false;
			if (bDrawTestBox) bDrawTestBox = false;
		}
	}

	else if (name == bDrawTestBox.getName()) {
		if (bDrawTestBox) {
			if (bDrawParts) bDrawParts = false;
			if (bDrawOriginal) bDrawOriginal = false;
			if (bDrawOnePiece) bDrawOnePiece = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::Changed(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice() << "Changed:" << name << ": " << e;

	//--

	if (name == bMouseCamera.getName()) {
		if (bMouseCamera)
			camera.enableMouseInput();
		else
			camera.disableMouseInput();
	}

	else if (name == vResetCam.getName()) {
		doResetCamera();
	}
}

//--------------------------------------------------------------
void ofApp::save() {
	ofLogNotice(__FUNCTION__);

	ofxSurfing::saveGroup(parameters);
	ofxSurfing::saveGroup(paletteParams);

	ofxSaveCamera(camera, path_CameraSettings);
}

//--------------------------------------------------------------
void ofApp::load() {
	ofLogNotice(__FUNCTION__);

	ofxSurfing::loadGroup(parameters);
	ofxSurfing::loadGroup(paletteParams);

	ofxLoadCamera(camera, path_CameraSettings);
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofLogNotice(__FUNCTION__);

	ofRemoveListener(parameters.parameterChangedE(), this, &ofApp::Changed);
	ofRemoveListener(drawParams.parameterChangedE(), this, &ofApp::ChangedDraw);

	ofRemoveListener(paletteParams.parameterChangedE(), this, &ofApp::ChangedPalette);

	save();
}

//--------------------------------------------------------------
void ofApp::doResetTransforms() {
	ofLogNotice(__FUNCTION__);

	//szFloor = 0;

	scale = -0.6;
	yRotate = -45;
	yPos = 0;
}

//--------------------------------------------------------------
void ofApp::doResetCamera() {
	ofLogNotice(__FUNCTION__);

	camera.setupPerspective();
	camera.reset();
	camera.setVFlip(false);
	camera.setDistance(SURFING__PBR__SCENE_SIZE_UNIT);

	rotateSpeed = 0.5f;

	//refresh
	bMouseCamera.set(bMouseCamera.get());
}
