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
	SurfingSceneManager scenePBR;

	//--

public:
	ofParameterGroup parameters { "ofApp" };

	ofParameterGroup params_Scene { "Scene" };
	ofParameterGroup params_Panels { "Panels" };
	ofParameterGroup params_SceneExtra { "SceneExtra" };
	ofParameterGroup params_SimpleOriginal { "Simple Og" };
	ofParameterGroup params_Objects { "Objects" };
	ofParameterGroup params_Camera { "Camera" };
	ofParameterGroup params_Palette { "Palette" };

private:
	void loadModelFiles();

	ofxAssimpModelLoader model;
	vector<std::unique_ptr<ofxAssimpModelLoader>> models;

	ofxAssimpModelLoader modelOriginal;
	ofMaterial materialOriginal;
	ofParameter<float> shininessOriginal { "Shin Og", 120, 0, 120 };
	ofParameter<ofFloatColor> colorOriginal { "Color Og", ofColor(), ofColor(0), ofColor(1) };

	//--

private:
	ofEasyCam cam;
	float cameraOrbit;
	string path_CamSettings = "Camera_Settings.ini";

	ofParameter<bool> bTestBox { "Test Box", false };
	ofParameter<bool> bGrid { "Grid", false };
	ofParameter<bool> bMouseCam { "MouseCam", false };
	ofParameter<bool> bResetCam { "Reset Cam", false };
	ofParameter<bool> bRotate { "Rotate", true };
	ofParameter<bool> bInCam { "In Cam", true };
	ofParameter<float> speedRotate { "Speed", 0.5, 0, 1 };
	ofParameter<bool> bSimpleOriginal { "Simple Og", false };
	ofParameter<bool> bShoeParts = { "Colored Nike", true };
	ofParameter<bool> bParts = { "Parts", true };
	ofParameter<bool> bLights = { "Lights", true };
	ofParameter<bool> bMaterials = { "Materials", true };
	ofParameter<int> index { "index", 0, 0, 0 };
	ofParameter<bool> bMouseLight { "Mouse Light", false };
	ofParameter<float> mx { "mouseX", 0, 0, 1920 };
	ofParameter<float> my { "mouseY", 0, 0, 1080 };

	//--

private:
	void ChangedParameters(ofAbstractParameter & e);
	void ChangedPalette(ofAbstractParameter & e);

	string path_Objects;

	string curFileInfo;
	bool bHelpText;

	// Original model is a more raw object
	// with only one material ad color.
	void loadModelOriginal();
	void doRefreshColorsOriginal();
	void doRandomPalette();

	//--

	vector<ofParameter<ofFloatColor>> palette;

	// meshes
	ofDirectory dir;
	vector<vector<ofMesh>> meshParts;
	ofMesh mesh;
	vector<ofMesh> meshesOriginal;

	void load();
	void save();
};