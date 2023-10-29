/*

	EXAMPLE

	void ofApp::setup() {
		pbr.setup();
		pbr.setCameraPtr(&camera);

		// render scene
		callback_t myFunctionDraw = std::bind(&ofApp::renderScene, this);
		pbr.setFunction_renderScene(myFunctionDraw);
	}

	void ofApp::renderScene()
	{
		// plane
		pbr.drawPlane();

		// object
		pbr.beginMaterial();
		{
			ofDrawBox(0, 0, 0, 100);
		}
		pbr.endMaterial();
	}

*/


#pragma once
#include "ofMain.h"

#include "SurfingMaterial.h"
#include "ofxGui.h"

#include <functional>
using callback_t = std::function<void()>;

class ofxSurfingPBR
{
public:
	ofxSurfingPBR();
	~ofxSurfingPBR();

public:
	void setup();
	void draw();
	void drawGui();
	void exit();

private:
	void setupGui();
	void update();
	void update(ofEventArgs & args);

	void Changed(ofAbstractParameter & e);

private:
	string path = "scene.xml";

public:
	void setCameraPtr(ofCamera * camera_) {
		camera = camera_;
	}

private:
	ofCamera * camera;

	vector<shared_ptr<ofLight>> lights;

public:
	ofPlanePrimitive plane;
	ofParameter<glm::vec2> planeSz;
	ofParameter<bool> bDrawPlane;
	ofParameter<bool> wireframe;
	ofParameter<float> planeRot;
	ofParameter<float> planePos;
	ofParameter<float> planeDiffuse;
	ofParameter<float> planeShiness;
	ofParameter<float> planeSpecular;
	ofParameter<void> resetPlane {
		"Reset plane"
	};
	void doResetPlane();
	void drawPlane();

private:
	ofMaterial materialPlane;
	ofParameterGroup planeParams;
	ofParameterGroup planeTransform;

#ifdef SURFING__USE_DISPLACE
private:
	ofShader shader;
	ofFloatImage img;
private:
	ofParameter<float> noiseAmplitude;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseSpeed;
#endif

private:
	ofParameterGroup lightParams;
	ofParameter<float> x;
	ofParameter<float> y;
	ofParameter<float> z;
	ofParameter<void> resetLights{
		"Reset lights"
	};
	void doResetLights();

	ofParameterGroup shadowParams;
	ofParameter<float> shadowBias;
	ofParameter<bool> bDebug;
	ofParameter<float> shadowNormalBias;
	ofParameter<bool> bEnableShadow;
	ofParameter<void> resetShadow{
		"Reset shadow"
	};
	void doResetShadow();

	ofParameterGroup parameters;

	SurfingMaterial material;

	ofxPanel gui;

	//--

public:
	void beginMaterial();
	void endMaterial();

	void beginMaterialPlane();
	void endMaterialPlane();

	callback_t function_RenderScene = nullptr;
	void setFunction_renderScene(callback_t f = nullptr) {
		function_RenderScene = f;
	};
};
