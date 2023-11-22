#pragma once
#include "ofMain.h"

#include "ofxSurfingPBR.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void keyPressed(int key);

	ofxSurfingPBR pbr;
	void renderScene();

	ofEasyCam camera;
	void drawYourScene();
};