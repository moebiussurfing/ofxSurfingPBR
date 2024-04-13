/*

This example is hardly based on the example
openFrameworks\examples\gl\shadowsExample 
from the OF Core made by @NickHardeman.
We will learn how to replace easily their OF materials 
with standalone materials that the addon includes, 
in this case will be standalone:
all the rest of the environment will remain un-touched 
from the example, this means that we will not use 
the lights modules from the addon bigger classes like
ofxSurfingPBR or SurfingSceneManager neither: 
only the "vanilla" classes that bundles the OF Core will be used 
besides of the material class instances for ach scene element.

*/


#pragma once

#include "ofMain.h"

#include "SurfingMaterial.h"
// We only need this header bc
// we want to use only the material class
// from the ofxSurfingPBR addon!

//#define SURFING__USE__OF_CORE_PBR_MATERIALS
// uncomment to disable the addon features
// and switch to "vanilla" OF core materials.

#define USE_PAO_CAM 0// 0 is for FHD, 1 to 640x480

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	void buildHelp();
	string sHelp;

	void drawScene();
	void renderScene();

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
	/*void setGuiPosition(glm::vec2 pos);
	void refreshGui();
	ofRectangle getGuiShape() const;*/

	ofColor lightColor;
	float colorHue = 0.0f;

	bool bDrawFrustums = false;
	bool bEnableShadows = true;
	
	ofParameter<bool> bDrawBoxes;

	ofShadowType shadowType;
	float shadowSampleRadius = 2.;
	ofParameter<bool> bDrawLogo;
    
	//--

    // BEGIN PAO
	
	void setupShader();
	void updateShader();
    ofShader shader;
    ofPlanePrimitive plane;
    ofFloatImage img;
    ofVideoGrabber videoGrabber;
    int camWidth;
    int camHeight;
    ofParameter<float> noiseAmplitude;
    ofParameter<float> noiseScale;
    ofParameter<float> noiseSpeed;
    ofParameter<bool> bDrawPlaneShader;
	ofParameterGroup parametersPlaneShader;

    // END PAO
};
