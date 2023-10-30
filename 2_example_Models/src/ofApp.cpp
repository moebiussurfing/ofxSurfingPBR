#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef OF_APP_DEFINED_ofxSurfingHelpers
	ofxSurfingHelpers::setWindowTitleAsProjectName();
	ofxSurfingHelpers::setMonitorsLayout(-1, false, true);
#endif

	camera.setFarClip(10000);

	//--

	pbr.setup();
	pbr.setCameraPtr(&camera);

	pbr.loadCubeMap("cubemaps/kloppenheim_06_puresky_1k.exr");

	// render scene
	callback_t myFunctionDraw = std::bind(&ofApp::renderScene, this);
	pbr.setFunction_renderScene(myFunctionDraw);

	//--

	// models
	setupModels();
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
bool ofApp::loadModel(string path) {
	pathModel = path;

	ofLogNotice() << "Started loading model file... " << path;

	// model
	//model = ofxAssimpModelLoader();
	model.clear();

	bool b = model.load(pathModel, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);
	if (b)
		ofLogNotice() << "Loading successful";
	else
		ofLogError() << "File not found!";

	//model.setPosition(0, 2, 0);
	//model.setRotation(0, 90, 1, 0, 0);

	// mesh
	//meshModel.clear();
	//meshModel = ofVboMesh();
	//meshModel = model.getMesh(0);

	//// flip the normals
	//for (size_t i = 0; i < meshModel.getNumNormals(); i++) {
	//	meshModel.getNormals()[i] *= -1.f;
	//}
	
	// TODO: would be nice to queue multi meshes
	// bc some models have multiple..
	meshesModel.clear();
	int sz = model.getMeshCount();
	for (int i = 0; i < sz; i++) {
		//meshModel = model.getMesh(i);
		meshesModel.push_back(model.getMesh(i));
	}

	return b;
}

//--------------------------------------------------------------
void ofApp::setupModels() {

	// mesh
	// (scene 0)
	pathMesh = "models/ofLogoHollow.ply";
	mesh.load(pathMesh);
	mesh.mergeDuplicateVertices();
	// flip the normals
	for (size_t i = 0; i < mesh.getNumNormals(); i++) {
		mesh.getNormals()[i] *= -1.f;
	}

	//--

	// model
	// (scene 1)
	pathModel = "models/head25k.obj"; //problems with normals
	//pathModel = "models/basic_form.ply";
	loadModel(pathModel);

	//--

#ifdef SURFING__USE_FILE_BROWSER
	// models folder
	setupDir();

	fileName.setSerializable(false);

	eIndexFile = indexFile.newListener([this](int & i) {
		static int i_ = -1;
		if (i != i_) {//changed
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

	gui.setup("ofApp_Model");
	gui.add(scene);

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
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	pbr.draw();

	pbr.drawGui();

	int pad = 5;
	glm::vec2 p = glm::vec2(ofGetWindowWidth(), ofGetHeight()) - glm::vec2(gui.getShape().getWidth() + pad, gui.getShape().getHeight() + pad);
	gui.setPosition(p.x, p.y);
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	// floor plane
	pbr.drawPlane();

	// objects
	pbr.beginMaterial();
	{
		drawMyScene();
	}
	pbr.endMaterial();
}

//--------------------------------------------------------------
void ofApp::drawMyScene() {

	// transforms

	float uPos = 10;
	float y = ofMap(yPos, -1.f, 1.f, -uPos, uPos, true);

	float uScale = 200;
	float s = ofMap(scale, -1.f, 1.f, 10, uScale, true);

	int st = ofMap(speed, 0.f, 1.f, 60 * 30, 60, true);
	float d = ofMap(ofGetFrameNum() % st, 0.f, st, 0.f, 360.f);

	//--

	// mesh
	if (scene == 0) {
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

	// mesh model
	else if (scene == 1) {
		ofPushMatrix();
		ofScale(s);
		ofTranslate(0, y, 0);
		//ofTranslate(model.getPosition().x, model.getPosition().y, model.getPosition().z);
		if (bRotate) ofRotateYDeg(-d);
		{
			//meshModel.drawFaces();
			if (meshesModel.size() > 0) {
				for (int i = 0; i < meshesModel.size(); i++) {
					meshesModel[i].drawFaces();
				}
			} 
		}
		ofPopMatrix();
	}

	//--

	// prims
	else if (scene == 2) {
		ofPushMatrix();
		float uuPos = 800;
		float yy = ofMap(yPos, -1.f, 1.f, -uuPos, uuPos, true);
		ofTranslate(0, yy, 0);
		ofScale(s * 0.02f);
		if (bRotate) ofRotateYDeg(d);

		{
			ofPushMatrix();
			ofTranslate(-200, 100, 0);
			ofRotateXDeg(180);
			ofDrawCone(0, 0, 0, 65, 100);
			ofPopMatrix();

			ofPushMatrix();
			float spd = 240;
			ofRotateYDeg(360.f * (ofGetFrameNum() % (int)spd) / spd);
			ofDrawBox(0, 100, 0, 100);
			ofPopMatrix();

			ofDrawSphere(200, 100, 0, 50);
		}

		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'r') bRotate = !bRotate;

	if (key == 'd') pbr.bDebug = !pbr.bDebug;
	if (key == 'p') pbr.bDrawPlane = !pbr.bDrawPlane;
	if (key == 'c') pbr.bDrawCubeMap = !pbr.bDrawCubeMap;

	if (key == OF_KEY_F1) pbr.doRandomMaterialColors();
	if (key == OF_KEY_F2) pbr.doRandomMaterialSettings();
	if (key == OF_KEY_F3) pbr.doRandomMaterial();
	if (key == OF_KEY_F4) pbr.doResetMaterial();

#ifdef SURFING__USE_FILE_BROWSER
	if (key == OF_KEY_RIGHT) {
		if (dir.size() > 0) {
			indexFile.setWithoutEventNotifications(indexFile.get() + 1);
			indexFile.set(indexFile % dir.size());
		}
	}
#endif

	if (key == ' ') {
		if (scene < scene.getMax())
			scene = scene + 1;
		else
			scene = 0;
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	pbr.exit();
	gui.saveToFile("ofApp.json");
}