#pragma once

#include "ofMain.h"

#include "SurfingSceneManager.h"
#include "ofxAssimpModelLoader.h"
#include "ofxCameraSaveLoad.h"

//--------------------------------------------------------------
class ofApp : public ofBaseApp {
public:
	void setup();
	void setupParams();
	void update();
	void draw();
	void exit();
	void startup();
	void setupScene();
	void keyPressed(int key);

public:
	void drawScene();
	void drawObjects();
	
public:
	void drawGui();
	ofxPanel gui;

public:
	void beginLights();
	void endLights();

	void beginCam();
	void endCam();

	//--

public:
	SurfingSceneManager sceneManager;

	//--

public:
	ofParameterGroup parameters { "ofApp" };

	ofParameterGroup sceneParams { "Scene" };
	ofParameterGroup panelsParams { "Panels" };
	ofParameterGroup sceneExtraParams { "SceneExtra" };
	ofParameterGroup originalParams { "Original" };
	ofParameterGroup objectsParams { "Objects" };
	ofParameterGroup cameraParams { "Camera" };
	ofParameterGroup paletteParams { "Palette" };

private:
	void loadModelParts();

	ofxAssimpModelLoader model;

	vector<std::unique_ptr<ofxAssimpModelLoader>> models;

	ofxAssimpModelLoader modelOriginal;
	ofMaterial materialOriginal;
	ofParameter<float> shininessOriginal { "Shininess Original", 0, 0, 1 };
	ofParameter<ofFloatColor> colorOriginal { "Color Original", ofFloatColor(), ofFloatColor(0), ofFloatColor(1) };

	//--

private:
	ofEasyCam cam;
	float cameraOrbit;
	string path_CameraSettings = "CameraSettings.txt";

	ofParameter<bool> bDrawGrid { "Draw Grid", true };
	ofParameter<bool> bDrawTestBox { "Test Box", false };

	ofParameter<bool> bDrawOriginal { "Original", true };
	ofParameter<bool> bDrawShoeParts = { "Colored Parts", true };

	ofParameter<bool> bMouseCam { "Mouse Cam", false };
	ofParameter<void> vResetCam { "Reset Cam"};
	ofParameter<bool> bRotate { "Rotate", true };
	ofParameter<float> speedRotate { "Speed", 0.5, 0, 1 };

	//ofParameter<bool> bParts = { "Parts", true };
	//ofParameter<bool> bLights = { "Lights", true };
	//ofParameter<bool> bMaterials = { "Materials", true };
	//ofParameter<int> index { "index", 0, 0, 0 };

	void drawGrid();

	//--

private:
	void ChangedParameters(ofAbstractParameter & e);
	void ChangedPaletteParams(ofAbstractParameter & e);

	string path_Model;

	string sNameModel;

	// Original model is a more raw object
	// with only one material ad color.
	void loadModelOriginal();
	void loadModelSimple();
	void doRefreshColorsOriginal();
	void doRandomPalette();

	//--

	vector<ofParameter<ofFloatColor>> palette;

	// meshes
	ofDirectory dir;
	ofMesh mesh;
	vector<ofMesh> meshesModelOriginal;
	vector<vector<ofMesh>> meshesParts;

	void load();
	void save();
};