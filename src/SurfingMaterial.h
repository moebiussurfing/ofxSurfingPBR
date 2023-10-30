#pragma once
#include "ofMain.h"

#include "ofxGui.h"

//#define SURFING__USE_DISPLACE 
// TODO/WIP enable to test custom shader and displacement

#define SURFING__SZ_UNIT 1000

class SurfingMaterial {
public:
	SurfingMaterial();
	~SurfingMaterial();

	void setup();
	void exit();
	void drawGui();
	void Changed(ofAbstractParameter & e);

	void begin();
	void end();

	void doResetMaterial();
	void doRandomMaterial();
	void doRandomColors();
	void doRandomColorsAlpha();
	void doRandomColorGlobal();
	void doRandomSettings();

private:
	void setupGui();

	void update();
	void update(ofEventArgs & args);

	string path = "material.json";

	ofMaterial material;

public:
	ofParameterGroup parameters;
	ofParameterGroup colorParams;
	ofParameterGroup settingsParams;
	ofParameterGroup coatParams;

#ifdef SURFING__USE_DISPLACE
	ofPlanePrimitive plane;
	ofShader shader;
	ofFloatImage img;

	ofParameter<bool> useMaterial;

	ofParameter<float> displacementStrength;
	ofParameter<float> displacementNormalsStrength;
	ofParameter<float> normalGeomToNormalMapMix;

	ofParameter<float> noiseAmplitude;
	ofParameter<float> noiseScale;
	ofParameter<float> noiseSpeed;

	ofParameterGroup planeParams;
#endif

	ofParameter<ofFloatColor> globalColor;
	ofParameter<ofFloatColor> diffuseColor;
	ofParameter<ofFloatColor> specularColor;
	ofParameter<ofFloatColor> ambientColor;
	ofParameter<ofFloatColor> emissiveColor;
	ofParameter<void> randomColors;
	ofParameter<void> randomColorsAlpha;
	ofParameter<void> randomColorsGlobal;

	ofParameter<float> shininess;
	ofParameter<float> metallic;
	ofParameter<float> reflectance;
	ofParameter<float> roughness;
	ofParameter<void> randomSettings;

	ofParameter<float> clearCoatRoughness;
	ofParameter<float> clearCoatStrength;
	ofParameter<bool> clearCoat;
	
	ofParameter<void> resetMaterial;
	ofParameter<void> randomMaterial;
	
	ofxPanel gui;
};
