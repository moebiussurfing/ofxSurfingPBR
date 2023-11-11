//#pragma once
//#include "ofMain.h"
//
//#include "SurfingLights.h"
//#include "ofxGui.h"
//#include "ofxSurfingHelpersLite.h"
//#include "ofxSurfingPBRConstants.h"
//
////
////  SurfingLights.hpp
////  practiceEtc
////
////  Originally Created by Hiromitsu Iwanaka on 2018/05/26.
////  Hardly modified and updated by moebiusSufing, 2021.
////
//
////#define SURFING_CALIBRATE // -> Used to calibrate Bg box/sphere sizes..
//
////-
//
//class SurfingLights {
//public:
//	SurfingLights();
//	~SurfingLights();
//
//	void setup();
//	void setupGui();
//	void setGuiPosition(glm::vec2 pos);
//	void drawGui();
//	ofxPanel gui;
//	ofxPanel guiUser;
//	ofParameterGroup params_User;
//
//private:
//	void startup();
//
//public:
//	ofParameter<bool> bGui;
//	ofParameter<bool> bGui_Parts;
//	//ofParameter<bool> bGui_Palette;
//
//private:
//	ofLight pointLight;
//	ofLight spotLight;
//	ofLight directionalLight;
//
//public:
//	void begin();
//	void end();
//
//	void drawScene(bool bInCam = true);
//	void draw();
//
//public:
//	int mouseX, mouseY;
//
//private:
//	void setupParameters();
//
//	//-
//
//	ofParameterGroup params;
//
//	void ChangedLights(ofAbstractParameter & e);
//	void ChangedBg(ofAbstractParameter & e);
//	void ChangedBrights(ofAbstractParameter & e);
//
//	ofParameterGroup params_Lights;
//
//	//TODO:
//	// Bright settings should load setting after light colors..
//
//	ofParameterGroup params_Brights;
//
//	ofParameterGroup params_pointLight;
//	ofParameter<bool> bPoint;
//	ofParameter<bool> bPointReset;
//	ofParameter<float> pointBright;
//	ofParameter<ofFloatColor> pointLightAmbientColor;
//	ofParameter<ofFloatColor> pointLightDiffuseColor;
//	ofParameter<ofFloatColor> pointLightSpecularColor;
//	ofParameter<glm::vec3> pointLightPosition;
//
//	ofParameterGroup params_spotLight;
//	ofParameter<bool> bSpot;
//	ofParameter<bool> bSpotReset;
//	ofParameter<float> spotBright;
//	ofParameter<ofFloatColor> spotLightAmbientColor;
//	ofParameter<ofFloatColor> spotLightDiffuseColor;
//	ofParameter<ofFloatColor> spotLightSpecularColor;
//	ofParameter<glm::vec3> spotLightOrientation;
//	ofParameter<glm::vec3> spotLightPosition;
//	ofParameter<int> spotLightCutOff;
//	ofParameter<int> spotLightConcetration;
//
//	ofParameterGroup params_directionalLight;
//	ofParameter<bool> bDirectional;
//	ofParameter<bool> bDirectionalReset;
//	ofParameter<float> directionalBright;
//	ofParameter<ofFloatColor> directionalLightAmbientColor;
//	ofParameter<ofFloatColor> directionalLightDiffuseColor;
//	ofParameter<ofFloatColor> directionalLightSpecularColor;
//	ofParameter<glm::vec3> directionalLightOrientation;
//	ofParameter<glm::vec3> directionalLightPosition;
//
//public:
//	ofParameter<bool> bMini_Scene;
//	ofParameter<bool> bGui_SubLights { "Lights", false };
//	ofParameter<bool> bGui_SubBg { "Background", false };
//
//	//-
//
//public:
//	bool bDebugLights = 0;
//	ofParameter<bool> bInCam { "InCam", false };
//	ofParameter<bool> bAnimLights { "Anim Lights", false };
//
//private:
//	ofParameter<bool> bRotate { "Bg Rotate", true };
//	ofParameter<float> speedRotate { "Bg Speed", 0.5, 0, 1 };
//	ofParameter<bool> bSmoothLights;
//
//	ofVec3f center;
//
//	//-
//
//	/*
//private:
//	// Bg Box/Sphere
//
//	ofMaterial Bg_material;
//	ofImage img;
//
//public:
//	ofParameter<bool> bAutoSetColor;
//	ofParameter<bool> Bg_AutoSetColorPick;
//
//public:
//	//--------------------------------------------------------------
//	void setBgColorPlain(ofColor c) {
//		Bg_color = c;
//	}
//
//private:
//	ofParameterGroup paramsScene;
//	ofParameter<float> sizeScene;
//	ofParameter<bool> bUseTex;
//	ofParameter<bool> bDrawBox;
//	ofParameter<bool> bDrawSphere;
//	ofParameter<bool> vReset;
//
//	ofParameterGroup paramsColors;
//	ofParameter<float> Bg_shininess;
//	ofParameter<ofFloatColor> Bg_color;
//	ofParameter<ofFloatColor> Bg_diffuse;
//	ofParameter<ofFloatColor> Bg_ambient;
//	ofParameter<ofFloatColor> Bg_specular;
//	ofParameter<ofFloatColor> Bg_emissive;
//	ofParameter<float> Bg_Bright;
//	*/
//
//	//----
//
//	/*
//	// A material will be used for each added material/3D object
//
//	//--------------------------------------------------------------
//	//class SurfingMaterial
//	struct SurfingMaterial {
//		ofMaterial material;
//
//		ofParameterGroup params_Material { "Material" };
//		ofParameter<bool> bEnable { "Enable", true };
//		ofParameter<float> shininess { "Shininess", 120, 0, 120 };
//
//		ofParameter<ofFloatColor> color { "Color", ofFloatColor(0.8f, 0.8f, 0.8f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< diffuse reflectance
//		ofParameter<ofFloatColor> diffuse { "Diffuse", ofFloatColor(0.8f, 0.8f, 0.8f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< diffuse reflectance
//		ofParameter<ofFloatColor> ambient { "Ambient", ofFloatColor(0.2f, 0.2f, 0.2f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< ambient reflectance
//		ofParameter<ofFloatColor> specular { "Specular", ofFloatColor(0.0f, 0.0f, 0.0f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< specular reflectance
//		ofParameter<ofFloatColor> emissive { "Emissive", ofFloatColor(0.0f, 0.0f, 0.0f, 1.0f), ofFloatColor(0, 0, 0, 0), ofFloatColor(1, 1, 1, 1) }; ///< emitted light intensity
//
//		//--------------------------------------------------------------
//		void setup() {
//			params_Material.clear();
//			params_Material.add(shininess);
//			params_Material.add(diffuse);
//			params_Material.add(ambient);
//			params_Material.add(specular);
//			params_Material.add(emissive);
//		}
//		//--------------------------------------------------------------
//		void setParamsFromSettings() {
//			diffuse = material.getDiffuseColor();
//			ambient = material.getAmbientColor();
//			specular = material.getSpecularColor();
//			emissive = material.getEmissiveColor();
//			shininess = material.getShininess();
//		}
//		//--------------------------------------------------------------
//		void setSettingsFromParams() {
//			material.setDiffuseColor(diffuse);
//			material.setAmbientColor(ambient);
//			material.setSpecularColor(specular);
//			material.setEmissiveColor(emissive);
//			material.setShininess(shininess);
//		}
//		//--------------------------------------------------------------
//		void setFromMaterial(ofMaterial mat) {
//
//			material = mat;
//			setParamsFromSettings();
//		}
//		//--------------------------------------------------------------
//		void setColor(ofFloatColor col) {
//			color = col;
//			ambient.set(color);
//			diffuse.set(color);
//
//			setSettingsFromParams();
//		}
//	};
//
//public:
//	vector<SurfingMaterial> materials;
//
//	// setup();
//
//public:
//	//--------------------------------------------------------------
//	void addMaterial(string name = "") {
//
//		ofMaterialSettings _settings;
//		ofMaterial material;
//		Bg_shininess = 120;
//		material.setup(_settings);
//		material.setShininess(Bg_shininess);
//		material.setSpecularColor(ofFloatColor(1, 1, 1, 1));
//		material.setAmbientColor(ofFloatColor(1, 1, 1, 1));
//
//		SurfingMaterial _mat;
//		_mat.setup();
//		_mat.setFromMaterial(material);
//
//		materials.emplace_back(_mat);
//
//		string _name;
//		//_name = "Enable " + name);
//		if (name == "")
//			_name = "Material" + ofToString(materials.size());
//		else
//			_name = name;
//
//		materials.back().bEnable.setName(name);
//		materials.back().params_Material.setName(_name);
//	}
//	*/
//
//	//-
//
//	// Draw()
//
//	/*
//	//--------------------------------------------------------------
//	void beginMaterial(int index) {
//		if (index < materials.size())
//			materials[index].material.begin();
//	}
//	//--------------------------------------------------------------
//	void endMaterial(int index) {
//		if (index < materials.size())
//			materials[index].material.end();
//	}
//	*/
//	
//	//--------------------------------------------------------------
//	void beginLights() {
//		if (bPoint) pointLight.enable();
//		if (bSpot) spotLight.enable();
//		if (bDirectional) directionalLight.enable();
//	}
//	//--------------------------------------------------------------
//	void endLights() {
//		if (!bPoint) pointLight.disable();
//		if (bSpot) spotLight.disable();
//		if (bDirectional) directionalLight.disable();
//	}
//
//	/*
//public:
//	// Materials colors
//	//--------------------------------------------------------------
//	void setColor(int index, ofFloatColor color) {
//		materials[index].setColor(color);
//	}
//	//--------------------------------------------------------------
//	void setColor(ofFloatColor color) {
//		for (int i = 0; i < materials.size(); i++) {
//			materials[i].setColor(color);
//		}
//	}
//
//	// Bg Color
//	//--------------------------------------------------------------
//	void setColorBgGroup(ofFloatColor color) {
//
//		Bg_color.set(color);
//		Bg_diffuse.set(color);
//		Bg_ambient.set(color);
//		Bg_specular.set(color);
//		Bg_emissive.set(color);
//
//		setBrightToColorGroup();
//	}
//
//	// Resets
//private:
//	void doResetAll();
//	void doResetAll();
//	void doResetLights();
//	void doResetPoint();
//	void doResetSpot();
//	void doResetDirectional();
//
//	std::string pathSettings_Lights = "SceneLights.json";
//	std::string pathSettings_Bg = "SceneBg.json";
//
//	//--
//
//	//--------------------------------------------------------------
//	void refreshBrights() {
//		refreshBrightPoint();
//		refreshBrightDirect();
//		refreshBrightSpot();
//	}
//
//	//--------------------------------------------------------------
//	void refreshBrightPoint() {
//		auto ca = pointLightAmbientColor.get();
//		auto cd = pointLightDiffuseColor.get();
//		auto cs = pointLightSpecularColor.get();
//		ca.setBrightness(pointBright);
//		cd.setBrightness(pointBright);
//		cs.setBrightness(pointBright);
//		pointLightAmbientColor.set(ca);
//		pointLightDiffuseColor.set(cd);
//		pointLightSpecularColor.set(cs);
//	}
//
//	//--------------------------------------------------------------
//	void refreshBrightDirect() {
//		auto ca = directionalLightAmbientColor.get();
//		auto cd = directionalLightDiffuseColor.get();
//		auto cs = directionalLightSpecularColor.get();
//		ca.setBrightness(directionalBright);
//		cd.setBrightness(directionalBright);
//		cs.setBrightness(directionalBright);
//		directionalLightAmbientColor.set(ca);
//		directionalLightDiffuseColor.set(cd);
//		directionalLightSpecularColor.set(cs);
//	}
//
//	//--------------------------------------------------------------
//	void refreshBrightSpot() {
//		auto ca = spotLightAmbientColor.get();
//		auto cd = spotLightDiffuseColor.get();
//		auto cs = spotLightSpecularColor.get();
//		ca.setBrightness(spotBright);
//		cd.setBrightness(spotBright);
//		cs.setBrightness(spotBright);
//		spotLightAmbientColor.set(ca);
//		spotLightDiffuseColor.set(cd);
//		spotLightSpecularColor.set(cs);
//	}
//
//	//--------------------------------------------------------------
//	void setBrightToColorGroup() {
//		auto cc = Bg_color.get();
//		auto cd = Bg_diffuse.get();
//		auto ca = Bg_ambient.get();
//		auto cs = Bg_specular.get();
//		auto ce = Bg_emissive.get();
//		cc.setBrightness(Bg_Bright);
//		ca.setBrightness(Bg_Bright);
//		cd.setBrightness(Bg_Bright);
//		cs.setBrightness(Bg_Bright * 0.75f);
//		ce.setBrightness(Bg_Bright * 0.75f);
//		Bg_color.set(cc);
//		Bg_diffuse.set(cd);
//		Bg_ambient.set(ca);
//		Bg_specular.set(cs);
//		Bg_emissive.set(ce);
//	}
//
//	*/
//};
