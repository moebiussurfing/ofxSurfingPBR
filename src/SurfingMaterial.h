/*

	TODO

*/

#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"
#include "ofxSurfingPBRConstants.h"

class SurfingMaterial {
public:
	SurfingMaterial();
	~SurfingMaterial();

	void setup();
	void setup(string name);
	void drawGui();
	void exit();

private:
	void update();
	void update(ofEventArgs & args);

	void setupParams();
	void setupGui();
	void refreshGui();

	void ChangedSettings(ofAbstractParameter & e);
	void ChangedColors(ofAbstractParameter & e);
	void ChangedHelpers(ofAbstractParameter & e);
	void ChangedGlobals(ofAbstractParameter & e);

	// App flow controls
	//bool bDisableCallbacks = false;
	bool bDoneSetup = false;
	//bool bDoneSetupParams = false;
	//bool bDoneStartup = false;
	//bool bDoneDelayed = false;
	bool bAppRunning = false;

private:
	ofMaterial material;

public:
	void begin();
	void end();

public:
	ofParameterGroup parameters;

	ofParameterGroup colorParams;
	ofParameterGroup globalParams;
	ofParameterGroup globalLinksParams;
	ofParameterGroup settingsParams;
	ofParameterGroup coatParams;
	ofParameterGroup randomizersParams;
	ofParameterGroup helpersParams;

	ofParameter<ofFloatColor> globalColor; // will set all colors except touching their alphas.
	ofParameter<float> globalAlpha; // will set this alpha to all colors.

private:
	//bool bAttendingGlobal = false;
	ofParameter<void> vFromColorToGlobal;
	ofParameter<int> indexFromColorToGlobal;
	ofParameter<string> nameSourceGlobal;

	bool bFlagGlobalColor = false;
	void doGlobalColor();

	bool bFlagGlobalAlpha = false;
	void doGlobalAlpha();

	bool bFlagIndexFromColorToGlobal = false;
	void doIndexFromColorToGlobal();

	bool bFlagFromColorIndexToGlobals = false;
	void doFromColorIndexToGlobals();

public:
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

	ofParameter<void> vResetMaterial;
	ofParameter<void> vRandomMaterialFull;
	ofParameter<void> vRandomColors;
	ofParameter<void> vRandomAlphas;
	ofParameter<void> vRandomColorsAlpha;
	ofParameter<void> vRandomColorsGlobal;
	ofParameter<void> vRandomSettings;

	ofxPanel gui;
	void setGuiPosition(glm::vec2 pos);

	ofxPanel guiHelpers;
	ofParameter<bool> bGui;
	ofParameter<bool> bGuiHelpers;

	ofParameter<int> & getIndexStateParam();

public:
	string pathRoot = "ofxSurfingPBR_Material";
	string path = "";
	string name = "";
	string ext = ".json";
	void setName(const string & n); //call before setup

public:
	void load();
	void save();

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	SurfingAutoSaver autoSaver;
#endif

public:
	void doResetMaterial();
	void doRandomMaterial();
	void doRandomColors();
	void doRandomAlphas();
	void doRandomColorsAlpha();
	void doRandomColorGlobal();
	void doRandomSettings();

private:
	bool bFlagDoResetMaterial = false;
	bool bFlagDoRandomMaterialFull = false;
	bool bFlagDoRandomColors = false;
	bool bFlagDoRandomAlphas = false;
	bool bFlagDoRandomColorGlobal = false;
	bool bFlagDoRandomColorsAlpha = false;
	bool bFlagDoRandomSettings = false;

	bool bAttendingColors= false;

private:
	// History browser
	size_t sizeHistory;
	ofParameterGroup historyParams;
	ofParameter<void> vClearHistory;
	ofParameter<void> vRefeshHistory;
	ofParameter<void> vStoreNewState;
	ofParameter<void> vSaveState;

	//workflow
	bool bAutoSaveBeforeChangeIndex = true;
	ofParameter<bool> bAutoStoreAfterRandoms;

	ofParameter<void> vPrevHistory;
	ofParameter<void> vNextHistory;
	ofParameter<void> vRecallState;
	ofParameter<void> vRemoveState;
	ofParameter<int> indexHistory;
	int indexHistory_ = -1;
	void doRefreshIndexHistory();

private:
	bool bFlagDoRefreshIndexHistory = false;
	bool bFlagDoPrevHistory = false;
	bool bFlagDoNextHistory = false;
	bool bFlagDoSaveState = false;
	bool bFlagDoStoreNewState = false;
	bool bFlagDoRecallState = false;
	bool bFlagDoRemoveState = false;
	bool bFlagDoRefeshHistory = false;
	bool bFlagDoClearHistory = false;

private:
	void setupHistoryManager();
	void refreshHistoryFolder();
	void reorganizeHistoryFolder();
	void doClearHistory();
	void doRecallState(int i);
	void doRemoveState(int i);
	void removeHistoryFolder();
	void refreshGlobals();

	const string getFilePathHistoryState(const int & i);
	string pathHistory = "ofxSurfingPBR_Material_Temp\\";
	string pathHistoryNameRoot = "ofxSurfing_Material_";

public:
	void doPrevHistory();
	void doNextHistory();
	void doRecallState();
	void doSaveState(int i = -1);
	void doStoreNewState();

public:
	//TODO: startup fix overwrite settings from file..

	// Helpers to pair with OF core ofMaterial
	// Useful to replace code from your projects or OF CORE bundled examples.
	void setAmbientColor(ofFloatColor c) {
		ambientColor.set(c);
	}
	void setDiffuseColor(ofFloatColor c) {
		diffuseColor.set(c);
	}
	void setSpecularColor(ofFloatColor c) {
		specularColor.set(c);
	}
	void setEmissiveColor(ofFloatColor c) {
		emissiveColor.set(c);
	}
	void setShininess(float shininess) {
		this->shininess.set(shininess);
	}
};
