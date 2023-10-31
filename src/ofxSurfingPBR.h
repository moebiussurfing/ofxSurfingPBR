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
	- add ImGui mode.
	- add presets manager 
		and randomizer/undo/redo 
		to explore and save materials.
	- add ofxBgGradient addon

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

#define SURFING__USE_SHADER_AND_DISPLACERS // TODO/WIP enable to test custom shader and displacement

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
	void drawTestScene();
	void drawGui();
	void exit();
	void keyPressed(int key);

private:
	void setupParams();
	void setupGui();
	void update();
	void update(ofEventArgs & args);

	void Changed(ofAbstractParameter & e);

private:
	string path = "scene.json";

public:
	// autosave workflow
	// we will autosave after on every param change,
	// but only once per frame, reducing saving overflow.
	// we will save also when app exit.
	ofParameter<bool> bAutoSave;
	void load();
	void save();

private:
	uint64_t timeLastChange = 0;
	int timeSaveGap = 1000; //save every x milliseconds.
	bool bFlagSave = false;

public:
	void setCameraPtr(ofCamera * camera_) {
		camera = camera_;
	}

private:
	ofParameter<glm::vec2> planeSize;
	ofParameter<bool> bPlaneWireframe;
	ofParameter<bool> bPlaneInfinite; //make the plane huge size to "fit horizon line"
	ofParameter<float> planeRotation;
	ofParameter<float> planePosition;
	ofParameter<float> planeShiness;
	ofParameter<ofFloatColor> planeDiffuseColor;
	ofParameter<ofFloatColor> planeSpecularColor;
	ofParameter<ofFloatColor> planeGlobalColor;
	ofParameterGroup planeSettingsParams;
	ofParameter<void> resetPlane;
	ofParameter<void> resetPlaneTransform;
	void refreshPlane();

private:
	ofMaterial materialPlane;
	ofParameterGroup planeParams;
	ofParameterGroup planeTransform;

private:
	ofParameterGroup lightParams;
	ofParameter<float> lightX;
	ofParameter<float> lightY;
	ofParameter<float> lightZ;
	ofParameter<void> resetLight;
	ofParameter<ofFloatColor> lightAmbientColor;

	ofParameterGroup shadowParams;
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	ofParameter<bool> bDrawShadow;
	ofParameter<void> resetShadow;

	ofParameterGroup parameters;

public:
	ofParameter<bool> bDebug;
	ofParameter<bool> bKeys;
	ofParameter<bool> bDrawPlane;

public:
	ofxPanel gui;

	const ofRectangle getGuiShape() {
		ofRectangle r1 = gui.getShape();
		ofRectangle r2 = material.gui.getShape();
		ofRectangle bb = r1.getUnion(r2);
		return bb;
	};
	ofParameter<void> resetAll;

private:
	SurfingMaterial material;

	ofCamera * camera;

	vector<shared_ptr<ofLight>> lights;

	ofPlanePrimitive plane;

	//--

public:
	void beginMaterial();
	void endMaterial();

	void beginMaterialPlane();
	void endMaterialPlane();

private:
	callback_t function_RenderScene = nullptr;

public:
	void setFunction_renderScene(callback_t f = nullptr) {
		function_RenderScene = f;
	};

	//--

#ifdef SURFING__USE_CUBE_MAP
	string path_Cubemaps = "cubemaps";
	string path_CubemapFilename = "modern_buildings_2_1k.exr";
	ofCubeMap cubeMap;
	ofParameterGroup cubeMapParams;
	ofParameter<int> cubeMapMode;
	ofParameter<string> cubeMapModeName;
	ofParameter<float> cubeMapprefilterRoughness;
	ofParameter<bool> bDrawCubeMap;
	ofParameter<void> openCubeMap; //TODO
	ofParameter<void> resetCubeMap;
	void setupCubeMap();
	void doResetCubeMap();
	void processOpenFileSelection(ofFileDialogResult openFileResult);

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

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
private:
	ofShader shader;
	void setupShader();
	bool bShaderReady = false;
	ofFloatImage img;

	ofParameterGroup displacersParams;
	ofParameterGroup displaceMaterialParams;
	ofParameterGroup noiseParams;
	ofParameter<void> resetDisplace;
	ofParameter<void> resetNoise;
	ofParameter<bool> bShaderToPlane;
	ofParameter<bool> bDisplaceToMaterial;
	ofParameter<float> noiseAmplitude;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseSpeed;
	ofParameter<float> displacementStrength;
	ofParameter<float> displacementNormalsStrength;
	ofParameter<float> normalGeomToNormalMapMix;
	void setupParamsDisplace();
	void doResetNoise();
	void doResetDisplace();
	void updateDisplace();
	void refreshImg();

public:
	void beginShader();
	void endShader();
#endif
};
