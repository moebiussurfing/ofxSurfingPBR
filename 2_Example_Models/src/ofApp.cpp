#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// Theme
	if (1) ofxSurfing::setOfxGuiTheme(); // Customize ofxGui theme.

	// App window
	{
		ofxSurfing::setWindowTitleAsProjectName(); // Name the window app.
#if 1
		// Move and shape the window app.
		// Customize settings too: 60fps and vSync off.
		//ofxSurfing::setWindowAtMonitor(); // Stay at main display (center in my setup) and landscape.
		ofxSurfing::setWindowAtMonitor(-1); // Move to left display and set landscape.
		//ofxSurfing::setWindowAtMonitor(1, true); // Move to right display and set portrait.
#endif
	}

	//--

	setupPBR();

	//--

	// Scene

	setupMesh();

	setupModel();

#ifdef SURFING__USE__FILE_BROWSER
	setupModelsBrowser();
#endif

	//--

	setupParams();
}

//--------------------------------------------------------------
void ofApp::setupPBR() {
	pbr.setup();

	// Pass the camera pointer
	pbr.setCameraPtr(&camera);

	// Pass the render scene
	callback_t f = std::bind(&ofApp::renderScene, this);
	pbr.setFunctionRenderScene(f);

	//--

	// Optional

#if (USE_OPTIONAL_SETUP && 1)
	// Set log level
	pbr.setLogLevel(OF_LOG_VERBOSE);
#endif

#if (USE_OPTIONAL_SETUP && 1)
	// Check if it's the first time opening the App.
	// If not, we will force some settings for the scene:
	if (!pbr.getSettingsFileFound()) {
		ofLogWarning() << "Settings file for ofxSurfingPBR not found!";
		ofLogWarning() << "Forcing the ofxSurfingPBR initial scene.";

		// Background alt
		pbr.bDrawBg.set(true);
		pbr.bgColor.set(ofFloatColor(0, 0.03, 0.3, 1));

		// Plane
		pbr.planeGlobalColor.set(ofFloatColor(0.25, 0, 0.5, 1));

		// Material
		pbr.material.roughness = 0.5;
		pbr.material.reflectance = 0.5;
		pbr.material.globalColor.set(ofFloatColor::orange);

		bHelp = true;
	}
#endif

#if (USE_OPTIONAL_SETUP && 0)
	#ifdef SURFING__USE_CUBE_MAP
	// Force replace the default cubemap
	pbr.loadCubeMap("cubemaps/kloppenheim_06_puresky_1k.exr");
	#endif
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

	parameters.add(yPos);
	parameters.add(scale);
	parameters.add(speed);
	parameters.add(bRotate);

	parameters.add(bHelp);
	parameters.add(vReset);

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

	//--

	// Gui

	gui.setup(parameters);

	refreshGui();
	buildHelp();

	//--

	// Startup

	vReset.trigger(); //before loading settings or in case not settings file / app open for the first time.

	ofxSurfing::loadSettings(parameters, path);
}

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
	sHelp += "\n";
	sHelp += "LEFT  Prev\n";
	sHelp += "RIGHT Next\n";
	sHelp += "#";
	sHelp += ofToString(indexScene);
	sHelp += "\n";
	sHelp += "\n";
	sHelp += "SCENE\n";
	sHelp += namesScenes[indexScene];
}

//--------------------------------------------------------------
void ofApp::refreshGui() {
	// Center visible gui panels at the window bottom.
#ifdef SURFING__USE__FILE_BROWSER
	if (indexScene == 2) {
		ofxSurfing::setGuiPositionToLayoutBoth(gui, guiModels);
	} else
		ofxSurfing::setGuiPositionToLayout(gui); //move gui to bottom-center.
#else
	ofxSurfing::setGuiPositionToLayout(gui); //move gui to bottom-center.
#endif
}

//--------------------------------------------------------------
void ofApp::setupMesh() {

	// For scene 1

	pathMesh = "models/ofLogoHollow.ply";
	mesh.load(pathMesh);
	mesh.mergeDuplicateVertices();

	// flip the normals
	for (size_t i = 0; i < mesh.getNumNormals(); i++) {
		mesh.getNormals()[i] *= -1.f;
	}
}

//--------------------------------------------------------------
void ofApp::drawMesh() {
	// Transforms for the ofLogoHollow.ply mesh.
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
void ofApp::setupModel() {

	// For scene 2

	float scaled = 1.0f;

	//--

	pathModel = "models/head25k.obj"; // This file model have problems with normals...

	//pathModel = "models/basic_form.ply";
	//scaled = 2.0;

	//pathModel = "models/ultraviolence.fbx";
	//scaled = 0.01;

	//--

	bool b = loadModel(pathModel, scaled);
	if (b)
		ofLogNotice() << "Successfully loaded model file: " << pathModel;
	else
		ofLogError() << "Unable to load model file " << pathModel << ". Maybe not found!";
}

#ifdef SURFING__USE__FILE_BROWSER
//--------------------------------------------------------------
void ofApp::setupModelsBrowser() {

	surfingModels.setup();

	guiModels.setup(surfingModels.parameters);

	// Callback to trig the model file loading.
	// The model path is ready on surfingModels.pathModel!
	listenerLoadModel = surfingModels.loadBang.newListener([this](void) {
		string path = surfingModels.pathModel;
		this->loadModel(path);
	});

	listenerIndexModel = surfingModels.indexFile.newListener([this](int & i) {
		buildHelp(); //refresh help info
	});
}
#endif

//--------------------------------------------------------------
bool ofApp::loadModel(string path, float scaled) {

	pathModel = path;

	ofLogNotice() << "loadModel() Trying to load model file " << path;

	model.clear();

	bool b = model.load(pathModel, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);

	if (b) {
		ofLogNotice() << "Loading successful";

		// Model transforms
		// Then must be applied before draw!
		float s = scaled;
		model.setScale(s, s, s);
		ofLogNotice() << "Model scaled by:" << s;

		//model.setPosition(0, 2, 0);//TODO
		//model.setRotation(0, 90, 1, 0, 0);//TODO

		//--

		// Add meshes to the vector of ofVboMesh's.
		// We queue multi meshes,
		// bc some models could have multiple parts.
		meshesModel.clear();
		for (int i = 0; i < model.getMeshCount(); i++) {
			meshesModel.push_back(model.getMesh(i));
		}

		if (meshesModel.size() > 0)
			ofLogNotice() << "Queued " << meshesModel.size() << " meshes into meshesModel vector<ofVboMesh>.";
	}

	else {
		ofLogError() << "File " << pathModel << " not found or failed!";
	}

	return b;
}

//--------------------------------------------------------------
void ofApp::update() {

#if 0
	// DEBUG cam
	if (ofGetFrameNum() % 60 == 0) {
		cout << "dist:" << pbr.getOfEasyCamPtr()->getDistance() << endl;
		cout << "pos:" << pbr.getOfEasyCamPtr()->getPosition() << endl;
	}
#endif

#if 0

	// Shows how to access the internal camera.
	// Make some animation to the distance.
	float t = ofGetElapsedTimef();
	float v = glm::sin(2.0f * glm::pi<float>() * t);
	v = ofMap(v, -1, 1, 700, 4000, true);
	pbr.getOfEasyCamPtr()->setDistance(v);
	//camera.setDistance(v);//will does the same than above line.
#endif
}

//--------------------------------------------------------------
void ofApp::drawModel() {
	glFrontFace(GL_CCW); //fix for "transparent" for model head25k.obj with normals problems..

#define USE_MODEL_TRANSFORMS 1

#if (USE_MODEL_TRANSFORMS)
	// Scene transforms
	ofPushMatrix();
	ofTranslate(0, 100, 0);
	ofScale(50.f);

	// Model transforms
	// Settled when loading the model,
	// but requires apply here before draw!
	ofScale(model.getScale().x, model.getScale().y, model.getScale().z);
	//ofTranslate(model.getPosition().x, model.getPosition().y, model.getPosition().z);//TODO
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

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	if (!pbr.bGui) return; //used as global show ui.

	pbr.drawGui();

	//--

	if (pbr.bGui_ofxGui) {
		gui.draw();

#ifdef SURFING__USE__FILE_BROWSER
		if (indexScene == 2) {
			auto bb = gui.getShape();
			guiModels.setPosition(bb.getTopRight() + glm::vec2 { (float)SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0.f });
			guiModels.draw();
		}
#endif
	}

	//--

	drawHelp();
}

//--------------------------------------------------------------
void ofApp::drawHelp() {
	if (bHelp) ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT);

#ifdef SURFING__USE__FILE_BROWSER
	if (indexScene == 2) {
		surfingModels.drawHelp();
	}
#endif
}

//--------------------------------------------------------------
void ofApp::renderScene() {

	// Plane / floor
	pbr.drawPlane();

	// Other objects
	pbr.beginMaterial();
	{
		drawMyScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawMyScene() {

#define DO_SCENE_TRANSFORMS 1

#if (DO_SCENE_TRANSFORMS)
	// Scene transforms
	ofPushMatrix();

	// Position
	float yUnit = SURFING__SCENE_SIZE_UNIT / 2.f;
	float y = ofMap(yPos, -1.f, 1.f,
		-yUnit, yUnit, true);

	// Scale
	float s = ofMap(scale, -1.f, 1.f,
		1.f / SURFING__SCENE_TEST_UNIT_SCALE, SURFING__SCENE_TEST_UNIT_SCALE, true);

	// Rotation
	int tmax = 30; //30 seconds to do 360deg at 60 fps, for the slower speed. faster speed is one second per 360deg.
	int f = ofMap(speed, 0.f, 1.f, 60 * tmax, 60, true);
	float d = ofMap(ofGetFrameNum() % f, 0, f, 0.f, 360.f);

	ofTranslate(0, y, 0);
	ofScale(s);
	if (bRotate) ofRotateYDeg(d);
#endif

	{
		//--

		// Scene 0
		// Three Prims

		if (indexScene == 0) {
			pbr.drawTestScene();
		}

		//--

		// Scene 1
		// Mesh

		if (indexScene == 1) {
			drawMesh();
		}

		//--

		// Scene 2
		// Model(s)

		else if (indexScene == 2) {
			drawModel();
		}
	}

#if DO_SCENE_TRANSFORMS
	ofPopMatrix();
#endif
}

//--------------------------------------------------------------
void ofApp::doReset() {
	// Scene
	scale = -0.6;
	yPos = 0;
	speed = 0.5;
	bRotate = false;

	//indexScene = 0;

#if 1
	// PBR stuff to reset:

	// Reset the camera only.
	pbr.doResetCamera();

	//// Reset all PBR, including camera.
	//pbr.doResetAll();
#endif
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

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'R') bRotate = !bRotate;
	if (key == 'H') bHelp = !bHelp;

	if (key == OF_KEY_RIGHT) {
		doNextScene();
	} else if (key == OF_KEY_LEFT) {
		doPrevScene();
	}

#ifdef SURFING__USE__FILE_BROWSER
	else if (key == OF_KEY_DOWN) {
		surfingModels.next();
	} else if (key == OF_KEY_UP) {
		surfingModels.previous();
	}
#endif

	//--

	pbr.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::windowResized(ofResizeEventArgs & resize) {
	int h = resize.height;
	int w = resize.width;
	glm::vec2 sz { w, h };
	static glm::vec2 sz_ { -1, -1 };
	if (sz != sz_) {
		sz_ = sz;
		ofLogNotice("ofApp") << "windowResized() Size: " << w << "," << h;

		refreshGui(); //refresh gui collapse folders
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();

	ofxSurfing::saveSettings(parameters, path);
}