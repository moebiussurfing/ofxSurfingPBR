/*

	This example shows how to illuminate a loaded FBX model  
	with their PBR textures.  
	Also, we have a data folder with some parts of the model  
	that we exported separately manually  
	from Blender, Cinema4D, or any others.  

	We will use the `SurfingSceneManager.h` class  
	to customize the scene.  
	This class auto-creates a combo of the four default lights,  
	to perform the light illumination,  
	and allows queuing materials and colors to our scene.  
	We will create a material for each part,  
	so we can tweak each PBR material color and the PBR settings  
	for each part independently.  

	We have also a rotating camera with height and longitude settings.  
	Also, we can apply one single material when using Draw OnePiece.  

*/


#pragma once

#include "ofMain.h"

#include "SurfingSceneManager.h"

#include "ofxAssimpModelLoader.h"
#include "ofxCameraSaveLoad.h"

//--------------------------------------------------------------
class ofApp : public ofBaseApp {
private:
	const string pathModel_DEFAULT = "models\\nike\\original\\nike1.fbx";//original textured model
	const string pathParts_DEFAULT = "models\\nike\\parts";//must contain separated parts without textures

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

	void setupScene();
	void drawScene();
	void renderScene();
	callback_t f_RenderScene = nullptr;

public:
	void setupGui();
	void drawGui();
	ofxPanel gui;
	ofParameter<bool> bGui;

public:
	void beginCamera();
	void endCamera();

	//--

public:
	SurfingSceneManager sceneManager;

	//--

public:
	ofParameterGroup parameters { "ofApp" };

	ofParameterGroup panelsParams { "Panels" };
	ofParameterGroup drawModeParams { "Mode" };
	ofParameterGroup drawParams { "Draw" };
	ofParameterGroup cameraParams { "Camera" };
	ofParameterGroup paletteParams { "Palette" };

private:
	string pathModel;
	string nameModel;

	// Original model is a more raw object
	// with only one material ad color.
	void loadModelOriginalMeshed();
	void loadModelOriginal();

private:
	void loadModelParts();

	vector<std::unique_ptr<ofxAssimpModelLoader>> models;
	vector<string> namesModels;

	ofxAssimpModelLoader modelOriginal;
	SurfingMaterial materialOnePiece;

	//--

	ofParameter<bool> bDrawOriginal { "Draw Original", false };
	ofParameter<bool> bDrawOnePiece { "Draw OnePiece", false };
	ofParameter<bool> bDrawParts = { "Draw Parts", false };
	ofParameter<bool> bDrawTestBox { "Draw TestBox", false };

private:
	void setupCamera();
	ofEasyCam camera;
	float rotationLong;
	string path_CameraSettings = "CameraSettings.txt";
	ofParameter<bool> bMouseCamera { "Mouse Camera", false };
	ofParameter<void> vResetCam { "Reset Camera" };
	ofParameter<bool> bAnimRotate { "Rotate Anim", true };
	ofParameter<float> rotateSpeed { "Rotate Speed", 0.5, 0, 1 };
	void doResetCamera();

	void pushTransforms();
	void popTransforms();

	ofParameterGroup transformsParams { "Transforms" };
	ofParameter<float> yRotate { "y Rotate", 0, -180, 180 };
	ofParameter<float> yPos { "Pos y", 0, -1.f, 1.f };
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<void> vResetTransforms { "Reset" };
	ofEventListener listenerReset;
	void doResetTransforms();

	ofParameter<bool> bDrawGrid { "Draw Grid", true };
	void drawGrid();

	//--

private:
	void Changed(ofAbstractParameter & e);
	void ChangedPalette(ofAbstractParameter & e);
	void ChangedDraw(ofAbstractParameter & e);

	void doRefreshColorsOriginal();
	void doRandomPalette();

	//--

	vector<ofParameter<ofFloatColor>> palette;

	// Meshes
	ofDirectory dir;
	ofMesh mesh;
	vector<ofVboMesh> meshesModelOriginal;
	vector<vector<ofVboMesh>> meshesParts;

	void load();
	void save();

	void drawHelp();
	void buildHelp(); //refresh help info to display updated
	ofParameter<bool> bHelp { "Help", false };
	string sHelp;
};