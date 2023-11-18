/*

	TODO

	add bang to update/link global color from other colors.
	fix coat for standalone material

*/


#pragma once
#include "ofMain.h"

#include "ofxSurfingPBRConstants.h"
#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"

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

	void Changed(ofAbstractParameter & e);
	void ChangedHelpers(ofAbstractParameter & e);
	void ChangedGlobals(ofAbstractParameter & e);

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

	ofParameter<ofFloatColor> globalColor; //will set all colors except touching their alphas.
	ofParameter<float> globalAlpha; //will set this alpha to all colors.
private:
	ofParameter<void> vToGlobal;
	ofParameter<int> fromColorToGlobal;
	ofParameter<string> nameSourceGlobal;
	bool bAttendingGlobal = false;
	void doToGlobal();

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
	ofParameter<void> vRandomMaterial;
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
	string ext= ".json";
	void setName(const string &n);//call before setup

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

	void setupHistoryManager();
	void refreshHistoryFolder();
	void reorganizeHistoryFolder();
	void doClearHistory();
	void doRecallState(int i);
	void doRemoveState(int i);
	void removeHistoryFolder();
	void refreshGlobals();

	const string getFilePathHistoryState(const int &i);
	string pathHistory = "ofxSurfingPBR_Material_Temp\\";
	string pathHistoryNameRoot = "ofxSurfing_Material_";

public:
	void doPrevHistory();
	void doNextHistory();
	void doStoreNewState();
	void doRecallState();
	void doSaveState(int i=-1);

public:
	//TODO: startup fix overwrite settings from file..
	// Helpers to pair with OF core ofMaterial
	// Useful to replace code from your projects or OF bunbled examples.
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
