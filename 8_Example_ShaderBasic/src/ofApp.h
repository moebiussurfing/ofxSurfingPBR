/*

This example includes the main and bigger class from the addon:  
`ofxSurfingPBR.h`.  

Example to test an internal shader.
Verify that is not commented 
the directive line where shader is enabled!!
#define SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
at the top of ofxSurfingPBRConstants.h
TODO: add a basic custom shader.

*/

#pragma once

#include "ofMain.h"

#include "ofxSurfingPBR.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void exit();

	ofxSurfingPBR pbr;
	void renderScene();

	ofEasyCam camera;
	void drawScene();
};
