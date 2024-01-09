/*

This example uses the main class from the `ofxSurfingPBR.h` addon:
ofxSurfingPBR
The above class bundles almost all the modules/features from the addon. 
Many modules can be used also as standalone, for other simpler and custom OF scenes.

For rendering some 3d meshes,
We are using also the `SurfingSceneManager.h` helper class, 
who uses also `SurfingFilesBrowserModels.h`.
These classes are `3d models` helpers paired with the addon.
With these classes we can easily browse the `3d models` files, 
directly from our app `bin/data/` path.

As we are using the full addon modules from `ofxSurfingPBR.h`,
we can render the objects using the bundled materials, 
the floor, all the `4 lights kit`, the background generators and the cubeMap.

*/

#pragma once

#include "ofMain.h"

//--

// OPTIONAL:
#define OF_APP__USE__MODELS_MANAGER
// We will draw some meshes.
// Uncomment to use an internal class helper: SurfingModelsManager.h
// Comment to use more low level (raw/vanilla) OF core classes.

// OPTIONAL:
//#define OF_APP__USE__OPTIONAL_SETUP
// Uncomment to test some optional features:
// 1. Changes log level to verbose.
// 2. Check if the app is probably opened for the first time.
// 3. Forces some scene settings.

//--

#include "ofxSurfingPBR.h"

#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

#ifdef OF_APP__USE__MODELS_MANAGER
	#include "SurfingModelsManager.h"
#endif

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void startup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void windowResized(ofResizeEventArgs & e);

	void setupParams();
	void drawGui();

	ofEasyCam camera;

	ofxSurfingPBR pbr;
	void setupPBR();
	void renderScene();
	void drawMyScene(); // Our scene is drawn here!
	void updateSceneTransforms();

	//--

	// Mesh
	string pathMesh;
	ofVboMesh mesh;
	void setupMesh();
	void drawMesh();

	//--

	// Model
	void drawModel();

	//--

	// Params

	// For the Gui and settings
	ofParameterGroup parameters;
	void Changed(ofAbstractParameter & e);
	string pathSettings = "ofApp.json";
	SurfingAutoSaver autoSaver;
	void save();
	void load();

	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "Pos y", 0, -1.f, 1.f };

	ofParameterGroup paramsTransforms;

	ofParameterGroup paramsAnimate;
	ofParameter<bool> bAnimRotate { "Rotate Anim", false };
	ofParameter<float> rotateSpeed { "Rotate Speed", 0.5f, 0, 1 };
	ofParameter<bool> bAnimZoom { "Zoom Anim", false };
	ofParameter<float> zoomSpeed { "Zoom Speed", 0.5f, 0, 1 };
	ofParameter<float> powZoom { "Zoom Pow", 0.5f, 0, 1 };

	ofParameter<int> indexScene { "SCENE", 0, 0, 2 };
	ofParameter<string> nameScene { "Name", "" };
	vector<string> namesScenes = { "Test Scene", "Mesh", "Model" };
	ofEventListener listenerIndexScene;

	ofParameter<void> vReset { "Reset Scene" };
	ofParameter<void> vNextIndexScene { "Next" };
	ofParameter<void> vPrevIndexScene { "Previous" };

	ofEventListener listenerReset;
	ofEventListener listenerNext;
	ofEventListener listenerPrev;

	//--

	void doPrevScene();
	void doNextScene();
	void doReset();

	void drawHelp();
	void buildHelp(); //refresh help info to display updated
	ofParameter<bool> bHelp { "Help", false };
	string sHelp;

	//--

	// Gui
	ofxPanel gui;
	SurfingOfxGuiPanelsManager guiManager;

	void refreshGui();
	// Refresh gui for minimize/collapse workflow
	// and to layout the gui panels on the app window.
	ofEventListener listenerGuiRefresh;

	//--

	// Models files browser
#ifdef OF_APP__USE__MODELS_MANAGER
	SurfingModelsManager modelsManager;
	ofEventListener listenerIndexModel; //get class internal index changed
#else
	// One single model using OF core
	void setupModel();
	ofxAssimpModelLoader model;
	vector<ofVboMesh> meshesModel;
	string pathModel;
	bool loadModel(string path, float scaled = 1);
#endif
};
