#pragma once

#include "ofMain.h"

#include "SurfingLights.h"
#include "SurfingMaterial.h"
#include "ofxCameraSaveLoad.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	void windowResized(int w, int h);

	void setupGui();
	void refreshGui();
	void drawGui();
	ofxPanel gui;
	ofParameterGroup g;
	ofParameter<bool> bRefreshGui;
	
	void drawScene();
	void renderScene();
	callback_t f_RenderScene = nullptr;

	SurfingMaterial material;
	SurfingLights lights;

	ofEasyCam camera;
	ofParameter<void> vResetCamera;
	ofEventListener listenerResetCamera;
	void doResetCamera();
	string pathCamera = "ofxSurfingPBR_Camera.txt";
};