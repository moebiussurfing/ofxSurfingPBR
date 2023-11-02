#pragma once
#include "ofMain.h"

// Optional
//#define SURFING__USE_FILE_BROWSER
// requires to check previously 
// the sanity of the files you pretend to load!
// bc some could freeze the app or too big!

#include "ofxSurfingPBR.h"

#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void drawGui();
	void keyPressed(int key);
	void exit();

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
	bool loadModel(string path, float scaled=1.0f);
	ofxAssimpModelLoader model;
	vector<ofVboMesh> meshesModel;

	ofxPanel gui;
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "PosY", 0, -1.f, 1.f };
	ofParameter<bool> bRotate { "Rotate", false };
	ofParameter<float> speed { "Speed", 0.5f, 0, 1 };
	ofParameter<int> indexScene { "Scene", 0, 0, 2 };
	ofParameter<void> reset { "Reset" };
	ofEventListener evReset;

#ifdef SURFING__USE_FILE_BROWSER
	void setupDir(string path = "");
	ofDirectory dir;
	ofParameter<string> fileName { "name", "" };
	ofParameter<int> indexFile {
		"file", 0, 0, 0
	};
	ofEventListener eIndexFile;
#endif
};
