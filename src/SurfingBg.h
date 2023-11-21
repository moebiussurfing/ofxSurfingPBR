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

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE

private:
	SurfingAutoSaver autoSaver;
#endif

private:
	void setupGui();
	void setupParameters();

public:
	void exit();

	//app flow
	bool bDoneStartup = false;
	bool bAppRunning = false;

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
	void startup();

public:
	ofParameter<bool> bGui;

	ofParameter<bool> bRotate;
	ofParameter<float> speedRotate;
	ofParameter<bool> bSmoothLights;

	ofParameterGroup parameters;

private:
	glm::vec3 center;

private:
	void Changed(ofAbstractParameter & e);
	void ChangedScene(ofAbstractParameter & e);
	void ChangedColors(ofAbstractParameter & e);

	//--

private:
	// Bg Box/Sphere
	ofMaterial material;

	//--

	// Bg color plain
public:
	ofParameterGroup bgColorPlainParams;
	ofParameter<bool> bDrawBgColorPlain;
	ofParameter<ofFloatColor> bgColorPlain;

public:
	void setBgColorPlain(ofFloatColor c);

	//--

public:
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
	ofParameterGroup paramsObject;
	ofParameterGroup paramsExtra;

	ofParameter<bool> bDrawObject;
	ofParameter<bool> bDrawWireframe;
	ofParameter<bool> bModeBox;
	ofParameter<bool> bModeSphere;
	ofParameter<float> resolutionSphere;
	ofParameter<float> resolutionBox;
	float resolutionSphere_=1;
	float resolutionBox_=1;

	ofParameter<void> vResetAll;
	ofEventListener listenerResetAll;

	ofParameter<void> vResetScene;
	ofParameter<void> vResetColors;

	ofParameterGroup paramsColor;
	ofParameterGroup paramsColorizers;
	ofParameterGroup paramsColorsGlobal;

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

	ofParameter<float> brightGlobal;

private:
	float brightGlobal_ = -1;

public:
	void setColorBgGroup(ofFloatColor color);
	void setBrightToColorGroup(float brg);

private:
	bool bFlagSetColorBgGroup = false;
	bool bFlagSetBrightToColorGroup = false;

public:
	void doResetAll();
	void doResetScene();
	void doResetColors();

	//public:
	//ofParameter<bool> bAutoSetColor;
	//ofParameter<bool> Bg_AutoSetColorPick;
	//ofParameter<bool> bInCam;

	//private:
	//	bool bAttendingRefreshColor = false;
};
