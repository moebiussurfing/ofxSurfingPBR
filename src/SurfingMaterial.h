#pragma once
#include "ofMain.h"

#include "ofxGui.h"

class SurfingMaterial {
public:
	SurfingMaterial();
	~SurfingMaterial();

	void setup();
	void drawGui();
	void exit();

	void begin();
	void end();

	void doResetMaterial();
	void doRandomMaterial();
	void doRandomColors();
	void doRandomColorsAlpha();
	void doRandomColorGlobal();
	void doRandomSettings();

private:
	void setupParams();
	void setupGui();
	void Changed(ofAbstractParameter & e);
	void update();
	void update(ofEventArgs & args);

public:
	ofMaterial material;

public:
	ofParameterGroup parameters;
	ofParameterGroup colorParams;
	ofParameterGroup settingsParams;
	ofParameterGroup coatParams;
	ofParameterGroup helpersParams;

	ofParameter<ofFloatColor> globalColor;
	ofParameter<ofFloatColor> diffuseColor;
	ofParameter<ofFloatColor> specularColor;
	ofParameter<ofFloatColor> ambientColor;
	ofParameter<ofFloatColor> emissiveColor;

	ofParameter<float> shininess;
	ofParameter<float> metallic;
	ofParameter<float> reflectance;
	ofParameter<float> roughness;

	ofParameter<float> clearCoatRoughness;
	ofParameter<float> clearCoatStrength;

	ofParameter<void> randomColors;
	ofParameter<void> randomColorsAlpha;
	ofParameter<void> randomColorsGlobal;
	ofParameter<void> randomSettings;
	ofParameter<bool> clearCoat;
	ofParameter<void> resetMaterial;
	ofParameter<void> randomMaterial;

	ofxPanel gui;

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
	string path = "material.json";
};
