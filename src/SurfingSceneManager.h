/*
* SurfingSceneManager.h
* This class is a new WIP scene manager to queue lights and materials
* to be used easily into a clean project.
* Do not requires to include the main ofxSurfingPBR.h.
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

	void setupBuild(); // must be called afer adding materials and colors before!

private:
	void setupParams();
	void setupGui();
	void refreshGui(bool bHard=false);
	void startup();

	void draw();

public:
	void drawGui();
	void exit();

private:
	bool bDoneExit = false;

private:
	callback_t f_RenderScene = nullptr;

public:
	void setFunctionRenderScene(callback_t f = nullptr);

	void clearColors();
	void clearMaterials();

	void addMaterial(string name = "");
	void addColor(ofFloatColor color, string name = "");

	void setColor(ofFloatColor color, int index);
	void setColorMaterial(ofFloatColor color, int index);

	void beginMaterial(int index);
	void endMaterial(int index);

	ofxPanel gui;
	ofParameterGroup parameters;

	ofParameterGroup materialsControlParams;
	ofParameterGroup colorsControlParams;

	ofxPanel guiMaterials;
	ofxPanel guiColors;

private:
	ofParameterGroup materialsParams;
	ofParameterGroup colorsParams;

private:
	SurfingLights lights;

public:
	void updateLights();
	void beginLights();
	void endLights();
	void drawDebugLights();

	void toggleDebug();

	ofParameter<bool>bGui_Materials;
	ofParameter<bool>bGui_Colors;
	ofParameter<int>indexMaterial;
	ofParameter<string>nameIndexMaterial;
	ofParameter<int> indexColor;
	ofParameter<string>nameIndexColor;
	ofEventListener listenerIndexColor;
	ofEventListener listenerIndexMaterial;

private:
	vector<std::unique_ptr<SurfingMaterial>> materials;
	vector<std::unique_ptr<ofParameter<ofFloatColor>>> colors;

	string path = "SurfingSceneManager.json";
};
