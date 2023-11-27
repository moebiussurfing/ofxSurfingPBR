#pragma once

#include "ofMain.h"

#include "SurfingSceneManager.h"

#include "ofxAssimpModelLoader.h"
#include "ofxCameraSaveLoad.h"

//--------------------------------------------------------------
class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);

	void setupParams();
	void startup();

public:
	void setupObjects();
	void drawObjects();

	void setupScene();
	void drawScene();
	
public:
	void setupGui();
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
	//ofParameterGroup extraParams { "Extra" };

	//ofParameterGroup originalParams { "Original" };
	ofParameterGroup drawParams { "Draw" };
	
	ofParameterGroup cameraParams { "Camera" };
	ofParameterGroup paletteParams { "Palette" };

private:
	void loadModelParts();

	ofxAssimpModelLoader model;

	vector<std::unique_ptr<ofxAssimpModelLoader>> models;
	
	vector<string> namesModels;

	ofxAssimpModelLoader modelOriginal;
	
	SurfingMaterial materialOriginal;

	//--

	ofParameter<bool> bDrawOriginal { "Draw Original", true };
	ofParameter<bool> bDrawParts = { "Draw Parts", false };
	ofParameter<bool> bDrawTestBox { "Draw TestBox", false };

private:
	void setupCamera();
	ofEasyCam cam;
	float cameraOrbit;
	string path_CameraSettings = "CameraSettings.txt";
	ofParameter<bool> bMouseCam { "Mouse Cam", false };
	ofParameter<void> vResetCam { "Reset Cam"};
	ofParameter<bool> bRotate { "Rotate", true };
	ofParameter<float> speedRotate { "Speed", 0.5, 0, 1 };
	ofParameter<bool> bDrawGrid { "Draw Grid", true };
	void drawGrid();

	//--

private:
	void Changed(ofAbstractParameter & e);
	void ChangedPalette(ofAbstractParameter & e);
	void ChangedDraw(ofAbstractParameter & e);

	string pathModel;
	string nameModel;

	// Original model is a more raw object
	// with only one material ad color.
	void loadModelOriginalMeshed();
	void loadModelOriginalSimple();

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