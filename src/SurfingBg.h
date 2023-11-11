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

	//flow
	bool bDoneStartup = false;
	bool bAppRunning = false;

public:
	void exit();

private:
	void drawScene();
	void drawObject(float r);

public:
	void draw();

public:
	void drawGui();
	void setGuiPosition(glm::vec2 pos);

private:
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

private:
	ofParameter<bool> bRotate;
	ofParameter<float> speedRotate;
	ofParameter<bool> bSmoothLights;
	glm::vec3 center;

public:
	ofParameterGroup parameters;

private:
	void ChangedScene(ofAbstractParameter & e);
	void ChangedColors(ofAbstractParameter & e);

	//--

private:
	// Bg Box/Sphere
	ofMaterial material;
	ofImage img;

	//--

	// Bg color plain
public:
	ofParameterGroup backgroundColorPlainParams;
	ofParameter<bool> bDrawBgColorPlain;
	ofParameter<ofFloatColor> bgColorPlain;

public:
	void setBgColorPlain(ofFloatColor c);

	//--

public:
	ofParameterGroup paramsScene;
	ofParameter<float> sizeScene;
	ofParameter<bool> bDrawTex;

	ofParameterGroup paramsObject;
	ofParameterGroup paramsExtra;

	ofParameter<bool> bDrawObject;
	ofParameter<bool> bDrawWireframe;
	ofParameter<bool> bDrawBox;
	ofParameter<bool> bDrawSphere;

	ofParameter<void> vResetAll;
	ofParameter<void> vResetScene;
	ofParameter<void> vResetColors;

	ofParameterGroup paramsColors;
	ofParameterGroup paramsColorsGlobal;

	ofParameter<ofFloatColor> globalColor; 

	ofParameter<ofFloatColor> colorGroup;
	ofFloatColor colorGroup_;

	ofParameter<ofFloatColor> diffuse;
	ofParameter<ofFloatColor> ambient;
	ofParameter<ofFloatColor> specular;
	ofParameter<ofFloatColor> emissive;

	ofParameter<float> shininess;
	
	ofParameter<float> brightGlobal;
private:
	float brightGlobal_=-1;

public:
	void setColorBgGroup(ofFloatColor color);
	void setBrightToColorGroup(float brg);
	
private:
	bool bFlagSetColorBgGroup = false;
	bool bFlagSetBrightToColorGroup = false;

//private:
//	bool bAttendingRefreshColor = false;

public:
	void doResetAll();
	void doResetScene();
	void doResetColors();

	//public:
	//ofParameter<bool> bAutoSetColor;
	//ofParameter<bool> Bg_AutoSetColorPick;
	//ofParameter<bool> bInCam;
};
