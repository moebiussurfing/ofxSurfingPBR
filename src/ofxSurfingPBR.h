/*

	All this code is copied and/or hardly based on the OF forum topic: 
	https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19
	authors: @NickHardeman and @Paolo

	This class have two materials ready to use.
	- One for the plane/floor were the shadows are drawn.
	- Another material to be used to apply to other 3d object/mesh.

	There's some commented code for adding a custom shader.


	TODO:
	- make a scene manager allowing
		to queue many materials 
		and lights on a std::vector.
		something like ofxPBR or ofxPBRHelper.
	- add ImGui mode.
	- add presets manager 
		and randomizer/undo/redo 
		to explore and save materials.
	- add ofxBgGradient addon 
		to draw a cool Bg instead of the cubeMap.

*/

/*

	SIMPLE EXAMPLE

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
		// floor plane
		pbr.drawPlane();

		// objects
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
#define SURFING__USE_CUBE_MAP
//#define SURFING__USE_SHADER_AND_DISPLACERS //WIP 
//#define SURFING__CLAMP_PLANE_SIZE_BC_PERFORMANCE //TODO

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
	void exit();
	void keyPressed(int key);

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

private:
	ofPlanePrimitive plane;

	ofParameterGroup planeParams;
	ofParameterGroup planeSettingsParams;
	ofParameterGroup planeTransform;

	ofParameter<bool> bPlaneWireframe;
	ofParameter<bool> bPlaneInfinite;//make the plane huge size to "fit horizon line"
	ofParameter<glm::vec2> planeSize;//normalized
	ofParameter<float> planeRotation;
	ofParameter<float> planePosition;
	ofParameter<float> planeShiness;
	ofParameter<ofFloatColor> planeDiffuseColor;
	ofParameter<ofFloatColor> planeSpecularColor;
	ofParameter<ofFloatColor> planeGlobalColor;

	ofParameter<void> resetPlane;
	ofParameter<void> resetPlaneTransform;
	void refreshPlane();

private:
	ofMaterial materialPlane;

private:
	vector<shared_ptr<ofLight>> lights;

	ofParameterGroup lightParams;
	ofParameter<float> lightX;
	ofParameter<float> lightY;
	ofParameter<float> lightZ;
	ofParameter<ofFloatColor> lightAmbientColor;
	ofParameter<void> resetLight;

	ofParameterGroup shadowParams;
	ofParameter<bool> bDrawShadow;
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	//ofParameter<glm::vec2> shadowSize;
	ofParameter<void> resetShadow;

	ofParameterGroup parameters;
	ofParameter<void> resetAll;

public:
	ofParameter<bool> bDebug;
	ofParameter<bool> bKeys;
	ofParameter<bool> bDrawPlane;

public:
	ofxPanel gui;

	// helper to improve layout with many gui panels.
	const ofRectangle getGuiShape() {
		ofRectangle r1 = gui.getShape();
		ofRectangle r2 = material.gui.getShape();
		ofRectangle bb = r1.getUnion(r2);
		return bb;
	};

private:
	ofCamera * camera;

private:
	callback_t function_RenderScene = nullptr;

public:
	void setFunction_renderScene(callback_t f = nullptr) {
		function_RenderScene = f;
	};

private:
	SurfingMaterial material;

	//--

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
	string path_CubemapFilename = "modern_buildings_2_1k.exr";

	ofParameterGroup cubeMapParams;
	ofParameter<int> cubeMapMode;
	ofParameter<string> cubeMapModeName;
	ofParameter<float> cubeMapprefilterRoughness;
	ofParameter<bool> bDrawCubeMap;
	
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

	void doResetMaterial() {
		material.doResetMaterial();
	};
	void doRandomMaterial() {
		material.doRandomMaterial();
	};
	void doRandomMaterialColors() {
		material.doRandomColors();
	};
	void doRandomMaterialColorsAlpha() {
		material.doRandomColorsAlpha();
	};
	void doRandomMaterialSettings() {
		material.doRandomSettings();
	};

	//--

private:
	string path = "scene.json";

public:
	// autosave workflow
	// we will autosave after on every param change,
	// but after waiting some ms. reducing saving overflow.
	// we will save also when app exit.
	ofParameter<bool> bAutoSave;

	void load();
	void save();

private:
	uint64_t timeLastChange = 0;
	int timeSaveGap = 1000; //save every x milliseconds.
	bool bFlagSave = false;

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
private:
	ofShader shader;
	ofFloatImage img;
	void setupShader();
	bool bShaderReady = false;

	ofParameterGroup displacersParams;
	ofParameterGroup displaceMaterialParams;
	ofParameterGroup noiseParams;

	ofParameter<void> resetDisplace;
	ofParameter<void> resetNoise;
	void doResetNoise();
	void doResetDisplace();

	ofParameter<bool> bShaderToPlane;
	ofParameter<bool> bDisplaceToMaterial;

	ofParameter<float> noiseAmplitude;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseSpeed;

	ofParameter<float> displacementStrength;
	ofParameter<float> displacementNormalsStrength;
	ofParameter<float> normalGeomToNormalMapMix;

	void setupParamsDisplace();
	void updateDisplace();
	void refreshShaderImg();

public:
	void beginShaderPlane();
	void endShaderPlane();
#endif
};
