#pragma once
#include "ofMain.h"

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

	void setupModel();

	ofVboMesh mesh;
	ofVboMesh meshModel;
	ofxAssimpModelLoader model;

	ofxPanel gui;
	string pathModel;
	string pathMesh;
	ofParameter<float> scale { "Scale",0, -1.f, 1.f };
	ofParameter<float> yPos { "PosY",0, -1.f, 1.f };
	ofParameter<bool> bRotate{ "Rotate",false };
	ofParameter<float> speed { "Speed",0.5f, 0, 1 };
	ofParameter<int> scene{ "Scene",0, 0, 2 };
	ofParameter<void> reset { "Reset" };
	ofEventListener listener;
};
