#pragma once
#include "ofMain.h"

//--

/*
	An openFrameworks addon 
	to easily test, learn, and use 
	the new PBR OF CORE features 
	from the new 0.12+ releases.

	You should use the GitHub master branch: 
		https://github.com/openframeworks/openFrameworks

	All this code is copied from or/and hardly based on this OF forum topic:
		https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19  

	Original inspiration code authors:  
		@NickHardeman https://github.com/NickHardeman
		and @paolo-scoppola https://github.com/paolo-scoppola.
*/

//--

/*
	TODO

	- add simple scenes presets manager
		- expose easy paramsPresets: 
			colors, lights power, 
	- add save/load all.
	- fix startup/exit loading all settings

	- make some linking colors 
		to fast set bg, floor, 
		lights together with bright tweaks...
	
	- add alternate top camera? 

	- add custom global path to /data/ofxSurfingPBR/MAT_NAME/

	- SHADERS AND DISPLACE
		- fix plane light not working when shaders enabled..
		- check pipeline / enable ArbTex from the example!
		- customize share plane divisions and img size/clamp.
		- get mesh mods/shader example.

	- queue cubeMaps into a vector to browse easily
	- add ImGui mode.
	- add presets manager 
		to explore and save materials.
	- make a scene manager allowing
		to queue many actors/ofNodes, materials 
		and lights on a std::vector.
		something like ofxPBR or ofxPBRHelper.
*/

//--

#include "SurfingMaterial.h"

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	#include "SurfingLights.h"
#endif

#include "SurfingBg.h"
#include "SurfingFloor.h"

#ifdef SURFING__PBR__USE_ADDON_EASY_CAM
	#include "surfingEasyCamOfxGui.h"
#else
	#include "ofxCameraSaveLoad.h"
#endif

#include "ofxGui.h"

#include <functional>
using callback_t = std::function<void()>;

//--

class ofxSurfingPBR {
public:
	ofxSurfingPBR();
	~ofxSurfingPBR();

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	SurfingLights lights;
#endif

private:
	void setupBg();

public:
	SurfingBg bg;
	SurfingFloor floor;

	ofParameter<bool> bDrawBg;

public:
	void setup(); // use only one setup method! don't call two setup methods!
	void setup(ofCamera & camera_); // use only one setup method! don't call two setup methods!

private:
	void startup();
	void startupDelayed();

public:
	void draw(); //main draw
	void drawFloor(); //plane or box floor
	void drawBg();

#define DO_SCENE_TEST_TRANSFORMS 1
	void drawTestScene(); // a simple scene with 3 prims for easy testing
	void pushTestSceneTransform();
	void popTestSceneTransform();
	ofParameter<bool> bDrawTestScene;

	void drawGui();
	void drawOfxGui();

private:
	void drawDebug();
	void drawDebugFPS();
	void drawHelp();

public:
	void exit(); // only required to save camera on exit

private:
	void keyPressed(int key);
	void keyPressed(ofKeyEventArgs & eventArgs);

	void windowResized(ofResizeEventArgs & e);

public:
	void setLogLevel(ofLogLevel logLevel);

private:
	void setupParams();
	void setupCallbacks();
	void setupGui();
	void refreshGui();

	void update();
	void update(ofEventArgs & args);

	//--

private:
	void updateScene();
	void drawScene();
	void drawDebugScene();

private:
	void Changed(ofAbstractParameter & e);
	void ChangedAdvanced(ofAbstractParameter & e);
	void ChangedTestScene(ofAbstractParameter & e);
	void ChangedDraw(ofAbstractParameter & e);
	void ChangedCamera(ofAbstractParameter & e);

	ofEventListener e_bDrawBgColorPlain;
	ofEventListener e_bDrawBgColorObject;
	ofEventListener e_bDrawCubeMap;

#ifdef SURFING__PBR__USE_CUBE_MAP
	void ChangedCubeMaps(ofAbstractParameter & e);
#endif

	//--

	// Material B for other objects
public:
	SurfingMaterial material;

public:
	void beginMaterial();
	void endMaterial();

	//----

#ifdef SURFING__PBR__USE_ADDON_EASY_CAM
	surfingEasyCamOfxGui cam;
#else
	// Camera
public:
	void setCameraPtr(ofCamera & camera_); //don't need use when camera is passed to setup function!
	void setCameraPtr(ofCamera * camera_); //don't need use when camera is passed to setup function!

	// For getting camera from the parent class/ofApp
	// (TODO: Currently is not required bc the cam is instantiated on there!)
	ofCamera * getOfCameraPtr();
	ofEasyCam * getOfEasyCamPtr();

private:
	ofParameterGroup paramsCamera;
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
#endif

	//--

private:
	ofEventListener eSaveOfxGui;
	ofEventListener eLoadOfxGui;

	callback_t f_RenderScene = nullptr;

public:
	void setFunctionRenderScene(callback_t f = nullptr);

	//--

private:
	// TODO fix crash callbacks
	// To fix some settings
	// that are not updated/refreshed correctly
	// and/or app startup crashes!
	// App flow controls
	//bool bDisableCallbacks = false;
	bool bDoneSetup = false;
	bool bDoneSetupParams = false;
	bool bDoneStartup = false;
	bool bDoneStartupDelayed = false;
	bool bDoneExit = false;
	bool bAppRunning = false;

	//--

public:
	// main container to expose to ui
	// and to handle the persistence of the settings.
	ofParameterGroup parameters;

	ofParameterGroup paramsDraw;
	ofParameterGroup paramsDrawBg;
	ofParameterGroup paramsShowGui;

	//--

	//mainly to expose to external ui's like ImGui
	ofParameterGroup & getMaterialParameters();

	ofParameter<bool> bGui; //for global ui
	ofParameter<bool> bGuiOfxGui; //for ofxGui

	ofParameterGroup paramsInternal;
	ofParameterGroup paramsGui;

	ofParameterGroup paramsAdvanced;
	//we would like to hide these params from the ui,
	//bc they are only included to add to settings group!

	ofParameter<void> vMinimizeAllGui;
	ofParameter<void> vMaximizeAllGui;

	//ofParameter<int> indexGuiLayout; //TODO notify
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
	bool bFlagBuildHelp = false;

public:
	ofParameter<bool> bHelp;

	ofParameter<bool> bDebug;
	bool bUnlockFps = false;
	ofParameter<bool> bKeys;

	ofParameterGroup paramsTestScene;
	ofParameter<float> scaleTestScene;
	ofParameter<glm::vec3> positionTestScene;
	ofParameter<void> vResetTestScene;

	ofParameter<void> vResetAll;

public:
	ofxPanel gui;

private:
	SurfingOfxGuiPanelsManager guiManager;

public:
	// helper to improve layout with many gui panels.
	ofRectangle getGuiShape() const;

	// returns layout index
	// each index would have different positions for help and debug windows
	//const int getGuiLayout();

	void doNextLayoutHelp();
	void doPrevLayoutHelp();
	//void doNextLayouGui();

	//for helping on responsive layouts
	const ofxSurfing::SURFING_LAYOUT getLayoutHelp();

	bool isWindowPortrait();

	//--

#ifdef SURFING__PBR__USE_CUBE_MAP
private:
	ofCubeMap cubeMap;

	void setupCubeMap();
	void drawCubeMap();
	string path_CubemapsFolder = "cubemaps"; //bin/data/cubemaps/
	string path_CubemapFilenameDefaultData = "modern_buildings_2_1k.exr"; //TODO: store path on settings
	bool bLoadedCubeMap = false;

public:
	ofParameter<string> path_CubemapFileAbsPath;

public:
	ofParameterGroup paramsCubeMap;
	ofParameter<string> cubeMapName;
	ofParameter<int> cubeMapMode;
	ofParameter<string> cubeMapModeName;
	ofParameter<float> cubeMapprefilterRoughness;
	ofParameter<float> cubeMapExposure;
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

private:
	ofEventListener eResetAll;
	ofEventListener eDebug;

public:
	void doResetAll(bool bHard = false);
	void doResetDefaultScene();
	void doResetTestScene();

	//--

private:
	string path = "ofxSurfingPBR_Scene.json";

public:
	bool load();
	void save();

	bool loadAll();
	void saveAll();
	ofParameter<void> vLoadAll;
	ofParameter<void> vSaveAll;
	ofEventListener eLoadAll;
	ofEventListener eSaveAll;

	bool getSettingsFileFound();
	// to check if the app is opened for the first time
	// this allows to force customize the scene from ofApp at startup!
	bool getSettingsFileFoundForScene();
	bool getSettingsFileFoundForMaterial();
	bool getSettingsFileFoundForCamera();

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
private:
	SurfingAutoSaver autoSaver;
#endif
};
