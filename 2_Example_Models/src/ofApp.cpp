#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofxSurfing::setWindowTitleAsProjectName(); // Name the window

#if 1
	ofxSurfing::setWindowAtMonitor(); // Center monitor / landscape.
	//ofxSurfing::setWindowAtMonitor(-1); // Move to left monitor / landscape.
	//ofxSurfing::setWindowAtMonitor(1,true); // Move to right monitor / portrait.
#endif

	//--

	camera.setFarClip(10000);

	//--

	setupPBR();

	//--

	setupMesh();

	setupModel();

	//--

	// TODO: make a simple class to browse models.
#ifdef SURFING__USE_FILE_BROWSER
	// models folder
	setupDir();

	fileName.setSerializable(false);

	eIndexFile = indexFile.newListener([this](int & i) {
		static int i_ = -1;
		if (i != i_) { //changed
			i_ = i;
			if (i < dir.size()) {
				fileName = dir.getName(i);
				string path = dir.getPath(i);
				loadModel(path);
			}
		}
	});
#endif

	//--

	setupGui();
}

//--------------------------------------------------------------
void ofApp::setupPBR() {
	pbr.setup();
	pbr.setCameraPtr(&camera);

#if 1
	pbr.loadCubeMap("cubemaps/kloppenheim_06_puresky_1k.exr");
// replaces the default cubemap
#endif

	// render scene
	callback_t myFunctionDraw = std::bind(&ofApp::renderScene, this);
	pbr.setFunction_renderScene(myFunctionDraw);
}

//--------------------------------------------------------------
void ofApp::setupGui() {

	gui.setup("ofApp_Model");
	gui.add(indexScene);

#ifdef SURFING__USE_FILE_BROWSER
	gui.add(indexFile);
	gui.add(fileName);
#endif

	gui.add(scale);
	gui.add(yPos);
	gui.add(bRotate);
	gui.add(speed);
	gui.add(reset);

	eReset = reset.newListener([this](void) {
		scale = 0;
		yPos = 0;
		speed = 0.5;
	});

	gui.loadFromFile("ofApp.json");

	//--

	// move gui panel
	glm::vec2 p;
	int pad = 5; //to borders

	// to window bottom-right
#if 0
	{
		glm::vec2 p1 = glm::vec2(ofGetWindowWidth(), ofGetHeight());
		glm::vec2 p2 = glm::vec2(gui.getShape().getWidth() + pad, gui.getShape().getHeight() + pad);
		p = p1 - p2;
	}
	// to window bottom-left
#else
	{
		p = glm::vec2(pad, ofGetHeight() - gui.getShape().getHeight() - pad);
	}
#endif
	gui.setPosition(p.x, p.y);
}

//--------------------------------------------------------------
void ofApp::setupMesh() {
	// for scene 0

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
	// for scene 1

	pathModel = "models/head25k.obj"; // This file model have problems with normals
	//pathModel = "models/basic_form.ply";

	bool b = loadModel(pathModel);
	if (b)
		ofLogNotice() << "loaded model file: " << pathModel;
	else
		ofLogError() << "model file " << pathModel << " not found!";
}

//--------------------------------------------------------------
bool ofApp::loadModel(string path) {

	pathModel = path;

	ofLogNotice() << "Started loading model file... \n"
				  << path;

	model.clear();

	bool b = model.load(pathModel, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);

	if (b)
		ofLogNotice() << "Loading successful";
	else
		ofLogError() << "File not found!";

	// Transform
	//model.setPosition(0, 2, 0);
	//model.setRotation(0, 90, 1, 0, 0);

	// Create the vector of ofVboMesh's
	// We queue multi meshes
	// bc some models have multiple parts..
	meshesModel.clear();
	for (int i = 0; i < model.getMeshCount(); i++) {
		meshesModel.push_back(model.getMesh(i));
	}

	/*
	Modify the mesh. 
	Get example from \openFrameworks\examples\gl\materialPBR
	
	https://forum.openframeworks.cc/t/ofxassimpmodelloader-how-to-tweak-model-meshes/36665/4
	ofxAssimpMeshHelper& helper = model.getMeshHelper(i);
        ofMesh* mesh = &helper.cachedMesh;
	*/

	return b;
}

//--------------------------------------------------------------
void ofApp::drawModel() {

	// draw all the meshes
	if (meshesModel.size() > 0) {
		for (int i = 0; i < meshesModel.size(); i++) {
			meshesModel[i].drawFaces();
		}
	}
}

#ifdef SURFING__USE_FILE_BROWSER
//--------------------------------------------------------------
void ofApp::setupDir(string path) {
	if (path == "") path = "models/";

	dir.listDir(path);
	dir.allowExt("ply");
	dir.allowExt("fbx");
	dir.allowExt("obj");
	dir.sort();

	indexFile.setMax(dir.size() - 1);
}
#endif

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	pbr.drawGui();

	//--

	gui.draw();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// plane / floor
	pbr.drawPlane();

	// other objects
	pbr.beginMaterial();
	{
		drawMyScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawMyScene() {

	// Transforms

	float uPos = 10;
	float y = ofMap(yPos, -1.f, 1.f, -uPos, uPos, true);

	float uScale = 200;
	float s = ofMap(scale, -1.f, 1.f, 10, uScale, true);

	int st = ofMap(speed, 0.f, 1.f, 60 * 30, 60, true);
	float d = ofMap(ofGetFrameNum() % st, 0.f, st, 0.f, 360.f);

	//--

	// Scene 0
	// Mesh
	if (indexScene == 0) {
		// mesh
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

	// Scene 1
	// Model
	else if (indexScene == 1) {
		glFrontFace(GL_CCW); //fix for "transparent" model

		ofPushMatrix();
		ofScale(s * 2.f);
		y += 1;
		ofTranslate(0, y, 0);
		//ofTranslate(model.getPosition().x, model.getPosition().y, model.getPosition().z);
		if (bRotate) ofRotateYDeg(-d);
		{
			drawModel();
		}
		ofPopMatrix();
	}

	//--

	// Scene 2
	// Three prims
	else if (indexScene == 2) {

		ofPushMatrix();
		float uuPos = 800;
		float yy = ofMap(yPos, -1.f, 1.f, -uuPos, uuPos, true);
		ofTranslate(0, yy, 0);
		ofScale(s * 0.02f);
		if (bRotate) ofRotateYDeg(d);
		{
			pbr.drawTestScene();
		}
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'r') bRotate = !bRotate;

	if (key == ' ') {
		if (indexScene < indexScene.getMax())
			indexScene = indexScene + 1;
		else
			indexScene = 0;
	}

#ifdef SURFING__USE_FILE_BROWSER
	if (key == OF_KEY_RIGHT) {
		if (dir.size() > 0) {
			indexFile.setWithoutEventNotifications(indexFile.get() + 1);
			indexFile.set(indexFile % dir.size());
		}
	}
#endif

	pbr.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();

	gui.saveToFile("ofApp.json");
}