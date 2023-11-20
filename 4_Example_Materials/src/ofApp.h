#pragma once
#include "ofMain.h"

#include "SurfingMaterial.h"

#include "SurfingMeshSphereDisplaced.h"
#include "ofxCameraSaveLoad.h"

#define SURFING__USE__LIGHTS

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void windowResized(int w, int h);
	void exit();

	void drawGui();
	void refreshGuiLinks();
	void refreshGuiAnchors();

	void renderScene();

	ofEasyCam camera;
	string pathCamera = "ofxSurfingPBR_Camera.txt";

	void drawScene();

	const size_t OFAPP_NUM_ITEMS = 4;

	vector<std::unique_ptr<SurfingMaterial>> materials;
	vector<std::unique_ptr<SurfingMeshSphereDisplaced>> objects;

#ifdef SURFING__USE__LIGHTS
	const int OFAPP_NUM_LIGHTS = 5;
	bool bEnableLights = true;
	vector<shared_ptr<ofLight>> pointLights;
	float colorHue = 0.0f;
	float angle = 0.0f;
	ofColor lightColor;
	void setupLights();
	void updateLights();
	void buildHelp();
	string sHelp = "";
	bool bHelp = true;
#endif

	ofxPanel gui;
	ofParameterGroup g;
};