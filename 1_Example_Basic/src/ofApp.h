/*

	This example includes the main and bigger class from the addon:  
	`ofxSurfingPBR.h`.  

	This class has multiple modules 
	or commonly used features included by default:  

	- Floor plane with a simple material  
		and customizable size and transforms.  
	- One full PBR material ready to be used.  
	- CubeMap for the PBR material and drawing as background.  
	- Background object with sphere or box with material.  
	- Test Scene draw with three prims.  
	- Lights bundle with 4 predefined lights: 
		`Point`, `Direct`, `Spot` and `Area`.  
	- An internal and public exposed camera.
	- Shader module to help integration/dev. 
		(currently commented / disabled by default)  

	All the modules has his own settings and a gui (ofGui) 
	to tweak all the internal parameters.  
	All of them with all the settings persistent, 
	gui exposed and ready to use on any project.  

*/

#pragma once

#include "ofMain.h"

#include "ofxSurfingPBR.h"

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
};
