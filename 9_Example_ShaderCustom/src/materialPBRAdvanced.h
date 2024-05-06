#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class materialPBRAdvanced {

public:
	void setup();
	void draw();
	void drawGui();

	void renderScene(bool bShadowPass);

	bool reloadShader();

	void keyPressed(int key);

	ofVboMesh meshLogoHollow;
	ofMaterial matLogo;
	ofMaterial matFloor;
	ofMaterial matPlywood;
	ofMaterial matSphere;

	ofVboMesh meshPlySphere;

	ofShader mDepthShader;

	ofParameter<bool> bDebug;
	ofParameter<bool> bWiggleVerts;

	ofParameterGroup parameters;
	ofxPanel gui;
};
