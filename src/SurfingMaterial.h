#pragma once
#include "ofMain.h"

#include "ofxGui.h"

//#define SURFING__USE_DISPLACE // enable to test custom shader and displacement

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

private:
	void setupGui();

	void update();
	void update(ofEventArgs & args);

	string path = "material.xml";

	ofMaterial material;

public:
	ofParameterGroup parameters;

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
	ofParameter<bool> clearCoat;
	
	ofParameter<void> resetMaterial{
		"Reset material"
	};
	
	ofxPanel gui;
};
