#pragma once

#include "ofMain.h"

#include "SurfingMaterial.h" 
// We only need this header bc 
// we want to use only the material class
// from the ofxSurfingPBR addon!

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
	void renderScene();

	void drawGui();
	
	void setupObjects();
	void setupLights();
	void setupMaterials();
	void resetMaterials();

	// Using OF CORE "vanilla" lights
	// not the bundled from the addon.
	vector<shared_ptr<ofLight>> lights;

	ofEasyCam camera;
	ofMesh logoMesh;
	ofMesh sphereMesh;
	ofMesh boxMesh;
	ofMesh cylinderMesh;

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	// Using addon materials
	SurfingMaterial bgMaterial;
	SurfingMaterial logoMaterial;
	SurfingMaterial boxesMaterial;
	SurfingMaterial sphereMaterial;

	void refreshGuiLinks();
	void refreshGuiAnchor();
#else
	// Using OF CORE "vanilla" materials
	ofMaterial bgMaterial;
	ofMaterial boxesMaterial;
	ofMaterial logoMaterial;
	ofMaterial sphereMaterial;
#endif

	ofParameter<bool> bDrawBg;
	ofParameter<bool> bDrawLogo;
	ofParameter<bool> bDrawBoxes;
	ofParameter<bool> bDrawSphere;
	ofParameter<void> vResetMaterials;

	ofParameterGroup parameters;
	void Changed(ofAbstractParameter & e);

	void save();
	bool load();
	
	ofxPanel gui;

	ofColor lightColor;
	float colorHue = 0.0f;

	bool bDrawFrustums = false;
	bool bEnableShadows = true;

	ofShadowType shadowType;
	float shadowSampleRadius = 2.;
};
