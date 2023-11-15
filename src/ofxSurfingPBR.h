#pragma once
#include "ofMain.h"

//--

/*
	An OpenFrameworks addon 
	to easily test, learn, and use 
	the new PBR CORE features from the new OF 0.12+ releases.

	You should use the GitHub master branch: 
		https://github.com/openframeworks/openFrameworks

	All this code is copied from or/and hardly based on this OF forum topic:
		https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19  

	Original authors:  
		@NickHardeman https://github.com/NickHardeman
		and @paolo-scoppola https://github.com/paolo-scoppola.
*/

//--

/*
* TODO
* 
	- SHADERS AND DISPLACE
		- fix plane light not working when shaders enabled..
		- check pipeline / enable ArbTex from the example!
		- customize share plane divisions and img size/clamp.
		- get mesh mods/shader example.

	- add ImGui mode.
	- add presets manager 
		to explore and save materials.
	- make a scene manager allowing
		to queue many actors/ofNodes, materials 
		and lights on a std::vector.
		something like ofxPBR or ofxPBRHelper.
		- create a new example 
			with two instantiated materials.
	- add custom global path to /data/ofxSurfingPBR/
*/

//--

#include "SurfingMaterial.h"
#include "SurfingLights.h"
#include "SurfingBg.h"

#include "ofxCameraSaveLoad.h"
#include "ofxGui.h"

#include <functional>
using callback_t = std::function<void()>;

//--

class ofxSurfingPBR {
public:
	ofxSurfingPBR();
	~ofxSurfingPBR();

	SurfingLights surfingLights;
	//void setupLights();
	//void beginLights();
	//void endLights();

private:
	void setupBg();
	SurfingBg surfingBg;

public:
	void setup(); //use only one setup method! don't call two setup methods!
	void setup(ofCamera & camera_); //use only one setup method! don't call two setup methods!

private:
	void startup();
	void startupDelayed();

public:
	void draw(); //main draw
	void drawPlane();
	void drawBg();
	void drawTestScene(); //a simple scene with 3 prims for easy testing
	void drawGui();
	void drawOfxGui();

private:
	void drawDebug();
	void drawDebugFPS();
	void drawHelp();

public:
	void exit();
	void keyPressed(int key);

private:
	void windowResized(ofResizeEventArgs & e);

public:
	void setLogLevel(ofLogLevel logLevel);

private:
	void setupParams();
	void setupGui();
	void refreshGui();
	void update();
	void update(ofEventArgs & args);

	//--

private:
	void setupPBRScene();
	void updatePBRScene();
	void drawPBRScene();
	void drawPBRSceneDebug();

private:
	void ChangedPlane(ofAbstractParameter & e);
	void ChangedLight(ofAbstractParameter & e);
	void ChangedShadow(ofAbstractParameter & e);
	void ChangedInternal(ofAbstractParameter & e);
	void ChangedTestScene(ofAbstractParameter & e);
	void ChangedCamera(ofAbstractParameter & e);
	//void ChangedBg(ofAbstractParameter & e);

#ifdef SURFING__USE_CUBE_MAP
	void ChangedCubeMaps(ofAbstractParameter & e);
#endif

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	void ChangedDisplacers(ofAbstractParameter & e);
#endif

	//--

	//material for other objects
public:
	SurfingMaterial material;

public:
	void beginMaterial();
	void endMaterial();

	//--

	//material for the plane floor
private:
	ofMaterial materialPlane;

public:
	void beginMaterialPlane();
	void endMaterialPlane();

	//--

	// Camera
public:
	void setCameraPtr(ofCamera & camera_); //don't need use when camera is passed to setup function!
	void setCameraPtr(ofCamera * camera_); //don't need use when camera is passed to setup function!

	// For getting camera from the parent class/ofApp
	// (TODO: Currently is not required bc the cam is instantiated on there!)
	ofCamera * getOfCameraPtr();
	ofEasyCam * getOfEasyCamPtr();

private:
	ofParameterGroup cameraParams;
	ofParameter<bool> bEnableCameraAutosave;
	ofParameter<void> vSaveCamera;
	ofParameter<void> vLoadCamera;
	string pathCamera = "ofxSurfingPBR_Camera.txt";

public:
	ofParameter<void> vResetCamera;

private:
	ofCamera * camera;

public:
	void doResetCamera();
	void doLoadCamera();
	void doSaveCamera();

	//--

private:
	callback_t f_RenderScene = nullptr;

public:
	void setFunctionRenderScene(callback_t f = nullptr);

	//--

private:
	ofPlanePrimitive plane; //floor
	void refreshPlane();
	bool bFlagRefreshPlane = false;

	//--

private:
	//TODO fix crash callbacks
	// To fix some settings
	// that are not updated/refreshed correctly
	// and/or app startup crashes!
	// App flow controls
	bool bDisableCallbacks = false;
	bool bDoneSetup = false;
	bool bDoneSetupParams = false;
	bool bDoneStartup = false;
	bool bDoneDelayed = false;
	bool bAppRunning = false;

	//--

public:
	// Main container to expose to ui and to handle persistent settings.
	ofParameterGroup parameters;

	ofParameterGroup showDrawParams;
	ofParameterGroup showGuiParams;

	//--

	//mainly to expose to external ui's like ImGui
	ofParameterGroup & getMaterialParameters();

	ofParameter<bool> bGui; //for global ui
	ofParameter<bool> bGui_ofxGui; //for ofxGui

	ofParameterGroup internalParams;
	ofParameterGroup guiParams;

	ofParameterGroup advancedParams;
	//we would like to hide these params from the ui,
	//bc they are only included to add to settings group!

	ofParameter<void> vMinimizeAllGui;
	ofParameter<void> vMaximizeAllGui;

	ofParameter<int> guiLayout; //TODO notify
	ofParameter<string> nameGuiLayout; //TODO notify

private:
	vector<string> namesGuiLayouts = {
		"LAYOUT 0",
		"LAYOUT 1"
	};
	string sWindowDimensions = "";

public:
	ofParameter<int> helpLayout;
	ofParameter<string> nameHelpLayout;

private:
	void buildHelp();
	string sHelp;

public:
	ofParameter<bool> bHelp;

	ofParameter<bool> bDebug;
	ofParameter<bool> bKeys;

	ofParameterGroup testSceneParams;
	ofParameter<float> scaleTestScene;
	ofParameter<float> positionTestScene;
	ofParameter<void> vResetTestScene;

	ofParameter<bool> bDrawPlane;

	ofParameterGroup planeParams;
	ofParameterGroup planeMaterialParams;
	ofParameterGroup planeSettingsParams;
	ofParameterGroup planeColorsParams;
	ofParameterGroup planeTransformParams;
	ofParameterGroup shadowParams;

	ofParameter<bool> bPlaneWireframe;
	ofParameter<glm::vec2> planeSize; //normalized
	ofParameter<glm::vec2> planeResolution;
	ofParameter<bool> bPlaneInfinite;
	//will ignore planeSize
	//make the plane huge size to better "fit a full horizon line"

	ofParameter<float> planeRotation; //x axis
	ofParameter<float> planePosition; //y pos

	ofParameter<ofFloatColor> planeGlobalColor; //linked (as master not target) to the other colors
	ofParameter<ofFloatColor> planeDiffuseColor;
	ofParameter<ofFloatColor> planeSpecularColor;

	ofParameter<float> planeShiness;

	ofParameter<bool> bDrawShadow;
	ofParameter<bool> bDebugShadow;
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;

	//TODO: OF PBR
	// add other internal OF params
	//ofParameter<float> shadowStrength;
	//ofParameter<glm::vec2> shadowSize;

	ofParameter<void> vResetPlane;
	ofParameter<void> vResetPlaneTransform;
	ofParameter<void> vResetShadow;
	ofParameter<void> vResetAll;

private:
	ofxPanel gui;

public:
	// helper to improve layout with many gui panels.
	ofRectangle getGuiShape() const;

	// returns layout index
	// each index would have different positions for help and debug windows
	const int getGuiLayout();

	bool isVisibleDebugShader();

	void doNextLayoutHelp();
	void doPrevLayoutHelp();
	void doNextLayouGui();

	// For helping on responsive layouts
	const ofxSurfing::SURFING_LAYOUT getLayoutHelp();

	bool isWindowPortrait();

	//--

#ifdef SURFING__USE_CUBE_MAP
private:
	ofCubeMap cubeMap;

	void setupCubeMap();
	string path_CubemapsFolder = "cubemaps"; //bin/data/cubemaps/
	string path_CubemapFilenameDefaultData = "modern_buildings_2_1k.exr"; //TODO: store path on settings
	bool bLoadedCubeMap = false;

public:
	ofParameter<string> path_CubemapFileAbsPath;

public:
	ofParameterGroup cubeMapParams;
	ofParameter<string> cubeMapName;
	ofParameter<int> cubeMapMode;
	ofParameter<string> cubeMapModeName;
	ofParameter<float> cubeMapprefilterRoughness;
	ofParameter<bool> bDrawCubeMap;
	ofParameter<void> vResetCubeMap;
	ofParameter<void> vOpenCubeMap;

private:
	void processOpenFileSelection(ofFileDialogResult openFileResult);
	void doResetCubeMap();

public:
	bool loadCubeMap(string path = "");
#endif

	//--

public:
	void doResetAll(bool bExcludeExtras = false);
	ofEventListener listenerResetAll;

	void doResetPlane();
	void doResetPlaneTransform();
	void doResetShadow();
	void doResetTestScene();

	void doResetMaterial();
	void doRandomMaterial();
	void doRandomMaterialSettings();
	void doRandomMaterialColorGlobal();
	void doRandomMaterialColors();
	void doRandomMaterialColorsAlpha();
	void doRandomMaterialAlphas();

	//--

private:
	string path = "ofxSurfingPBR_Scene.json";

public:
	void load();
	void save();

	bool getSettingsFileFound();
	// To check if the app is opened for the first time
	// this allows to force customize the scene from ofApp at startup!
	bool getSettingsFileFoundForScene();
	bool getSettingsFileFoundForMaterial();
	bool getSettingsFileFoundForCamera();

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
private:
	SurfingAutoSaver autoSaver;
#endif

	//--

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
private:
	ofShader shaderPlane;
	bool bLoadedShaderPlane = false;
	void setupShaderPlane();
	ofFloatImage img;
	ofParameter<bool> bLimitImage;

	ofParameterGroup displacersParams;
	ofParameterGroup displaceMaterialParams;
	ofParameterGroup noiseParams;

	ofParameter<bool> bShaderToPlane;
	ofParameter<bool> bDisplaceToMaterial;

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
	void beginShaderPlane();
	void endShaderPlane();
#endif
};
