/*

This example is for building a testing 
workflow for custom shader features.
Based on @Paolo code.

*/


#pragma once

#include "ofMain.h"

#include "SurfingMaterial.h"
// We only need this header bc
// we want to use only the material class
// from the ofxSurfingPBR addon!

#define SURFING__USE__OF_CORE_PBR_MATERIALS
// uncomment to disable the addon features/helpers
// and switch to "vanilla" OF core materials 
// but instead of the addon!

#define USE_PAO_CAM 0// 0 is for FHD, 1 to 640x480

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	void drawScene();
	void renderScene();

	void buildHelp();
	string sHelp;

	void drawGui();

	void setupObjects();
	void setupLights();
	void setupMaterials();
	void resetMaterials();
	void setupGui();

	// Using OF CORE "vanilla" lights
	// not the bundled from the addon.
	vector<shared_ptr<ofLight>> lights;

	ofEasyCam camera;
	ofMesh logoMesh;
	ofMesh boxMesh;

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	// Using addon materials
	SurfingMaterial boxesMaterial;
	SurfingMaterial logoMaterial;
#else
	// Using OF CORE "vanilla" materials
	ofMaterial logoMaterial;
	ofMaterial boxesMaterial;
#endif

	void refreshGuiLinks();
	void refreshGuiAnchor();
	
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
	
	ofParameter<bool> bDrawBoxes;

	ofShadowType shadowType;
	float shadowSampleRadius = 2.;
	ofParameter<bool> bDrawLogo;
    
	//--

	void setupShader();
	void updateShader();
    ofShader shader;
    ofPlanePrimitive plane;
    ofFloatImage img;
    ofVideoGrabber videoGrabber;
	ofParameter<void> vRestartCamera; //reset camera
	int camWidth;
    int camHeight;
    ofParameter<float> noiseAmplitude;
    ofParameter<float> noiseScale;
    ofParameter<float> noiseSpeed;
    ofParameter<bool> bDrawPlaneShader;
	ofParameter<void> vRandomNoise;
	ofParameter<int> indexMode;//TODO: add more modes
	ofParameterGroup parametersPlaneShader;
	void ChangedParametersPlaneShader(ofAbstractParameter & e);
	size_t indexCam = 0; //pick your device id from console window

    ofParameter<float> offsetPlaneHeight;
};
