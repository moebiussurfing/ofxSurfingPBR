/*

	move shadow here
	add global color
	fix rotate glm
	resets
	simple mode
	fix debug

*/

//--

#pragma once
#include "ofMain.h"

//  SurfingLights.hpp
//  Originally Created by Hiromitsu Iwanaka on 2018/05/26.
//  Hardly modified and updated by moebiusSurfing, 2021.

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"
#include "ofxSurfingPBRConstants.h"

//--

class SurfingLights {
public:
	SurfingLights();
	~SurfingLights();

private:
public:
	// lights
	vector<shared_ptr<ofLight>> lights;
	ofParameter<bool> bDebug;
	ofParameter<bool> bDebugShadow;
	callback_t f_RenderScene = nullptr;
	void setFunctionRenderScene(callback_t f = nullptr);
	ofParameter<void> vResetLights;

public:
	void setupLights();
	void updateLights();
	void drawLights();
	void drawDebugPBRlights();

private:
	ofLight pointLight;
	ofLight spotLight;
	ofLight directionalLight;

public:
	void setup();
	void exit();

	//void draw();
	//void drawScene(bool bInCam = true);

private:
	void setupParameters();
	void startup();

private:
	void setupGui();
	void refreshGui();

public:
	void begin();
	void end();

public:
	ofxPanel gui;
	void setGuiPosition(glm::vec2 pos);
	void drawGui();
	ofParameter<bool> bGui;
	
	ofxPanel guiShadows;
	ofParameter<bool> bGui_Shadows;


public:
	int mouseX, mouseY;

	//-

	ofParameterGroup parameters;

	void Changed(ofAbstractParameter & e);
	void ChangedLights(ofAbstractParameter & e);
	void ChangedBrights(ofAbstractParameter & e);

	ofParameterGroup params_User;
	ofParameterGroup params_Enablers;
	ofParameterGroup params_Extra;
	ofParameterGroup params_Lights;
	ofParameterGroup params_Brights;

	//--

	ofParameterGroup pointParams;
	ofParameterGroup pointColorsParams;
	ofParameter<bool> bPoint;
	ofParameter<void> vPointReset;
	ofParameter<float> pointBright;
	ofParameter<ofFloatColor> pointLightAmbientColor;
	ofParameter<ofFloatColor> pointLightDiffuseColor;
	ofParameter<ofFloatColor> pointLightSpecularColor;
	ofParameter<glm::vec3> pointLightPosition;
	ofParameter<ofFloatColor> pointLightGlobalColor;

	ofParameterGroup spotLightParams;
	ofParameterGroup spotLightColorsParams;
	ofParameter<bool> bSpot;
	ofParameter<void> vSpotReset;
	ofParameter<float> spotBright;
	ofParameter<ofFloatColor> spotLightAmbientColor;
	ofParameter<ofFloatColor> spotLightDiffuseColor;
	ofParameter<ofFloatColor> spotLightSpecularColor;
	ofParameter<ofFloatColor> spotLightGlobalColor;

	ofParameter<glm::vec3> spotLightOrientation;
	ofParameter<glm::vec3> spotLightPosition;
	ofParameter<int> spotLightCutOff;
	ofParameter<int> spotLightConcentration;

	ofParameterGroup directionalLightParams;
	ofParameterGroup directionalColorsParams;
	ofParameter<bool> bDirectional;
	ofParameter<void> vDirectionalReset;
	ofParameter<float> directionalBright;
	ofParameter<ofFloatColor> directionalLightAmbientColor;
	ofParameter<ofFloatColor> directionalLightDiffuseColor;
	ofParameter<ofFloatColor> directionalLightSpecularColor;
	ofParameter<ofFloatColor> directionalLightGlobalColor;
	ofParameter<glm::vec3> directionalLightOrientation;
	ofParameter<glm::vec3> directionalLightPosition;


public:
	ofParameter<bool> bAnimLights;
	void updateAnims();
	ofParameter<bool> bDebugLights ;
	//void drawDebug();

private:
	ofParameter<bool> bSmoothLights;

	ofVec3f center;

public:
	void beginLights();
	void endLights();

	// Resets
public:
	void doResetLights();
//	void doResetAll();
private:
	void doResetPoint();
	void doResetSpot();
	void doResetDirectional();

	string pathSettings = "ofxSurfingPBR_Lights.json";
	string pathSettingsShadows = "ofxSurfingPBR_Shadows.json";

public:
	void load();
	void save();

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
private:
	SurfingAutoSaver autoSaver;
#endif

	//--

private:
	void refreshBrights();
	void refreshBrightPoint();
	void refreshBrightDirect();
	void refreshBrightSpot();

	//----

	ofParameterGroup shadowParams;

public:
	ofParameter<bool> bDrawShadow;
	//ofParameter<bool> bDebugShadow;
	
private:
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	ofParameter<void> vResetShadow;

	//TODO: OF PBR
	// add other internal OF params
	//ofParameter<float> shadowStrength;
	//ofParameter<glm::vec2> shadowSize;

public:
	void doResetShadow();
private:
	void ChangedShadow(ofAbstractParameter & e);
};
