/*
* SurfingLights.h
* 
* This class is a predefined set of the OF core 
* 4 different light types:
* Point, Directional, Spot and Area.
* Ready to be inserted on an environment scene.
* With the related parameters and a ui based on ofxGui.
* Bundled testing animations, mouse positioning and debug helpers,
* for positioning or shadows.
* All settings are persistent.
* https://openframeworks.cc///documentation/gl/ofLight/
*/

/*
	TODO

	add look at draggable/moveable points
	fix not saving power settings?
		refresh power after changing colors!
		bc if not, do not reflects the bright/power of the light!
	fix appeared visible rectangle when drawing area light?
	create a cool init state.
	make simple mode/user/game controls.
	for light positions: add height, long/lat, distance controls instead of xyz
*/

//--

#pragma once
#include "ofMain.h"

//  Some code parts are originally inspired by Hiromitsu Iwanaka.
//  Area light code is taken from: openFrameworks\examples\gl\areaLightExample"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"
#include "ofxSurfingPBRConstants.h"
#include "SurfingHelperPosition.h"

//----

class SurfingLights {
public:
	SurfingLights();
	~SurfingLights();

public:
	void setup();
	void exit();
	void keyPressed(int key);

private:
	bool bDoneExit = false;

private:
	void setupParameters();
	void setupParametersLights();
	void setupParametersShadows();
	void setupCallbacks();
	void startup();
	void startupDelayed();

	//--

private:
	//TODO fix crash callbacks
	// To fix some settings
	// that are not updated/refreshed correctly
	// and/or app startup crashes!
	// App flow controls
	//bool bDisableCallbacks = false;
	bool bDoneSetup = false;
	bool bDoneStartup = false;
	bool bDoneStartupDelayed = false;
	bool bAppRunning = false;

private:
	void update();
	void update(ofEventArgs & args);

public:
	void setupLights();
	void updateLights();
	void drawDebugLights();

private:
	void computeLights();
	void computeShadows();

	//--

public:
	void begin();
	void end();

	//--
public:
	shared_ptr<ofLight> getLight(size_t index);

private:
	vector<shared_ptr<ofLight>> lights;

	callback_t f_RenderScene = nullptr;

public:
	void setFunctionRenderScene(callback_t f = nullptr);

	ofParameter<bool> bDebug;
	ofParameter<bool> bDebugShadow;
	ofParameter<bool> bAutoLayout;

	ofParameter<void> vResetAllLights;

private:
	bool bFlagDoResetAllLights = false;

private:
	bool bFlagRefreshGui = false;

	//--

private:
	void setupGui();
	void refreshGui(bool bHard = false); //to reset anchor too

public:
	ofRectangle getGuiShape() const;

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
	ofParameterGroup lightsParams;
	ofParameterGroup lightsSettingsParams;

	//--

	void ChangedLights(ofAbstractParameter & e);

	//--

	ofParameterGroup globalColorsParams;

	ofParameter<ofFloatColor> globalColor; //main global
	ofFloatColor globalColor_;
	ofParameter<int> indexFromColorToGlobal;
	ofParameter<string> nameSourceGlobal;
	bool bFlagIndexFromColorToGlobal = false;
	void doIndexFromColorToGlobal();
	void doGetGlobalColorFromIndexColor();
	bool bAttendingColors = false;
	bool bFlagFromColorIndexToGlobals = false;

	// Point
	ofParameterGroup pointParams;
	ofParameterGroup pointColorsParams;
	ofParameter<bool> bPoint;
	ofParameter<bool> bPointShadow;
	ofParameter<void> vPointReset;
	ofParameter<ofFloatColor> pointAmbientColor;
	ofParameter<ofFloatColor> pointDiffuseColor;
	ofParameter<ofFloatColor> pointSpecularColor;

	PointCartesian pointPosition;

	ofParameter<ofFloatColor> pointGlobalColor;
	ofParameter<float> pointSizeFar;

	//--

	// Directional
	ofParameterGroup directionalParams;
	ofParameterGroup directionalColorsParams;
	ofParameter<bool> bDirectional;
	ofParameter<bool> bDirectionalShadow;
	ofParameter<void> vDirectionalReset;
	ofParameter<ofFloatColor> directionalAmbientColor;
	ofParameter<ofFloatColor> directionalDiffuseColor;
	ofParameter<ofFloatColor> directionalSpecularColor;
	ofParameter<ofFloatColor> directionalGlobalColor;
	ofParameter<glm::vec3> directionalOrientation;
	PointCartesian directionalPosition;
	ofParameter<float> directionalSizeFar;
	ofParameter<float> directionalSizeNear;

	//--

	// Spot
	ofParameterGroup spotParams;
	ofParameterGroup spotColorsParams;
	ofParameter<bool> bSpot;
	ofParameter<bool> bSpotShadow;
	ofParameter<void> vSpotReset;
	ofParameter<ofFloatColor> spotAmbientColor;
	ofParameter<ofFloatColor> spotDiffuseColor;
	ofParameter<ofFloatColor> spotSpecularColor;
	ofParameter<ofFloatColor> spotGlobalColor;
	ofParameter<glm::vec3> spotOrientation;
	PointCartesian spotPosition;
	ofParameter<int> spotCutOff;
	ofParameter<int> spotConcentration;
	ofParameter<float> spotSizeFar;

	//--

	// Area
	ofParameterGroup areaParams;
	ofParameterGroup areaColorsParams;
	ofParameter<bool> bArea;
	ofParameter<bool> bAreaShadow;
	ofParameter<void> vAreaReset;
	ofParameter<ofFloatColor> areaAmbientColor;
	ofParameter<ofFloatColor> areaDiffuseColor;
	ofParameter<ofFloatColor> areaSpecularColor;
	PointCartesian areaPosition;
	ofParameter<glm::vec3> areaOrientation;
	ofParameter<ofFloatColor> areaGlobalColor;
	ofParameter<int> modeAnimArea;
	ofParameter<glm::vec2> areaSize;
	ofParameter<float> areaSizeFar;

private:
	float orbitSpeed = 0.0;
	float highwaySpeed = 0.;
	float bump = 0.;
	float bumpHeight = 50.0;

	//--

public:
	void updateAnims();
	void updateAnimsArea();
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
	void doResetAllLights(bool bHard = false);

private:
	void doResetPoint(bool bHard = false);
	void doResetSpot(bool bHard = false);
	void doResetDirectional(bool bHard = false);
	void doResetArea(bool bHard = false);

	bool bFlagDoResetPoint = false;
	bool bFlagDoResetSpot = false;
	bool bFlagDoResetDirectional = false;
	bool bFlagDoResetArea = false;

	//--

	// settings
private:
	string pathSettings = "ofxSurfingPBR_Lights.json";
	string pathSettingsShadows = "ofxSurfingPBR_Shadows.json";

	bool bFlagLoad = false;

public:
	bool load();
	void save();

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
private:
	SurfingAutoSaver autoSaver;
#endif

	//--

	// Powers
private:
	bool bFlagDoRefreshPowers = false;
	bool bFlagDoRefreshPowerPoint = false;
	bool bFlagDoRefreshPowerDirect = false;
	bool bFlagDoRefreshPowerSpot = false;
	bool bFlagDoRefreshPowerArea = false;

	void doRefreshPowers();
	void doRefreshPowerPoint();
	void doRefreshPowerDirect();
	void doRefreshPowerSpot();
	void doRefreshPowerArea();

	void ChangedPowers(ofAbstractParameter & e);
	ofParameterGroup powersParams;
	ofParameter<float> pointPower;
	ofParameter<float> spotPower;
	ofParameter<float> directionalPower;
	ofParameter<float> areaPower;

	//----

	// shadow
private:
	ofParameterGroup shadowParams;

public:
	ofParameter<bool> bDrawShadow;

private:
	ofParameter<float> shadowBias;
	ofParameter<float> shadowNormalBias;
	ofParameter<float> shadowSampleRadius;
	ofParameter<int> indexShadowType;
	ofParameter<string> nameShadowType;
	ofParameter<float> shadowStrength;
	ofParameter<int> shadowResolution;

	ofParameter<void> vResetShadow;
	bool bFlagDoResetShadow = false;

public:
	ofxPanel guiShadows;
	ofParameter<bool> bGui_Shadows;

public:
	void doResetShadow();

private:
	void ChangedShadow(ofAbstractParameter & e);
};