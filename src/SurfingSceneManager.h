#pragma once

#include "ofMain.h"

#include "SurfingMaterial.h"
#include "SurfingLights.h"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"

class SurfingSceneManager {
	public:

	SurfingSceneManager();
	~SurfingSceneManager();
	
	void setup();
	void update();
	void draw();
	void drawGui();
	void exit();

private:
	callback_t f_RenderScene = nullptr;
public:
	void setFunctionRenderScene(callback_t f = nullptr);

	void addMaterial(string name);
	void addColor(ofFloatColor color);
	void setColor(ofFloatColor color, int index);
	void setColorMaterial(ofFloatColor color, int index);
	void beginMaterial(int index);
	void endMaterial(int index);

	void clearColors();
	void clearMaterials();


	ofxPanel gui;
	ofParameterGroup parameters;
	ofParameterGroup parametersMaterials;
	ofParameterGroup parametersColors;

	SurfingLights surfingLights;
	void beginLights();
	void endLights();

	vector<std::unique_ptr<SurfingMaterial>> materials;
	vector<std::unique_ptr<ofParameter<ofFloatColor>>> colors;

};
