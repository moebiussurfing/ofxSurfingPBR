#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window

#if 1
	//ofxSurfing::setWindowAtMonitor(); // Center monitor / landscape.
	ofxSurfing::setWindowAtMonitor(-1); // Move to left monitor / landscape.
	//ofxSurfing::setWindowAtMonitor(1,true); // Move to right monitor / portrait.
#endif

	//--

	setupPBR();

	//--

	setupMesh();

	setupModel();

	//--

#ifdef SURFING__USE_FILE_BROWSER
	surfingModels.setup();

	// Callback to trig the model file loading.
	// The model path is ready on surfingModels.pathModel!
	listenerLoadModel = surfingModels.loadBang.newListener([this](void) {
		string path = surfingModels.pathModel;
		this->loadModel(path);
	});

	listenerIndexModel = surfingModels.indexFile.newListener([this](int & i) {
		buildHelp(); //refresh help info
		refreshGui(); //refresh gui collase folders
	});

#endif

	//--

	setupParams();
}

//--------------------------------------------------------------
void ofApp::setupPBR() {
	pbr.setup();
	pbr.setCameraPtr(&camera);

	//--

	// Optional

#if 1
	// Set log level
	pbr.setLogLevel(OF_LOG_VERBOSE);
#endif

#if 0
	//TODO:
	// Check if it's the first time opening the App
	if (!pbr.getSettingsFileFound()) {
		ofLogWarning() << "ofxSurfingPBR Settings file not found!";

		// must fix crashing on callback..
		// Force startup settings
		pbr.bDrawBgAlt.set(true);
		pbr.bgAltColor.setWithoutEventNotifications(ofFloatColor::orange);
		pbr.planeGlobalColor.setWithoutEventNotifications(ofFloatColor::red);
		pbr.material.globalColor.setWithoutEventNotifications(ofFloatColor::yellow);
	}
#endif

#if 0
	// Force replace the default cubemap
	pbr.loadCubeMap("cubemaps/kloppenheim_06_puresky_1k.exr");
#endif

	// Render scene
	callback_t f = std::bind(&ofApp::renderScene, this);
	pbr.setFunctionRenderScene(f);
}

//--------------------------------------------------------------
void ofApp::setupParams() {

	parameters.setName("ofApp");
	parameters.add(yPos);
	parameters.add(scale);
	parameters.add(next);
	parameters.add(indexScene);

#ifdef SURFING__USE_FILE_BROWSER
	parameters.add(surfingModels.parameters);
#endif

	parameters.add(speed);
	parameters.add(bRotate);
	parameters.add(bHelp);
	parameters.add(reset);

	gui.setup(parameters);

	ofxSurfing::setGuiPositionToLayout(gui);

#ifdef SURFING__USE_FILE_BROWSER
	gui.getGroup(surfingModels.parameters.getName()).minimize();
#endif

	//--

	// Callbacks

	listenerNext = next.newListener([this](void) {
		nextScene();
	});

	listenerReset = reset.newListener([this](void) {
		scale = 0;
		yPos = 0;
		speed = 0.5;
	});

	listenerIndexScene = indexScene.newListener([this](int & i) {
		buildHelp();
		refreshGui();
	});

	buildHelp();

	//--

	//gui.loadFromFile(path);
	ofxSurfing::loadSettings(parameters, path);
}

//--------------------------------------------------------------
void ofApp::buildHelp() {

	sHelp = "";
	sHelp += "\n";
	sHelp += "HELP\n";
	sHelp += "ofApp\n";
	sHelp += "\n";
	sHelp += "H Help\n";
	sHelp += "g Gui\n";
	sHelp += "G ofxGui\n";
	sHelp += "R Rotate\n";
	sHelp += "\n";

	sHelp += "SPACE\n  Next Scene\n";
	sHelp += "\n";

	switch (indexScene) {
	case 0:
		sHelp += "SCENE\n#0 \nTEST SCENE";
		break;
	case 1:
		sHelp += "SCENE\n#1 \nMESH";
		break;
	case 2:
		sHelp += "SCENE\n#2 \nMODEL";
#ifdef SURFING__USE_FILE_BROWSER
		sHelp += "S";
#endif
		break;
	}
	sHelp += "\n\n";

#ifdef SURFING__USE_FILE_BROWSER
	if (indexScene == 2) {
		sHelp += "BROWSE\n";
		sHelp += "  UP    Previous\n";
		sHelp += "  DOWN  Next\n";
		sHelp += "\n";

		sHelp += surfingModels.getFilename();
		sHelp += "\n\n";

		sHelp += surfingModels.getFilenamesList();
	}
#endif

	sHelp += " \n";
	//sHelp += " \n";
}

//--------------------------------------------------------------
void ofApp::refreshGui() {
#if 0
	#ifdef SURFING__USE_FILE_BROWSER
	if (indexScene == 2)
		gui.getGroup(surfingModels.parameters.getName()).maximize();
	else
		gui.getGroup(surfingModels.parameters.getName()).minimize();
	#endif
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
void ofApp::drawModel() {

	// Draw all the meshes
	if (meshesModel.size() > 0) {
		for (int i = 0; i < meshesModel.size(); i++) {
			meshesModel[i].drawFaces();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	if (!pbr.bGui) return; //used as global show gui

	pbr.drawGui();

	//--

	if (bHelp) {
		//ofxSurfing::ofDrawBitmapStringBox(sHelp); //centered

		if (!pbr.isVisibleDebugShader())
			ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
		else
			ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT);

		//ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_CENTER); //top centered

		//TODO: fix to put above gui panel
		// not sure if there is some bug on OF getBoundingBox..
		//auto bbG = gui.getShape();
		//auto bbH = ofxSurfing::getBBBitmapStringBox(sHelp);
		//int x = bbG.getTopLeft().x + bbG.getWidth() / 2;
		//int y = bbG.getTopLeft().y - bbG.getHeight();
		//x -= bbH.getWidth() / 2;
		//y -= 20;
		//glm::vec2 p { x, y };
		//ofxSurfing::ofDrawBitmapStringBox(sHelp, p.x, p.y);
	}

	if (pbr.bGui_ofxGui) gui.draw();
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

	// Scene transforms

	float yPosUnit = 10;
	float y = ofMap(yPos, -1.f, 1.f, -yPosUnit, yPosUnit, true);

	float scaleUnit = 200;
	float s = ofMap(scale, -1.f, 1.f, 10.f, scaleUnit, true);

	int f = ofMap(speed, 0.f, 1.f, 60 * 30, 60, true);
	float d = ofMap(ofGetFrameNum() % f, 0.f, f, 0.f, 360.f);

	//--

	// Scene 0
	// Three Prims

	if (indexScene == 0) {

		ofPushMatrix();
		float yPosUnit = 800;
		float y = ofMap(yPos, -1.f, 1.f, -yPosUnit, yPosUnit, true);
		ofTranslate(0, y, 0);
		ofScale(s * 0.02f);

		if (bRotate) ofRotateYDeg(d);
		{
			pbr.drawTestScene();
		}

		ofPopMatrix();
	}

	//--

	// Scene 1
	// Mesh

	if (indexScene == 1) {

		ofPushMatrix();
		ofScale(s);
		ofTranslate(0, y, 0);
		ofRotateXDeg(-90);

		if (bRotate) ofRotateZDeg(d);
		{
			mesh.draw();
		}

		ofPopMatrix();
	}

	//--

	// Scene 2
	// Model

	else if (indexScene == 2) {
		glFrontFace(GL_CCW); //fix for "transparent" for model head25k.obj with normals problems..

		ofPushMatrix();

		// Scene transforms
		ofScale(s * 2.0f);
		ofTranslate(0, y, 0);

		// Model transforms
		// Settled when loading the model,
		// but requires apply here before draw!
		ofScale(model.getScale().x, model.getScale().y, model.getScale().z);
		//ofTranslate(model.getPosition().x, model.getPosition().y, model.getPosition().z);//TODO

		if (bRotate) ofRotateYDeg(-d);
		{
			drawModel();
		}

		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'R') bRotate = !bRotate;
	if (key == 'H') bHelp = !bHelp;

	if (key == ' ') {
		nextScene();
	}

#ifdef SURFING__USE_FILE_BROWSER
	if (key == OF_KEY_DOWN) {
		surfingModels.next();
	}

	if (key == OF_KEY_UP) {
		surfingModels.previous();
	}
#endif

	//--

	pbr.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::nextScene() {
	if (indexScene < indexScene.getMax())
		indexScene = indexScene + 1;
	else
		indexScene = 0;
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();

	//gui.saveToFile("ofApp.json");
	ofxSurfing::saveSettings(parameters, path);
}