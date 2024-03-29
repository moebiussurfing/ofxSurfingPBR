#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

#if 1
	ofxSurfing::setOfxGuiTheme(); // Customize ofxGui theme.
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.
	ofxSurfing::setWindowAtMonitor(-1); // Move to left display and set landscape.
#endif

	// Verbose
	//ofSetLogLevel(OF_LOG_VERBOSE);

	//--

	setupPBR();

	//--

	// Scene

	setupMesh();

#ifdef OF_APP__USE__MODELS_MANAGER
	modelsManager.setup();

	listenerIndexModel = modelsManager.indexFile.newListener([this](int & i) {
		buildHelp(); // refresh help info
		modelsManager.refreshGui();
	});
#else
	setupModel();
#endif

	//--

	setupParams();
	setupGui();
	startup();
}

//--------------------------------------------------------------
void ofApp::setupPBR() {

	// Setup passing the camera object
	pbr.setup();

	// Pass the render scene
	callback_t f = std::bind(&ofApp::renderScene, this);
	pbr.setFunctionRenderScene(f);

	//--

	// Optional

#ifdef OF_APP__USE__OPTIONAL_SETUP

	// 1.
	// Set log level verbose
	pbr.setLogLevel(OF_LOG_VERBOSE);

	// Check if it's the first time opening the App.
	// (that's true when a JSON scene file settings is not located.)
	// If not, we will force some settings for the scene:
	// Set to 1 to forcing overwrite scene always,
	// even when a previous file settings found!
	// Set to 0 to force settings only when opening for the first time,
	// that means that there's no settings file located.
	#define FORCE_OVERWRITE_SCENE_ALWAYS_EVEN_IF_LOCATED_FILE_SETTINGS 1
	#if (!FORCE_OVERWRITE_SCENE_ALWAYS_EVEN_IF_LOCATED_FILE_SETTINGS)
	// 2.
	if (!pbr.getSettingsFileFound())
	#endif
	{
		// 3.
		ofLogWarning("ofApp") << "Settings file ofxSurfingPBR_Scene.json not found!";
		ofLogWarning("ofApp") << "Forcing the ofxSurfingPBR initial scene with some settings.";

		// Background
		pbr.bg.bDrawBgColorObject.set(true);
		pbr.bg.globalColor.set(ofFloatColor { 0, 1, 1, 1 });
		pbr.bg.bDrawWireframe.set(true);
		pbr.bg.wireColor.set(ofFloatColor { 0, 0, 0, 0.5 });

		// Floor
		pbr.bDrawFloorBox = true;

		// Floor material
		pbr.materialFloor.globalColor.set(ofFloatColor { 0, 0.5, 0.25, 1 });
		pbr.materialFloor.setRoughness(0.5); //accessing the public ofMaterial

		// Other objects material
		pbr.material.shininess = 0.5 * SURFING__PBR__MAX_SHININESS;
		pbr.material.roughness = 0.5;
		pbr.material.metallic = 0.5;
		pbr.material.reflectance = 0.5;
		pbr.material.globalColor.set(ofFloatColor::orange);
		pbr.material.globalAlpha.set(1.0);

		// Some internals
		bHelp = true;
		pbr.bDebug = true;
		pbr.bHelp = true;
	}

	// Force replace the default cubeMap
	pbr.loadCubeMap("cubemaps/kloppenheim_06_puresky_1k.exr");

#endif
}

//--------------------------------------------------------------
void ofApp::setupParams() {

	// Parameters

	parameters.setName("ofApp");

	parameters.add(vNextIndexScene);
	parameters.add(vPrevIndexScene);
	parameters.add(indexScene);
	parameters.add(nameScene);

	paramsGlobalTransforms.setName("Global Transforms");
	paramsGlobalTransforms.add(yPos);
	paramsGlobalTransforms.add(scale);
	parameters.add(paramsGlobalTransforms);

	paramsAnimate.setName("Animate");
	paramsAnimate.add(bAnimRotate);
	paramsAnimate.add(rotateSpeed);
	paramsAnimate.add(bAnimZoom);
	paramsAnimate.add(zoomSpeed);
	paramsAnimate.add(powZoom);
	parameters.add(paramsAnimate);

	parameters.add(vReset);

#ifndef SURFING__PBR__USE_ADDON_EASY_CAM
	parameters.add(pbr.vResetCamera);
#else
	parameters.add(pbr.camera.vResetCamera);
#endif

	parameters.add(bHelp);

	nameScene.setSerializable(false);

	//--

	// Callbacks

	listenerIndexScene = indexScene.newListener([this](int & i) {
		nameScene = namesScenes[i];

		buildHelp();
		refreshGui();
	});

	listenerNext = vNextIndexScene.newListener([this](void) {
		doNextScene();
	});
	listenerPrev = vPrevIndexScene.newListener([this](void) {
		doPrevScene();
	});

	listenerReset = vReset.newListener([this](void) {
		doReset();
	});

	ofAddListener(parameters.parameterChangedE(), this, &ofApp::Changed);
}

//--------------------------------------------------------------
void ofApp::setupGui() {

	gui.setup(parameters);

	//--

	guiManager.setup(&gui);
	guiManager.add(&gui);
#ifdef OF_APP__USE__MODELS_MANAGER
	guiManager.add(modelsManager.getGuiPtr());
#endif
	guiManager.startup();

	//--

	//ofxSurfing::setGuiPositionToLayoutPanelsCentered(gui, 1, ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER);
	gui.getGroup(paramsAnimate.getName()).minimize();

	refreshGui();

	buildHelp();
}

//--------------------------------------------------------------
void ofApp::Changed(ofAbstractParameter & e) {
	autoSaver.saveSoon();
}

//--------------------------------------------------------------
void ofApp::startup() {

	doReset();
	// before loading settings or in the case that not settings file located
	// aka app is opening for the first time.

	//--

	// Auto saver
	// Register the local save function to be called when saving is required.
	callback_t f = std::bind(&ofApp::save, this);
	autoSaver.setFunctionSaver(f);

	// Load settings
	load();
}

//--------------------------------------------------------------
void ofApp::setupMesh() {

	// For scene 1

	pathMesh = "models/ofLogoHollow.ply";

	mesh.load(pathMesh);
	mesh.mergeDuplicateVertices();

	// flip the normals to fix ofLogoHollow.ply
	if (1) {
		for (size_t i = 0; i < mesh.getNumNormals(); i++) {
			mesh.getNormals()[i] *= -1.f;
		}
	}
}

//--------------------------------------------------------------
void ofApp::drawMesh() {

	// Hardcoded transforms
	// for the ofLogoHollow.ply mesh.
	ofPushMatrix();
	ofTranslate(0, 60, 0);
	ofScale(40.f);
	ofRotateXDeg(-90);
	{
		mesh.draw();
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::update() {

	if (bAnimZoom) {
		// Shows how to access the internal camera.
		// Make some animation to the distance/zoom.
		float t = ofGetElapsedTimef();
		float f = ofMap(zoomSpeed, 0.f, 1.f, 0.1, 2, true);
		float v = glm::sin(2.f * glm::pi<float>() * t * f);
		float a = ofMap(powZoom, 0.f, 1.f, 1.02, 10, true);
		int unit = 600;
		v = ofMap(v, -1, 1, unit, unit * a, true);

		// Set the camera distance:
		pbr.getOfEasyCamPtr()->setDistance(v); // We can access the internal ptr for the cam.
		//camera.setDistance(v); // But note also that will does the same than the above line.
	}

#ifndef OF_APP__USE__MODELS_MANAGER
	refreshGui();
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// Scene transforms (ofApp local)
	ofPushMatrix();
	updateSceneTransforms();
	{
		// Floor
		pbr.drawFloor();

		// Other objects
		pbr.beginMaterial();
		{
			drawMyScene();
		}
		pbr.endMaterial();
	}
	ofPopMatrix();
}

//------

//--------------------------------------------------------------
void ofApp::updateSceneTransforms() {

	// Position
	float yUnit = SURFING__PBR__SCENE_SIZE_UNIT / 2.f;
	float y = ofMap(yPos, -1.f, 1.f, -yUnit, yUnit, true);

	// Scale
	float s = ofMap(scale, -1.f, 1.f, 1.f / SURFING__PBR__SCENE_TEST_UNIT_SCALE, SURFING__PBR__SCENE_TEST_UNIT_SCALE, true);

	// Rotation
	int tmax = 120;
	// 30 seconds to complete 360deg
	// at 60 fps, for the slower speed.
	// faster speed is one second per 360deg.
	int f = ofMap(rotateSpeed, 0.f, 1.f, 60 * tmax, 60, true);
	float d = ofMap(ofGetFrameNum() % f, 0, f, 0.f, 360.f);

	ofTranslate(0, y, 0);
	ofScale(s);
	if (bAnimRotate) ofRotateYDeg(d);
}

//--------------------------------------------------------------
void ofApp::drawMyScene() {

	/*
		We draw our scene here!
	*/

	//--

	// Draw the model object/s
	{
		// Scene 0: Three Prims
		if (indexScene == 0) {
			pbr.drawTestScene();
		}

		//--

		// Scene 1: Mesh
		if (indexScene == 1) {
			drawMesh();
		}

		//--

		// Scene 2: Model
		else if (indexScene == 2) {
#ifdef OF_APP__USE__MODELS_MANAGER
			modelsManager.draw();
#else
			drawModel();
#endif
		}
	}
}

//------

//--------------------------------------------------------------
void ofApp::drawGui() {
	if (!pbr.bGui) return; //used as global show ui.

	pbr.drawGui();

	//--

	if (pbr.bGuiOfxGui) {

#ifdef OF_APP__USE__MODELS_MANAGER
		if (indexScene == 2) {
			guiManager.draw();
		} else {
			gui.draw();
		}
#else
		gui.draw();
#endif
	}

	//--

	drawHelp();
}

//--------------------------------------------------------------
void ofApp::drawHelp() {

	// Responsive layout
	if (bHelp) {
		ofxSurfing::ofDrawBitmapStringBox(sHelp, &gui, ofxSurfing::SURFING_LAYOUT_TOP_LEFT);
	}

#ifdef OF_APP__USE__MODELS_MANAGER
	if (indexScene == 2)
		if (guiManager.getGuiVisible(1))
			modelsManager.drawHelp();
#endif
}

//--

//--------------------------------------------------------------
void ofApp::doReset() {

	// Scene
	scale = -0.6;
	yPos = 0;
	rotateSpeed = 0.5;
	bAnimRotate = false;
	bAnimZoom = false;
	powZoom = 0.5;
	zoomSpeed = 0.5;
}

//--------------------------------------------------------------
void ofApp::doNextScene() {
	if (indexScene < indexScene.getMax())
		indexScene = indexScene + 1;
	else
		indexScene = 0;
}

//--------------------------------------------------------------
void ofApp::doPrevScene() {
	if (indexScene > 0)
		indexScene = indexScene - 1;
	else
		indexScene = indexScene.getMax();
}

//--

//--------------------------------------------------------------
void ofApp::buildHelp() {

	sHelp = "";
	sHelp += "HELP\n";
	sHelp += "ofApp\n";
	sHelp += "\n";
	sHelp += "H Help\n";
	sHelp += "g Gui\n";
	sHelp += "G ofxGui\n";
	sHelp += "R Rotate\n";
	sHelp += "A Zoom Anim\n";
	sHelp += "\n";
	sHelp += "#" + ofToString(indexScene) + "\n";
	sHelp += "SCENE\n";
	sHelp += namesScenes[indexScene] + "\n";
	sHelp += "\n";
	sHelp += "RIGHT Next\n";
	sHelp += "LEFT  Previous\n";
}

//--------------------------------------------------------------
void ofApp::refreshGui() {

	guiManager.refreshGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'A')
		bAnimZoom = !bAnimZoom;

	if (key == 'R')
		bAnimRotate = !bAnimRotate;

	else if (key == 'H')
		bHelp = !bHelp;

	else if (key == OF_KEY_RIGHT) {
		doNextScene();
	} else if (key == OF_KEY_LEFT) {
		doPrevScene();
	}

	//--

#ifdef OF_APP__USE__MODELS_MANAGER
	modelsManager.keyPressed(key);
#endif
}

//--------------------------------------------------------------
void ofApp::windowResized(ofResizeEventArgs & resize) {
	glm::vec2 sz { resize.width, resize.height };
	static glm::vec2 sz_ { -1, -1 };
	if (sz != sz_) {
		sz_ = sz;
		ofLogNotice("ofApp") << "windowResized() Size: " << sz_.x << "," << sz_.y;

		refreshGui(); // to refresh the responsive layout
	}
}

//--------------------------------------------------------------
void ofApp::load() {
	//return;
	ofLogNotice("ofApp") << "load()";

	autoSaver.pause();
	ofxSurfing::loadSettings(parameters, pathSettings);
	autoSaver.start();
}

//--------------------------------------------------------------
void ofApp::save() {
	ofLogNotice("ofApp") << "save()";

	ofxSurfing::saveSettings(parameters, pathSettings);
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofLogNotice("ofApp") << "exit()";

	ofRemoveListener(parameters.parameterChangedE(), this, &ofApp::Changed);

	pbr.exit();
}

//--

#ifndef OF_APP__USE__MODELS_MANAGER
//--------------------------------------------------------------
void ofApp::setupModel() {

	// For scene 2

	float scaled = 1.0f;

	//--

	pathModel = "models/head25k.obj";
	scaled = 1.5;
	// Note that this file model have problems with normals...
	// We will need:
	// glFrontFace(GL_CCW);

	//pathModel = "models/basic_form.ply";
	//scaled = 2.0;

	//pathModel = "models/Fox_05.fbx";
	//scaled = 0.05;

	//--

	bool b = loadModel(pathModel, scaled);
	if (b)
		ofLogNotice("ofApp") << "Successfully loaded model file: " << pathModel;
	else
		ofLogError("ofApp") << "Unable to load model file " << pathModel << ". Maybe not found!";
}

//--------------------------------------------------------------
bool ofApp::loadModel(string path, float scaled) {

	bool b = model.load(path, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);

	// hardcoded
	model.setPosition(0, 2, 0); //TODO

	// we can apply transforms to models
	if (scaled != 1) model.setScale(scaled, scaled, scaled);

	meshesModel.clear();
	size_t sz = model.getMeshCount();
	for (int j = 0; j < sz; j++) {
		ofVboMesh vm = model.getMesh(j);
		meshesModel.push_back(vm);
	}

	return b;
}

//--------------------------------------------------------------
void ofApp::drawModel() {
	glFrontFace(GL_CCW);
	// fix for "transparent" for model head25k.obj with normals problems..

	#define USE_MODEL_TRANSFORMS 1

	#if (USE_MODEL_TRANSFORMS)
	// Scene previous hardcoded transforms
	ofPushMatrix();
	ofTranslate(0, 50, 0);
	ofScale(40.f);
	ofRotateYDeg(-3 * 45.f);

	// Model transforms
	// Settled when loading the model,
	// but requires apply here before draw!
	ofScale(model.getScale().x, model.getScale().y, model.getScale().z);

	//TODO:  would need to pass as argument like the above scale..
	ofTranslate(model.getPosition().x, model.getPosition().y, model.getPosition().z);
	#endif

	// Draw all the model queued meshes
	{
		if (meshesModel.size() > 0) {
			for (int i = 0; i < meshesModel.size(); i++) {
				meshesModel[i].drawFaces();
			}
		}
	}

	#if (USE_MODEL_TRANSFORMS)
	ofPopMatrix();
	#endif
}
#endif