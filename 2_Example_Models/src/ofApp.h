#pragma once
#include "ofMain.h"

#define SURFING__USE_FILE_BROWSER

/*
* //TODO
Modify the mesh. 
Get example from \openFrameworks\examples\gl\materialPBR
	
https://forum.openframeworks.cc/t/ofxassimpmodelloader-how-to-tweak-model-meshes/36665/4
ofxAssimpMeshHelper& helper = model.getMeshHelper(i);
ofMesh* mesh = &helper.cachedMesh;
*/

#include "ofxSurfingPBR.h"

#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

#ifdef SURFING__USE_FILE_BROWSER
	#include "SurfingModels.h"
#endif

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void keyPressed(int key);
	void exit();

	void setupParams();
	void drawGui();

	ofEasyCam camera;

	ofxSurfingPBR pbr;
	void setupPBR();
	void renderScene();
	void drawMyScene();

	// Mesh
	string pathMesh;
	ofVboMesh mesh;
	void setupMesh();

	// Model
	string pathModel;
	void setupModel();
	void drawModel();
	bool loadModel(string path, float scaled = 1.0f);
	ofxAssimpModelLoader model;
	vector<ofVboMesh> meshesModel;

	// Gui
	ofxPanel gui;
	void refreshGui(); //refresh gui for minimize/collapse workflow
	
	ofParameterGroup parameters;
	string path = "ofApp.json";

	// Params
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "Pos y", 0, -1.f, 1.f };
	ofParameter<bool> bRotate { "Rotate", false };
	ofParameter<float> speed { "Speed", 0.5f, 0, 1 };
	ofParameter<int> indexScene { "Scene", 0, 0, 2 };
	ofEventListener listenerIndexScene;
	ofParameter<void> reset { "Reset" };
	ofEventListener listenerReset;
	ofParameter<void> next { "Next" };
	ofEventListener listenerNext;

	ofParameter<bool> bHelp { "Help", false };
	string sHelp;
	void buildHelp(); //refresh help info to display updated

	void nextScene();

	// Models files browser
#ifdef SURFING__USE_FILE_BROWSER
	SurfingModels surfingModels;
	ofEventListener listenerLoadModel;
	ofEventListener listenerIndexModel;
#endif
};
