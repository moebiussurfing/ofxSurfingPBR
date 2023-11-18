#pragma once
#include "ofMain.h"

#define OFAPP__USE__OPTIONAL_SETUP 0
#define SURFING__USE__MODELS_MANAGER

//--

#include "ofxSurfingPBR.h"

#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

#ifdef SURFING__USE__MODELS_MANAGER
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

	ofParameterGroup animateParams;
	ofParameterGroup transformsParams;

	ofParameter<bool> bAnimRotate { "Rotate Anim", false };
	ofParameter<float> rotateSpeed { "Rotate Speed", 0.5f, 0, 1 };
	ofParameter<bool> bAnimZoom { "Zoom Anim", false };
	ofParameter<float> zoomSpeed { "Zoom Speed", 0.5f, 0, 1 };
	ofParameter<float> powZoom { "Zoom Pow", 0.5f, 0, 1 };

	ofParameter<int> indexScene { "SCENE", 0, 0, 2 };
	ofParameter<string> nameScene { "Name", "" };
	vector<string> namesScenes = {
		"THREE-PRIMS",
		"MESH",
		"MODELS"
	};
	ofEventListener listenerIndexScene;

	ofParameter<void> vReset { "Reset Scene" };
	ofParameter<void> vNextIndexScene { "Next" };
	ofParameter<void> vPrevIndexScene { "Prev" };

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

	void refreshGui();
	// Refresh gui for minimize/collapse workflow
	// and to layout the gui panels on the app window.
	ofEventListener listenerGuiRefresh;

	//--

	// Models files browser
#ifdef SURFING__USE__MODELS_MANAGER
	SurfingModelsManager modelsManager;
	ofEventListener listenerIndexModel;//get class internal index changed
#else
	// One single model
	void setupModel();
	ofxAssimpModelLoader model;
	vector<ofVboMesh> meshesModel;
	string pathModel;
	bool loadModel(string path, float scaled = 1);
#endif
};
