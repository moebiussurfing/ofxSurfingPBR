#pragma once
#include "ofMain.h"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"
#include "ofxSurfingPBRConstants.h"

//--

class SurfingBg {
public:
	SurfingBg();
	~SurfingBg();

public:
	void setup();

private:
	void update(ofEventArgs & args);

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE

private:
	SurfingAutoSaver autoSaver;
#endif

private:
	void setupGui();
	void setupParameters();
	void setupCallbacks();

public:
	void exit();

	//app flow
	bool bDoneStartup = false;
	bool bAppRunning = false;
	//bool bDisableCallbacks = false;

public:
	void draw();

private:
	void drawScene();
	void drawObject(float r);

public:
	void drawGui();
	void setGuiPosition(glm::vec2 pos);
	void refreshGui();

public:
	bool load();
	void save();

private:
	string pathSettings = "ofxSurfingPBR_Bg.json";

public:
	ofxPanel gui;
private:
	ofEventListener eSaveOfxGui;
	ofEventListener eLoadOfxGui;

private:
	void startup();

public:
	ofParameter<bool> bGui;

	ofParameter<bool> bRotate;
	ofParameter<float> speedRotate;
	//ofParameter<bool> bSmoothLights;

	ofParameterGroup parameters;

private:
	glm::vec3 center;

private:
	void Changed(ofAbstractParameter & e);
	void ChangedScene(ofAbstractParameter & e);
	void ChangedBgColor(ofAbstractParameter & e);
	void ChangedBgColorObject(ofAbstractParameter & e);

	//--

private:
	// Bg Box/Sphere
	ofMaterial material;

	//--

	// Bg color plain
public:
	ofParameterGroup paramsColorPlain;
	ofParameter<bool> bDrawBgColorPlain;
	ofParameter<ofFloatColor> bgColorPlain;

public:
	void setBgColorPlain(ofFloatColor c);
	void setColorGlobal(ofFloatColor color);

	//--

public:
	ofParameterGroup paramsRotate;
	ofParameterGroup paramsScene;
	ofParameter<float> sizeScene;

	ofParameter<bool> bUseTexture;
	ofParameter<void> vOpenTexture;
	ofParameter<string> pathTexture;

private:
	ofImage img;
	void processOpenFileSelection(ofFileDialogResult openFileResult);
	void loadTexture(string path);

public:
	ofParameterGroup paramsColorObject;
	//ofParameterGroup paramsExtra;

	ofParameter<bool> bDrawBgColorObject;
	ofParameter<bool> bDrawWireframe;
	ofParameter<ofFloatColor> wireColor;
	ofParameter<bool> bModeBox;
	ofParameter<bool> bModeSphere;
	ofParameter<float> resolutionSphere;
	ofParameter<float> resolutionBox;
	float resolutionSphere_=1;
	float resolutionBox_=1;

	ofParameter<void> vResetAll;
	ofParameter<void> vResetScene;
	ofParameter<void> vResetColors;

	ofEventListener eResetAll;
	ofEventListener eResetScene;
	ofEventListener eResetColors;

	ofParameterGroup paramsColorMaterial;
	ofParameterGroup paramsBgColor;
	//ofParameterGroup paramsColorsGlobal;

	ofParameter<ofFloatColor> globalColor;

private:
	ofParameter<ofFloatColor> colorGroup;
	ofFloatColor colorGroup_;

public:
	ofParameter<ofFloatColor> diffuse;
	ofParameter<ofFloatColor> ambient;
	ofParameter<ofFloatColor> specular;
	ofParameter<ofFloatColor> emissive;

	ofParameter<float> shininess;

private:
	float brightGlobal_ = -1;

public:
	void setColorBgGroup(ofFloatColor color);

private:
	bool bFlagSetColorBgGroup = false;

public:
	void doResetAll();
	void doResetScene();
	void doResetColors();
};
