/*

	All this code is copied and/or hardly based on the OF forum topic: 
	https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19
	authors: @NickHardeman and @Paolo


	TODO:
	- add cubemap file path to settings
	- make a scene manager allowing
		to queue many materials 
		and lights on a std::vector.
		something like ofxPBR or ofxPBRHelper.
	- add ImGui mode.
	- add presets manager 
		and randomizer/undo/redo 
		to explore and save materials.
	- add ofxBgGradient addon 
		or a simple Bg color
		to draw a cool Bg instead of the cubeMap.
	- add custom gloabal path to /data/ofxSurfingPBR/

*/

/*

	SIMPLE EXAMPLE

	//ofApp.h
	
	#include "ofxSurfingPBR.h"
	
	ofEasyCam camera;
	
	ofxSurfingPBR pbr;
	void renderScene();

	//ofApp.cpp

	void ofApp::setup() {
		pbr.setup();

		// pass the local camera
		pbr.setCameraPtr(&camera);

		// pass the render scene function
		callback_t myFunctionDraw = std::bind(&ofApp::renderScene, this);
		pbr.setFunction_renderScene(myFunctionDraw);
	}

	void ofApp::renderScene()
	{
		// plane floor
		pbr.drawPlane();

		// other objects
		pbr.beginMaterial();
		{
			ofDrawBox(0, 0, 0, 100);
		}
		pbr.endMaterial();
	}

*/

//--

#pragma once
#include "ofMain.h"

//--

// Optional

//#define SURFING__USE_SHADER_AND_DISPLACERS // TODO: WIP: add another shader/material.
//#define SURFING__CLAMP_PLANE_SIZE_BC_PERFORMANCE // TODO: testing to improve performance

#define SURFING__USE_CUBE_MAP

//--

// Constants
#define SURFING__PLANE_RESOLUTION 10
#define SURFING__PLANE_INFINITE_MAGNITUDE 100.f
#define SURFING__SZ_UNIT 1000

//--

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
	void draw();
	void drawPlane();
	void drawTestScene();//a simple scene for easy testing
	void drawGui();
	void drawDebug();
	void exit();
	void keyPressed(int key);

	void setLogLevel(ofLogLevel logLevel);

private:
	void setupParams();
	void setupGui();
	void update();
	void update(ofEventArgs & args);
	void Changed(ofAbstractParameter & e);

public:
	void setCameraPtr(ofCamera * camera_) {
		camera = camera_;
	}

public:
	ofParameterGroup parameters;//main container to expose to gui and to handle settings

	ofParameterGroup & getMaterialParameters() {//mainly to expose to external gui's like ImGui
		return material.parameters;
	};

	ofParameter<bool> bGui; //for global ui
	ofParameter<bool> bGui_ofxGui; //for ofxGui

	ofParameter<bool> bDebug;
	ofParameter<bool> bKeys;
	ofParameter<bool> bDrawPlane;

	ofParameterGroup planeParams;
	ofParameterGroup planeSettingsParams;
	ofParameterGroup planeTransformParams;
	ofParameterGroup lightParams;
	ofParameterGroup shadowParams;

	ofParameter<bool> bPlaneWireframe;
	ofParameter<bool> bPlaneInfinite;//make the plane huge size to better "fit a full horizon line"
	ofParameter<glm::vec2> planeSize;//normalized
	ofParameter<float> planeRotation;//x axis
	ofParameter<float> planePosition;//y pos

	ofParameter<ofFloatColor> planeGlobalColor;//linked to the other colors
	ofParameter<ofFloatColor> planeDiffuseColor;
	ofParameter<ofFloatColor> planeSpecularColor;
	ofParameter<float> planeShiness;

	ofParameter<void> resetPlane;
	ofParameter<void> resetPlaneTransform;
	
	ofParameter<void> resetLight;
	ofParameter<void> resetShadow;
	ofParameter<void> resetAll;

	ofParameter<float> lightX;
	ofParameter<float> lightY;
	ofParameter<float> lightZ;
	ofParameter<ofFloatColor> lightAmbientColor;

	ofParameter<bool> bDrawShadow;
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	//TODO: add other internal PBR params
	//ofParameter<float> shadowStrength;
	//ofParameter<glm::vec2> shadowSize;

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
	callback_t function_RenderScene = nullptr;

public:
	void setFunction_renderScene(callback_t f = nullptr) {
		function_RenderScene = f;
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
	string path_Cubemaps = "cubemaps";//bin/data/cubemaps/
	string path_CubemapFilename = "modern_buildings_2_1k.exr";//TODO: store path on settings
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

	//--

private:
	string path = "scene.json";

public:
	void load();
	void save();

	bool getSettingsFileFound();//to check if the app is opened for the first time
	// that allows to force customize the scene from ofApp at startup!
	
	// autosave workflow
	// we will autosave after every param change,
	// but after waiting some ms. reducing saving overflow.
	// we will save also when app exit.
	ofParameter<bool> bAutoSave;

private:
	uint64_t timeLastChange = 0;
	int timeSaveDelay = 1000; //save delayed x milliseconds.
	bool bFlagSave = false;

	//some app flow controls
	//bool bDoneSetup = false;
	bool bDoneStartup = false;//fixes some callback crashes at startup

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
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

public:
	void beginShaderPlane();
	void endShaderPlane();
#endif
};
