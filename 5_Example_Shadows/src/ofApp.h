#pragma once

#include "ofMain.h"

#include "SurfingMaterial.h"

//#define SURFING__USE__OF_CORE_PBR_MATERIALS
// uncomment to disable the addon features 
// and switch to "vanilla" OF core materials.

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	void buildHelp();
	string sHelp;

	void drawScene();
	void drawGui();

	void renderScene();
	void setupMaterials();

	void resetMaterials();

	// Using OF CORE "vanilla" lights
	vector<shared_ptr<ofLight>> lights;

	ofEasyCam camera;
	ofMesh logoMesh;
	ofMesh sphereMesh;
	ofMesh boxMesh;
	ofMesh cylinderMesh;

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	SurfingMaterial bgMaterial;
	SurfingMaterial logoMaterial;
	SurfingMaterial boxesMaterial;
	SurfingMaterial sphereMaterial;

	ofParameter<bool> bDrawBg;
	ofParameter<bool> bDrawLogo;
	ofParameter<bool> bDrawBoxes;
	ofParameter<bool> bDrawSphere;
	ofParameterGroup parameters;
	void Changed(ofAbstractParameter & e);

	ofxPanel gui;
	void refreshGuiLinks();
	void refreshGuiAnchor();

	SurfingAutoSaver autoSaver;
	void save();
	void load();
#else
	// Using OF CORE "vanilla" materials
	ofMaterial bgMaterial;
	ofMaterial boxesMaterial;
	ofMaterial logoMaterial;
	ofMaterial sphereMaterial;
#endif

	ofColor lightColor;
	float colorHue = 0.0f;

	bool bDrawFrustums = false;
	bool bEnableShadows = true;

	ofShadowType shadowType;
	float shadowSampleRadius = 2.;
};
