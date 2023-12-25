#pragma once
#include "ofMain.h"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"
#include "ofxSurfingPBRConstants.h"
#include "SurfingMaterial.h"

//--

class SurfingFloor {
public:
	SurfingFloor();
	~SurfingFloor();

	void setup();

private:
	void startup();

public:
	void doResetAll();

public:
	void update();
	void update(ofEventArgs & args);
	void updateScene();

public:
	void draw(); // plane or box floor

private:
	void drawPlaneFloor(); // plane floor
	void drawBoxFloor(); // box floor

public:
	void drawDebug();
	void drawGui();
	
public:
	void exit(); //only required to save camera on exit
	void keyPressed(int key);
	ofParameter<bool> bKeys;

private:
	void windowResized(ofResizeEventArgs & e);

private:
	void setupParams();
	void setupGui();
	void refreshGui();

	ofEventListener eSaveOfxGui;
	ofEventListener eLoadOfxGui;

	//--

private:
	void Changed(ofAbstractParameter & e);

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	void ChangedDisplacers(ofAbstractParameter & e);
#endif

	//--

public:
	SurfingMaterial material;

	void beginMaterial();
	void endMaterial();

	void setColorGlobal(ofFloatColor color);

	//--

private:
	ofPlanePrimitive floorPlane; //floor
	void refreshPlane();
	bool bFlagRefresPlane = false;

	ofBoxPrimitive floorBox; //floor
	void refreshBox();
	bool bFlagRefreshBox = false;
	ofParameter<float> floorBoxDepth;

	//--

private:
	// TODO fix crash callbacks
	//To fix some settings
	//that are not updated/refreshed correctly
	//and/or app startup crashes!
	//App flow controls
	//bool bDisableCallbacks = false;
	bool bDoneSetup = false;
	bool bDoneSetupParams = false;
	bool bDoneStartup = false;
	bool bDoneExit = false;
	//bool bDoneStartupDelayed = false;
	//bool bAppRunning = false;

	//--

public:
	ofParameterGroup parameters;
	ofParameterGroup paramsTransform;
	ofParameterGroup paramsDraw;

	ofParameter<bool> bDraw;
	ofParameter<bool> bDrawPlane;
	ofParameter<bool> bDrawBox;

	ofParameter<bool> bDrawWireframe;
	ofParameter<glm::vec2> size; //normalized
	ofParameter<glm::vec2> resolution;
	ofParameter<bool> bSquaredSize;
	ofParameter<bool> bInfinite;
	// will ignore size
	// make the plane huge size to better "fit a full horizon line"

	ofParameter<float> rotation; //x axis
	ofParameter<float> position; //y pos

	ofParameter<void> vResetFloor;
	ofParameter<void> vResetFloorTransform;

	ofxPanel gui;
	ofParameter<bool> bGui;
	ofParameter<bool> bDebug;

public:
	ofParameter<int> helpLayout;

public:
	// helper to improve layout with many gui panels.
	ofRectangle getGuiShape() const;

	bool isVisibleDebugShader();

	//--

	void doResetFloor();
	void doResetFloorTransform();

	//--

private:
	string path = "ofxSurfingPBR_Floor.json";

public:
	bool load();
	void save();

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
private:
	SurfingAutoSaver autoSaver;
#endif

	//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
private:
	ofShader shaderPlane;
	bool bLoadedShaderPlane = false;
	void setupShaderPlane();
	ofFloatImage img;
	ofParameter<bool> bLimitImage;

	ofParameterGroup displacersParams;
	ofParameterGroup displaceMaterialParams;
	ofParameterGroup noiseParams;

	ofParameter<float> noiseAmplitude;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseSpeed;

	ofParameter<float> displacementStrength;
	ofParameter<float> displacementNormalsStrength;
	ofParameter<float> normalGeomToNormalMapMix;

	ofParameter<void> vResetDisplace;
	ofParameter<void> vResetNoise;
	void doResetNoise();
	void doResetDisplace();

	void setupParamsDisplace();
	void updateDisplace();
	void refreshImgShaderPlane();

public:
	ofParameter<bool> bShaderToPlane;
	ofParameter<bool> bDisplaceToMaterial;

public:
	void beginShaderPlane();
	void endShaderPlane();
#endif



};
