/*

	This example do not includes the main bigger classes from the addon:  
	`ofxSurfingPBR.h` neither the smaller `SurfingSceneManager.h`.  
	These both classes have multiple modules or features included by default,  
	(as it could be the CubeMap, the background object, the floor plane...etc).  

	Here we want to learn and create a standalone material  
	and a standalone kit of lights.  
	The material is applied to the floor plane  
	and to the testing three prims scene that we will draw.  

	The lights bundled are 4 in total.  
	One of each of the available types from the OF core:  
	`Point`, `Direct`, `Spot` and `Area`.  
	All them with his settings on a UI to tweak all their internals.  
	All of them with all the settings persistent,  
	with an UI exposed and ready  
	to use on any project.  

	Notice that camera position will be persistent.

*/

#pragma once

#include "ofMain.h"

#include "ofxCameraSaveLoad.h"

#include "SurfingLights.h"
#include "SurfingMaterial.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void draw();
	void exit();
	void windowResized(int w, int h);
	void keyPressed(int key);

	// Gui
	void setupGui();
	void refreshGui(bool bHard = false);
	void drawGui();
	ofxPanel gui;
	ofParameterGroup g;
	ofParameter<bool> bAutoLayout;//will groups ui panels!

	// Scene
	void drawScene();
	void renderScene();
	callback_t f_RenderScene = nullptr;

	// Camera
	ofEasyCam camera;
	ofParameter<void> vResetCamera;
	ofEventListener listenerResetCamera;
	void doResetCamera();
	string pathCamera = "ofxSurfingPBR_Camera.txt";

	//--

	// Standalone material
	SurfingMaterial material;

	// Standalone lights
	SurfingLights lights;
};