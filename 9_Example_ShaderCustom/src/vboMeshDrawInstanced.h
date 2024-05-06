#pragma once

// tig: uncomment the following line to use programmable GL , and GLSL 150
// otherwise this example will run using OpenGL 2.0 / GLSL 1.20
#define USE_PROGRAMMABLE_GL

// note that if you use programmable GL, a different set of shaders will be loaded.
// see ofApp.cpp


#include "ofMain.h"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"

class vboMeshDrawInstanced {
	
	ofVboMesh mVboBox;

	shared_ptr<ofShader> mShdInstanced;
	ofTexture mTexDepth;
	
	ofParameter<bool> bDraw;
	ofParameter<bool> isShaderDirty;
	ofParameter<glm::vec3> position;
	ofParameter<glm::vec3> rotation;
	ofParameterGroup parameters;
	ofxPanel gui;

public:
	vboMeshDrawInstanced();
	~vboMeshDrawInstanced();

	void setup();
	void update();
	void draw();
	void drawGui();
	
	void keyReleased(int key);
};
