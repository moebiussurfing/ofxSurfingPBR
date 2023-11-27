/*

This example do not includes the main bigger classes from the addon:  
`ofxSurfingPBR.h` neither the `SurfingSceneManager.h`.  
These both classes have multiple modules or features included by default,  
(as it could be the CubeMap, the background object, the floor plane...etc).  

Here we want to learn and create a standalone material  
and a standalone kit of lights.  
The material is applied to the floor plane  
and to the testing three prims scene that we will draw.  

The lights bundled are 4 in total. One of each of the available types   
into OF core:  
`Point`, `Direct`, `Spot` and `Area`.  
All with his settings on a gui to tweak all the internals.  
All of them with all the settings persistent, gui exposed and ready  
to use on any project.  

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

	// gui
	void setupGui();
	void refreshGui(bool bHard=false);
	void drawGui();
	ofxPanel gui;
	ofParameterGroup g;
	ofParameter<bool> bRefreshGui;
	
	// scene
	void drawScene();
	void renderScene();
	callback_t f_RenderScene = nullptr;

	//camera
	ofEasyCam camera;
	ofParameter<void> vResetCamera;
	ofEventListener listenerResetCamera;
	void doResetCamera();
	string pathCamera = "ofxSurfingPBR_Camera.txt";

	//--

	// standalone material
	SurfingMaterial material;

	// standalone lights
	SurfingLights lights;
};