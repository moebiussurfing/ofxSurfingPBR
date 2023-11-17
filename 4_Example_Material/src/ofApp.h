#pragma once
#include "ofMain.h"

#include "ofxSurfingPBR.h"
#include "SurfingMaterial.h"

#include "SurfingMeshSphereDisplaced.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void keyPressed(int key);
	void exit();

	ofxSurfingPBR pbr;
	void renderScene();

	ofEasyCam camera;
	void drawYourScene();

	SurfingMaterial material;

	SurfingMeshSphereDisplaced sphereDisp;
};