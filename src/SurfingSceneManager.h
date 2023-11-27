/*
* SurfingSceneManager.h
* This class is a new WIP scene manager to queue lights and materials
* to be used easily into a clean project.
* Do not requires to include the main ofxSurfingPBR.h
*/

#pragma once

#include "ofMain.h"

#include "SurfingLights.h"
#include "SurfingMaterial.h"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"

class SurfingSceneManager {
public:
	SurfingSceneManager();
	~SurfingSceneManager();

	void setup();

	void updateLights();

private:
	void draw();

public:
	void drawGui();
	void exit();

private:
	callback_t f_RenderScene = nullptr;

public:
	void setFunctionRenderScene(callback_t f = nullptr);

	void clearColors();
	void clearMaterials();

	void addMaterial(string name);
	void addColor(ofFloatColor color);

	void setColor(ofFloatColor color, int index);
	void setColorMaterial(ofFloatColor color, int index);

	void beginMaterial(int index);
	void endMaterial(int index);

	ofxPanel gui;
	ofParameterGroup parameters;

private:
	ofParameterGroup parametersMaterials;
	ofParameterGroup parametersColors;

private:
	SurfingLights lights;

public:
	void beginLights();
	void endLights();
	void drawDebugLights();

private:
	vector<std::unique_ptr<SurfingMaterial>> materials;
	vector<std::unique_ptr<ofParameter<ofFloatColor>>> colors;

	string path = "SurfingSceneManager.json";
};
