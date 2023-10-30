/*

	All this code is copied and/or hardly based on the OF forum topic: 
	https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19
	authors: @NickHardeman and @Paolo

	This class have two materials ready to use.
	- One for the plane/floor were the shadows are drawn.
	- Another material to be used to apply to other 3d object/mesh.

	There's some commented code for adding a custom shader.


	TODO:
	- make a scene manager allowing to queue many materials and lights on a std::vector.
	- add ImGui.
	- add presets manager and randomizer/undo/redo to explore and save materials.

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

// OPTIONAL
#define SURFING__USE_CUBE_MAP

#include "SurfingMaterial.h"
#include "ofxGui.h"

#include <functional>
using callback_t = std::function<void()>;

class ofxSurfingPBR {
public:
	ofxSurfingPBR();
	~ofxSurfingPBR();

public:
	void setup();
	void draw();
	void drawPlane();
	void drawGui();
	void exit();

private:
	void setupParams();
	void setupGui();
	void update();
	void update(ofEventArgs & args);

	void Changed(ofAbstractParameter & e);

private:
	string path = "scene.json";

public:
	void setCameraPtr(ofCamera * camera_) {
		camera = camera_;
	}

private:
	ofParameter<glm::vec2> planeSz;
	ofParameter<bool> bDrawPlane;
	ofParameter<bool> wireframe;
	ofParameter<float> planeRot;
	ofParameter<float> planePos;
	ofParameter<float> planeShiness;
	ofParameter<ofFloatColor> planeDiffuseColor;
	ofParameter<ofFloatColor> planeSpecularColor;
	ofParameter<ofFloatColor> globalColor;
	ofParameterGroup planeSettingsParams;
	ofParameter<void> resetPlane;
	ofParameter<void> resetPlaneTransform;

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

	ofParameterGroup shadowParams;
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	ofParameter<bool> bEnableShadow;
	ofParameter<void> resetShadow;

	ofParameterGroup parameters;

public:
	ofParameter<bool> bDebug;

public:
	ofxPanel gui;

	const ofRectangle getGuiShape() {
		auto r1 = material.gui.getShape();
		auto r2 = gui.getShape();
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
	void doResetcubeMap();
	void processOpenFileSelection(ofFileDialogResult openFileResult);

public:
	bool loadCubeMap(string path = "");
#endif

	//--

	void doResetPlane();
	void doResetPlaneTransform();
	void doResetLight();
	void doResetShadow();
	void doResetAll();

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

#ifdef SURFING__USE_DISPLACE
private:
	ofShader shader;
	ofFloatImage img;

private:
	ofParameter<float> noiseAmplitude;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseSpeed;
#endif
};
