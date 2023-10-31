#pragma once
#include "ofMain.h"

#include "ofxSurfingHelpers.h"
#include "ofxSurfingPBR.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	ofEasyCam camera;
	void drawYourScene();

	ofxSurfingPBR pbr;
	void renderScene();
};
