#pragma once
#include "ofMain.h"

#define SURFING__USE_FILE_BROWSER

#include "ofxSurfingPBR.h"

#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

#ifdef SURFING__USE_FILE_BROWSER
	#include "SurfingModels.h"
#endif

class ofApp : public ofBaseApp {
public:
	void setup();
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
	
	ofParameterGroup parameters;
	string path = "ofApp.json";

	// Params
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "Pos y", 0, -1.f, 1.f };
	ofParameter<bool> bRotate { "Rotate", false };
	ofParameter<float> speed { "Speed", 0.5f, 0, 1 };
	ofParameter<int> indexScene { "SCENE", 0, 0, 2 };
	ofParameter<void> reset { "Reset" };
	ofParameter<void> nextIndexScene { "Next" };

	ofEventListener listenerIndexScene;
	ofEventListener listenerReset;
	ofEventListener listenerNext;

	void doNextScene();
	void doReset();

	ofParameter<bool> bHelp { "Help", false };
	string sHelp;
	void buildHelp(); //refresh help info to display updated

	// Gui
	ofxPanel gui;
	void refreshGui(); //refresh gui for minimize/collapse workflow

	// Models files browser
#ifdef SURFING__USE_FILE_BROWSER
	void setupModelsBrowser();
	SurfingModels surfingModels;

	ofEventListener listenerLoadModel;
	ofEventListener listenerIndexModel;
#endif
};
