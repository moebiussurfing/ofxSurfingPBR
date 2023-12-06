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

	void setupBuild(); // must be called after adding materials and colors before!

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

	void clearMaterials();

	void addMaterial(string name = "");

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	void clearColors();
	void addColor(ofFloatColor color, string name = "");
#endif

	void setColor(ofFloatColor color, int index);
	void setColorMaterial(ofFloatColor color, int index);

	void beginMaterial(int index);
	void endMaterial(int index);

	ofxPanel gui;
	ofParameterGroup parameters;

	ofParameterGroup materialsControlParams;
	ofxPanel guiMaterials;

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
private:
	ofParameterGroup colorsControlParams;
	ofxPanel guiColors;
	ofParameterGroup colorsParams;
#endif

private:
	ofParameterGroup materialsParams;

private:
	SurfingLights lights;

public:
	void updateLights();
	void beginLights();
	void endLights();
	void drawDebugLights();

	void toggleDebug();

	ofParameter<bool>bGui_Materials;
	ofParameter<int>indexMaterial;
	ofParameter<string>nameIndexMaterial;
	ofEventListener listenerIndexMaterial;

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	ofParameter<bool>bGui_Colors;
	ofParameter<int> indexColor;
	ofParameter<string>nameIndexColor;
	ofEventListener listenerIndexColor;
	vector<std::unique_ptr<ofParameter<ofFloatColor>>> colors;
#endif

private:
	vector<std::unique_ptr<SurfingMaterial>> materials;

	string path = "SurfingSceneManager.json";
};
