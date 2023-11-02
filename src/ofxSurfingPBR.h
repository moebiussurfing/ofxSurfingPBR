#pragma once
#include "ofMain.h"

//--

/*

	All this code is copied and/or hardly based on the OF forum topic: 
	https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19
	authors: @NickHardeman and @Paolo

	See SIMPLE EXAMPLE at the bottom.


	TODO:

	- fix startup plane
	- add callbacks per group?
	- make autosave class

	- hardcode infinite plane mode.
	- add save/load camera
	- cusomize share plane divisions and img size.
	- get mesh mods example.
	- add undo history/browser..
		store each group on a vector to browse back
	- add cubemap file path to settings

	- add ImGui mode.
	- add presets manager 
		and randomizer/undo/redo 
		to explore and save materials.
	- make a scene manager allowing
		to queue many materials 
		and lights on a std::vector.
		something like ofxPBR or ofxPBRHelper.
	- add ofxBgGradient addon ? copy just the gradients
	- add custom global path to /data/ofxSurfingPBR/
	- add randomizers for plane/scene too.

*/

//--

#include "SurfingConstantsPBR.h"

//--

#include "SurfingHelpersLite.h"
#include "SurfingMaterial.h"
#include "ofxGui.h"

#include <functional>
using callback_t = std::function<void()>;

//--

class ofxSurfingPBR {
public:
	ofxSurfingPBR();
	~ofxSurfingPBR();

public:
	void setup();
	void startup();
	void draw();
	void drawPlane();
	void drawTestScene(); //a simple scene for easy testing
	void drawGui();
	void drawDebug();
	void drawHelp();
	void exit();
	void keyPressed(int key);

	void setLogLevel(ofLogLevel logLevel);

private:
	void setupParams();
	void setupGui();
	void update();
	void update(ofEventArgs & args);

	//--

private:
	//void Changed(ofAbstractParameter & e);
	void ChangedPlane(ofAbstractParameter & e);
	void ChangedLight(ofAbstractParameter & e);
	void ChangedShadow(ofAbstractParameter & e);
	void ChangedInternal(ofAbstractParameter & e);

#ifdef SURFING__USE_CUBE_MAP
	void ChangedCubeMaps(ofAbstractParameter & e);
#endif

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	void ChangedDisplacers(ofAbstractParameter & e);
#endif

	ofEventListener listenerResetAll;

	//--

public:
	void setCameraPtr(ofCamera * camera_) {
		camera = camera_;
	}

public:
	ofParameterGroup parameters; //main container to expose to gui and to handle settings

	ofParameterGroup & getMaterialParameters() { //mainly to expose to external gui's like ImGui
		return material.parameters;
	};

	ofParameter<bool> bGui; //for global ui
	ofParameter<bool> bGui_ofxGui; //for ofxGui

	ofParameterGroup internalParams;

	ofParameterGroup testSceneParams;
	ofParameter<float> scaleTestScene;
	ofParameter<float> positionTestScene;
	ofParameter<void> resetTestScene;

	ofParameter<bool> bDebug;
	ofParameter<bool> bKeys;

	ofParameter<bool> bHelp;
	string sHelp;
	void buildHelpInfo();
	int helpLayout = 2; //top-right

	ofParameter<bool> bDrawPlane;

	ofParameterGroup planeParams;
	ofParameterGroup planeSettingsParams;
	ofParameterGroup planeTransformParams;
	ofParameterGroup lightParams;
	ofParameterGroup shadowParams;

	ofParameter<bool> bPlaneWireframe;
	ofParameter<bool> bPlaneInfinite; //will ignore planeSize
	//make the plane huge size to better "fit a full horizon line"
	ofParameter<glm::vec2> planeSize; //normalized
	ofParameter<glm::ivec2> planeResolution;
	ofParameter<float> planeRotation; //x axis
	ofParameter<float> planePosition; //y pos

	ofParameter<ofFloatColor> planeGlobalColor; //linked to the other colors
	ofParameter<ofFloatColor> planeDiffuseColor;
	ofParameter<ofFloatColor> planeSpecularColor;

	ofParameter<float> planeShiness;
	ofParameter<float> lightX;
	ofParameter<float> lightY;
	ofParameter<float> lightZ;
	ofParameter<ofFloatColor> lightAmbientColor;

	ofParameter<bool> bDrawShadow;
	ofParameter<bool> bDebugShadow;
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	//TODO: add other internal PBR params
	//ofParameter<float> shadowStrength;
	//ofParameter<glm::vec2> shadowSize;

	ofParameter<void> resetPlane;
	ofParameter<void> resetPlaneTransform;
	ofParameter<void> resetLight;
	ofParameter<void> resetShadow;
	ofParameter<void> resetAll;

private:
	ofxPanel gui;

public:
	// helper to improve layout with many gui panels.
	const ofRectangle getGuiShape() {
		ofRectangle r1 = gui.getShape();
		ofRectangle r2 = material.gui.getShape();
		ofRectangle bb = r1.getUnion(r2);
		return bb;
	};

	//--

private:
	ofCamera * camera;

private:
	callback_t f_RenderScene = nullptr;

public:
	void setFunctionRenderScene(callback_t f = nullptr) {
		f_RenderScene = f;
	};

private:
	ofPlanePrimitive plane;
	void refreshPlane();

	ofMaterial materialPlane;

	vector<shared_ptr<ofLight>> lights;

	//--

public:
	SurfingMaterial material;

public:
	void beginMaterial();
	void endMaterial();

	void beginMaterialPlane();
	void endMaterialPlane();

	//--

#ifdef SURFING__USE_CUBE_MAP
	ofCubeMap cubeMap;

	void setupCubeMap();
	string path_Cubemaps = "cubemaps"; //bin/data/cubemaps/
	string path_CubemapFilename = "modern_buildings_2_1k.exr"; //TODO: store path on settings
	bool bLoadedCubeMap = false;

	ofParameterGroup cubeMapParams;
	ofParameter<string> cubeMapName;
	ofParameter<int> cubeMapMode;
	ofParameter<string> cubeMapModeName;
	ofParameter<float> cubeMapprefilterRoughness;
	ofParameter<bool> bDrawCubeMap;
	ofParameter<ofFloatColor> bgAltColor;
	ofParameter<bool> bDrawBgAlt;

	ofParameter<void> openCubeMap;
	void processOpenFileSelection(ofFileDialogResult openFileResult);

	ofParameter<void> resetCubeMap;
	void doResetCubeMap();

public:
	bool loadCubeMap(string path = "");
#endif

	//--

	void doResetPlane();
	void doResetPlaneTransform();
	void doResetLight();
	void doResetShadow();
	void doResetAll(bool bExcludeMaterial = false);

	void doResetMaterial();
	void doRandomMaterial();
	void doRandomMaterialSettings();
	void doRandomMaterialColorGlobal();
	void doRandomMaterialColors();
	void doRandomMaterialColorsAlpha();
	void doRandomMaterialAlphas();

private:
	// History undo/redo
	ofParameterGroup historyParams;
	vector<ofParameterGroup> history;
	ofParameter<void> resetHistory;
	ofParameter<int> indexHistory;
	
	//--

private:
	string path = "ofxSurfingPBR_Scene.json";

public:
	void load();
	void save();

	bool getSettingsFileFound(); //to check if the app is opened for the first time
	// that allows to force customize the scene from ofApp at startup!

	SurfingAutoSaver autoSaver;

	//some app flow controls
	//bool bDoneSetup = false;
	bool bDoneSetupParams = false;
	bool bDoneStartup = false; //fixes some callback crashes at startup
	//bool bEnableCallbacks = true;

	//--

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
private:
	ofShader shaderPlane;
	bool bLoadedShaderPlane = false;
	void setupShaderPlane();
	ofFloatImage img;

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

	ofParameter<void> resetDisplace;
	ofParameter<void> resetNoise;
	void doResetNoise();
	void doResetDisplace();

	void setupParamsDisplace();
	void updateDisplace();
	void refreshImgShaderPlane();

	void setupPBRScene();
	void updatePBRScene();
	void drawPBRScene();
	void drawPBRSceneDebug();

public:
	void beginShaderPlane();
	void endShaderPlane();
#endif
};