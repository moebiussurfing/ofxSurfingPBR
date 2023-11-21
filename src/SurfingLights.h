/*
* SurfingLights.h
* 
* This class is a predefined setup of 3 different type lights:
* point, directional and spot.
* Ready to be inserted on an environment scene.
* 
*/



/*
	TODO
	add global from/to color workflow (copy from material).
	create a cool init state.
	make simple mode/user/ game controls.
	fix mouse and time anims.
	add area light. get from OF example.
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

public:
	void setup();
	void exit();

private:
	void setupParameters();
	void startup();

private:
	void update();
	void update(ofEventArgs & args);

public:
	void setupLights();
	void updateLights();
	void drawLights();
	void drawDebugLights();

	//--

public:
	void begin();
	void end();

	//--

private:
	vector<shared_ptr<ofLight>> lights;

	callback_t f_RenderScene = nullptr;

public:
	void setFunctionRenderScene(callback_t f = nullptr);

	ofParameter<bool> bDebug;
	ofParameter<bool> bDebugShadow;

	ofParameter<void> vResetLights;

private:
	bool bFlagDoResetLights= false;

private:
	bool bFlagRefreshGui = false;

	//--

private:
	void setupGui();
	void refreshGui();

public:
	ofxPanel gui;
	void setGuiPosition(glm::vec2 pos);
	void drawGui();
	ofParameter<bool> bGui;

	//-

	ofParameterGroup parameters;

	ofParameterGroup params_User;
	ofParameterGroup params_Enablers;
	ofParameterGroup params_Extra;
	ofParameterGroup params_TestAnims;
	ofParameterGroup params_Lights;

	//--

	void ChangedLights(ofAbstractParameter & e);

	//void ChangedPoint(ofAbstractParameter & e);
	//void ChangedSpot(ofAbstractParameter & e);
	//void ChangedDirectional(ofAbstractParameter & e);

	//--

	// Point
	ofParameterGroup pointParams;
	ofParameterGroup pointColorsParams;
	ofParameter<bool> bPoint;
	ofParameter<void> vPointReset;
	ofParameter<ofFloatColor> pointAmbientColor;
	ofParameter<ofFloatColor> pointDiffuseColor;
	ofParameter<ofFloatColor> pointSpecularColor;
	ofParameter<glm::vec3> pointPosition;
	ofParameter<ofFloatColor> pointGlobalColor;

	//--

	// Spot
	ofParameterGroup spotParams;
	ofParameterGroup spotColorsParams;
	ofParameter<bool> bSpot;
	ofParameter<void> vSpotReset;
	ofParameter<ofFloatColor> spotAmbientColor;
	ofParameter<ofFloatColor> spotDiffuseColor;
	ofParameter<ofFloatColor> spotSpecularColor;
	ofParameter<ofFloatColor> spotGlobalColor;
	ofParameter<glm::vec3> spotOrientation;
	ofParameter<glm::vec3> spotPosition;
	ofParameter<int> spotCutOff;
	ofParameter<int> spotConcentration;

	//--

	// Directional
	ofParameterGroup directionalParams;
	ofParameterGroup directionalColorsParams;
	ofParameter<bool> bDirectional;
	ofParameter<void> vDirectionalReset;
	ofParameter<ofFloatColor> directionalAmbientColor;
	ofParameter<ofFloatColor> directionalDiffuseColor;
	ofParameter<ofFloatColor> directionalSpecularColor;
	ofParameter<ofFloatColor> directionalGlobalColor;
	ofParameter<glm::vec3> directionalOrientation;
	ofParameter<glm::vec3> directionalPosition;

	//--

public:
	void updateAnims();
	void restoreAnims();
	ofParameter<bool> bAnimLights;
	ofParameter<bool> bAnimLightsMouse;
	int mouseX, mouseY;

	ofParameter<bool> bDebugLights;

private:
	ofParameter<bool> bSmoothLights;

	ofVec3f center;

	//--

	// Resets
public:
	void doResetLights(bool bColorsToo = false);

private:
	void doResetPoint(bool bColorsToo=false);
	void doResetSpot(bool bColorsToo = false);
	void doResetDirectional(bool bColorsToo = false);

	bool bFlagDoResetPoint = false;
	bool bFlagDoResetSpot = false;
	bool bFlagDoResetDirectional = false;

	//--

	// settings
private:
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

	// brights
private:
	bool bFlagDoRefreshBrights = false;
	bool bFlagDoRefreshBrightPoint = false;
	bool bFlagDoRefreshBrightDirect = false;
	bool bFlagDoRefreshBrightSpot = false;

	void doRefreshBrights();
	void doRefreshBrightPoint();
	void doRefreshBrightDirect();
	void doRefreshBrightSpot();

	void ChangedBrights(ofAbstractParameter & e);
	ofParameterGroup params_Brights;
	ofParameter<float> pointBright;
	ofParameter<float> spotBright;
	ofParameter<float> directionalBright;

	//----

	// shadow
private:
	ofParameterGroup shadowParams;

public:
	ofParameter<bool> bDrawShadow;

private:
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	ofParameter<void> vResetShadow;
	bool bFlagDoResetShadow = false;

	//TODO: OF PBR
	// get other shadow params from OF example
	// add other internal OF params
	//ofParameter<float> shadowStrength;
	//ofParameter<glm::vec2> shadowSize;

public:
	ofxPanel guiShadows;
	ofParameter<bool> bGui_Shadows;

public:
	void doResetShadow();

private:
	void ChangedShadow(ofAbstractParameter & e);
};
