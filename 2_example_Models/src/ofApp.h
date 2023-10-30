#pragma once
#include "ofMain.h"

// OPTIONAL
#define SURFING__USE_FILE_BROWSER 
// requires check previously the sanity of the files you load!
// some could freeze the app!

#include "ofxSurfingHelpers.h"
#include "ofxSurfingPBR.h"

#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	void drawMyScene();

	ofEasyCam camera;
	ofxSurfingPBR pbr;
	void renderScene();

	void setupModels();

	ofVboMesh mesh;
	//ofVboMesh meshModel;
	vector<ofVboMesh> meshesModel;
	ofxAssimpModelLoader model;
	bool loadModel(string path);

	ofxPanel gui;
	string pathModel;
	string pathMesh;
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "PosY", 0, -1.f, 1.f };
	ofParameter<bool> bRotate { "Rotate", false };
	ofParameter<float> speed { "Speed", 0.5f, 0, 1 };
	ofParameter<int> scene { "Scene", 0, 0, 2 };
	ofParameter<void> reset { "Reset" };
	ofEventListener eReset;

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
