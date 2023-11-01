#pragma once
#include "ofMain.h"

#include "SurfingConstantsPBR.h"

#include "SurfingHelpersLite.h"
#include "ofxGui.h"

class SurfingMaterial {
public:
	SurfingMaterial();
	~SurfingMaterial();

	void setup();
	void drawGui();
	void exit();

private:
	void update();
	void update(ofEventArgs & args);

	void setupParams();
	void setupGui();
	void Changed(ofAbstractParameter & e);

public:
	void begin();
	void end();

	void doResetMaterial();
	void doRandomMaterial();
	void doRandomColors();
	void doRandomAlphas();
	void doRandomColorsAlpha();
	void doRandomColorGlobal();
	void doRandomSettings();

	ofMaterial material;

public:
	ofParameterGroup parameters;
	ofParameterGroup colorParams;
	ofParameterGroup settingsParams;
	ofParameterGroup coatParams;
	ofParameterGroup helpersParams;

	ofParameter<ofFloatColor> globalColor; //will set all colors except touching their alphas.
	ofParameter<float> globalAlpha; //will set this alpha to all colors.

	ofParameter<ofFloatColor> diffuseColor;
	ofParameter<ofFloatColor> specularColor;
	ofParameter<ofFloatColor> ambientColor;
	ofParameter<ofFloatColor> emissiveColor;

	ofParameter<float> shininess;
	ofParameter<float> metallic;
	ofParameter<float> reflectance;
	ofParameter<float> roughness;

	ofParameter<bool> bClearCoat;
	ofParameter<float> clearCoatRoughness;
	ofParameter<float> clearCoatStrength;

	ofParameter<void> resetMaterial;
	ofParameter<void> randomMaterial;
	ofParameter<void> randomColors;
	ofParameter<void> randomAlphas;
	ofParameter<void> randomColorsAlpha;
	ofParameter<void> randomColorsGlobal;
	ofParameter<void> randomSettings;

	ofxPanel gui;

public:
	string path = "ofxSurfingPBR_Material.json";

public:
	void load();
	void save();

#ifdef SURFING__USE_AUTOSAVE_ENGINE
public:
	// autosave workflow
	// we will autosave after every param change,
	// but after waiting some ms. reducing saving overflow.
	// we will save also when app exit.
	ofParameter<bool> bAutoSave;

private:
	uint64_t timeLastChange = 0;
	int timeSaveDelay = 1000; //save delayed x milliseconds.
	bool bFlagSave = false;
#endif
};
