#pragma once
#include "ofMain.h"

//  SurfingLights.hpp
//  Originally Created by Hiromitsu Iwanaka on 2018/05/26.
//  Hardly modified and updated by moebiusSufing, 2021.

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
	ofParameter<float> lightX;
	ofParameter<float> lightY;
	ofParameter<float> lightZ;
	ofParameter<ofFloatColor> lightAmbientColor;
	void doResetLight();
	ofParameter<bool> bDebug;
	ofParameter<bool> bDebugShadow;
	callback_t f_RenderScene = nullptr;
	void setFunctionRenderScene(callback_t f = nullptr);
	ofParameter<void> vResetLight;
	ofParameterGroup lightParams;

public:
	void setupPBRlights();
	void updatePBRlights();
	void drawPBRlights();
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

public:
	int mouseX, mouseY;

	//-

	ofParameterGroup parameters;

	void Changed(ofAbstractParameter & e);
	void ChangedLights(ofAbstractParameter & e);
	void ChangedBrights(ofAbstractParameter & e);

	ofParameterGroup params_User;
	ofParameterGroup params_Lights;
	ofParameterGroup params_Brights;

	//--

	ofParameterGroup params_pointLight;
	ofParameter<bool> bPoint;
	ofParameter<bool> bPointReset;
	ofParameter<float> pointBright;
	ofParameter<ofFloatColor> pointLightAmbientColor;
	ofParameter<ofFloatColor> pointLightDiffuseColor;
	ofParameter<ofFloatColor> pointLightSpecularColor;
	ofParameter<glm::vec3> pointLightPosition;

	ofParameterGroup params_spotLight;
	ofParameter<bool> bSpot;
	ofParameter<bool> bSpotReset;
	ofParameter<float> spotBright;
	ofParameter<ofFloatColor> spotLightAmbientColor;
	ofParameter<ofFloatColor> spotLightDiffuseColor;
	ofParameter<ofFloatColor> spotLightSpecularColor;
	ofParameter<glm::vec3> spotLightOrientation;
	ofParameter<glm::vec3> spotLightPosition;
	ofParameter<int> spotLightCutOff;
	ofParameter<int> spotLightConcetration;

	ofParameterGroup params_directionalLight;
	ofParameter<bool> bDirectional;
	ofParameter<bool> bDirectionalReset;
	ofParameter<float> directionalBright;
	ofParameter<ofFloatColor> directionalLightAmbientColor;
	ofParameter<ofFloatColor> directionalLightDiffuseColor;
	ofParameter<ofFloatColor> directionalLightSpecularColor;
	ofParameter<glm::vec3> directionalLightOrientation;
	ofParameter<glm::vec3> directionalLightPosition;


public:
	bool bDebugLights = 0;
	ofParameter<bool> bAnimLights;

private:
	ofParameter<bool> bSmoothLights;

	ofVec3f center;

public:
	//--------------------------------------------------------------
	void beginLights() {
		if (bPoint) pointLight.enable();
		if (bSpot) spotLight.enable();
		if (bDirectional) directionalLight.enable();
	}
	//--------------------------------------------------------------
	void endLights() {
		if (bPoint) pointLight.disable();
		if (bSpot) spotLight.disable();
		if (bDirectional) directionalLight.disable();
	}

	/*
public:
	// Materials colors
	//--------------------------------------------------------------
	void setColor(int index, ofFloatColor color) {
		materials[index].setColor(color);
	}
	//--------------------------------------------------------------
	void setColor(ofFloatColor color) {
		for (int i = 0; i < materials.size(); i++) {
			materials[i].setColor(color);
		}
	}
	*/

	// Resets
public:
	void doResetAll();
private:
	void doResetLights();
	void doResetPoint();
	void doResetSpot();
	void doResetDirectional();

	string pathSettings = "ofxSurfingPBR_Lights.json";

public:
	void load();
	void save();

	//--

private:
	void refreshBrights();
	void refreshBrightPoint();
	void refreshBrightDirect();
	void refreshBrightSpot();

	//----

	/*
	* 
private:

	/*
	// A material will be used for each added material/3D object

	//--------------------------------------------------------------
	//class SurfingMaterial
	struct SurfingMaterial {
		ofMaterial material;

		ofParameterGroup params_Material { "Material" };
		ofParameter<bool> bEnable { "Enable", true };
		ofParameter<float> shininess { "Shininess", 120, 0, 120 };

		ofParameter<ofFloatColor> color { "Color", ofFloatColor(0.8f, 0.8f, 0.8f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< diffuse reflectance
		ofParameter<ofFloatColor> diffuse { "Diffuse", ofFloatColor(0.8f, 0.8f, 0.8f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< diffuse reflectance
		ofParameter<ofFloatColor> ambient { "Ambient", ofFloatColor(0.2f, 0.2f, 0.2f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< ambient reflectance
		ofParameter<ofFloatColor> specular { "Specular", ofFloatColor(0.0f, 0.0f, 0.0f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< specular reflectance
		ofParameter<ofFloatColor> emissive { "Emissive", ofFloatColor(0.0f, 0.0f, 0.0f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< emitted light intensity

		//--------------------------------------------------------------
		void setup() {
			params_Material.clear();
			params_Material.add(shininess);
			params_Material.add(diffuse);
			params_Material.add(ambient);
			params_Material.add(specular);
			params_Material.add(emissive);
		}
		//--------------------------------------------------------------
		void setParamsFromSettings() {
			diffuse = material.getDiffuseColor();
			ambient = material.getAmbientColor();
			specular = material.getSpecularColor();
			emissive = material.getEmissiveColor();
			shininess = material.getShininess();
		}
		//--------------------------------------------------------------
		void setSettingsFromParams() {
			material.setDiffuseColor(diffuse);
			material.setAmbientColor(ambient);
			material.setSpecularColor(specular);
			material.setEmissiveColor(emissive);
			material.setShininess(shininess);
		}
		//--------------------------------------------------------------
		void setFromMaterial(ofMaterial mat) {

			material = mat;
			setParamsFromSettings();
		}
		//--------------------------------------------------------------
		void setColor(ofFloatColor col) {
			color = col;
			ambient.set(color);
			diffuse.set(color);

			setSettingsFromParams();
		}
	};

public:
	vector<SurfingMaterial> materials;

	// setup();

public:
	//--------------------------------------------------------------
	void addMaterial(string name = "") {

		ofMaterialSettings _settings;
		ofMaterial material;
		Bg_shininess = 120;
		material.setup(_settings);
		material.setShininess(Bg_shininess);
		material.setSpecularColor(ofFloatColor(1, 1, 1, 1));
		material.setAmbientColor(ofFloatColor(1, 1, 1, 1));

		SurfingMaterial _mat;
		_mat.setup();
		_mat.setFromMaterial(material);

		materials.emplace_back(_mat);

		string _name;
		//_name = "Enable " + name);
		if (name == "")
			_name = "Material" + ofToString(materials.size());
		else
			_name = name;

		materials.back().bEnable.setName(name);
		materials.back().params_Material.setName(_name);
	}
	*/

	/*
	//--------------------------------------------------------------
	void beginMaterial(int index) {
		if (index < materials.size())
			materials[index].material.begin();
	}
	//--------------------------------------------------------------
	void endMaterial(int index) {
		if (index < materials.size())
			materials[index].material.end();
	}
	*/
};
