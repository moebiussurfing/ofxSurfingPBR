
/*
* SurfingTransformNode.h
* 
	A class derived from ofNode, 
	but adding ofParameters for 
	position, rotation, scale and other useful params (draw, debug...etc)
	Ready to be exposed to a UI.
	Handles the transforms for an object.
	To be applied easily and to store to JSON settings
*/

/*
	TODO
	- add refreshGui by passing transfromGroup. like presets groups
	- allow mode for linked or independent scale for xyz
		- fix lock all axis to x 
	- add bbox for selected model
	- reduce some feedback callbacks
	- add simple example?
*/

//--

#pragma once

#include "ofMain.h"

//TODO: add settings
#include "ofxSurfingHelpersLite.h"

//----

//OPTIONAL: comment the below line
// if you are using this class standalone,
// without the whole ofxSurfingPBR!
//#include "ofxSurfingPBRConstants.h"

#ifndef SURFING__PBR__SCENE_SIZE_UNIT
	#define SURFING__PBR__SCENE_SIZE_UNIT 1000.f
#endif

// Range for rotate
//#define SURFING__MAX_DEGREE 360
#define SURFING__MAX_DEGREE 180

//#define SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS

//----

class TransformNode : public ofNode {

	//--

	// Some constants
	const float scaleNormalizedRatio = 10.f;
	const int scaleNormalizedPowMax = 100;
	//const float scaleNormalizedUnit = scaleNormalizedPowMax;
	float scaleNormalizedUnit = scaleNormalizedRatio * scaleNormalizedPowMax;
	const float unitSize = SURFING__PBR__SCENE_SIZE_UNIT * 1.f;

	//--

	// Main ofParameter controls
	// Exposed to gui and "redirected" to ofNode's internals!
public:
	ofParameter<glm::vec3> positionParameter { "Position", glm::vec3(0),
		glm::vec3(-unitSize), glm::vec3(unitSize) };

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
	ofParameter<glm::vec3> scaleParameter { "Scale", glm::vec3(1),
		glm::vec3(1), glm::vec3(scaleNormalizedUnit) };
#else
	ofParameter<glm::vec3> scaleParameter { "Scale", glm::vec3(1),
		glm::vec3(1), glm::vec3(10) };
#endif
	glm::vec3 scaleParameter_ = { -1, -1, -1 }; //previous value

	ofParameter<glm::vec3> orientationEulerParameter { "Orientation Euler", glm::vec3(0),
		glm::vec3(-(float)SURFING__MAX_DEGREE), glm::vec3((float)SURFING__MAX_DEGREE) };

	ofParameter<bool> bScaleLinkAxis { "ScaleLinkAxis", true }; //TODO

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
	// Extra normalized controls
	// intended to adapt to our scene and objects dimensions.
	ofParameter<float> scaleNormalized { "Scale Norm", 0, -1.f, 1.f };
	ofParameter<int> scaleNormalizedPow { "Scale Pow", scaleNormalizedPowMax / 2, 1, scaleNormalizedPowMax };
	ofParameter<glm::vec3> positionNormalized { "Position Normalized", glm::vec3(0), glm::vec3(-1), glm::vec3(1) };
	ofParameterGroup paramsScaleNormalized;
#endif

	//exposed to the gui and settings
	ofParameterGroup parameters;
	ofParameterGroup paramsOfNode;
	ofParameterGroup paramsResets;

private:
	bool bAttendingPosition = false;
	bool bAttendingScale = false;
	bool bAttendingOrientation = false;

public:
	ofParameter<void> vReset { "Reset" };
	ofParameter<void> vResetScale { "Reset Scale" };
	ofParameter<void> vResetPosition { "Reset Position" };
	ofParameter<void> vResetOrientation { "Reset Orientation" };

	ofParameter<bool> bDraw { "Draw", true };
	ofParameter<bool> bDebug { "Debug", false };

private:
	SurfingAutoSaver autoSaver;
	string name = "";
	string pathSuffix = "_Transform.json";
	string path = "";
	bool bDoneSetup = false;

public:
	//preset
	ofxPanel gui;
	ofParameter<bool> bGui;
	ofParameterGroup paramsPreset;

	void setName(string name_) { //caall before setup
		ofLogNotice("TransformNode") << "setName(" << name_ << ")";
		name = name_;
	}

private:
	void setupSettings() {
		ofLogNotice("TransformNode") << "setupSettings()";

		// build preset group
		if (name == "") name = "Transform";
		//name = "UI " + name;
		bGui.set(name, true);

		paramsPreset.setName(name);

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
		paramsPreset.add(positionNormalized);
		paramsPreset.add(paramsScaleNormalized);
#endif
		paramsPreset.add(paramsOfNode);
		paramsPreset.add(paramsResets);

		path = name + pathSuffix;

		gui.setup(paramsPreset);
	}

	void startup() {
		ofLogNotice("TransformNode") << "startup()";

		if (bEnableSettings) {
			callback_t f = std::bind(&TransformNode::save, this);
			autoSaver.setFunctionSaver(f);

			load();
		}
	}

	void save() {
		ofLogNotice("TransformNode") << "save > " << path;

		ofxSurfing::saveSettings(parameters, path);
	}

	bool load() {
		ofLogNotice("TransformNode") << "load > " << path;

		autoSaver.pause();
		bool b = ofxSurfing::loadSettings(parameters, path);
		autoSaver.start();

		return b;
	}

	void update(ofEventArgs & args) {
		update();
	}

	void update() {
		if (!bDoneSetup) {
			setup();
		}
	}

	void Changed(ofAbstractParameter & e) {
		string name = e.getName();
		ofLogVerbose("TransformNode") << "Changed: " << name << ": " << e;

		if (bEnableSettings)
			if (e.isSerializable()) {
				autoSaver.saveSoon();
			}
	}

	//----

public:
	TransformNode() {
		ofLogNotice("TransformNode") << "Constructor";

		ofAddListener(ofEvents().update, this, &TransformNode::update);
		ofAddListener(parameters.parameterChangedE(), this, &TransformNode::Changed);
	}

	TransformNode(const TransformNode & other) {
		ofLogNotice("TransformNode") << "Constructor";

		ofAddListener(ofEvents().update, this, &TransformNode::update);
		ofAddListener(parameters.parameterChangedE(), this, &TransformNode::Changed);
	}

	~TransformNode() {
		ofLogNotice("TransformNode") << "Destructor";

		ofRemoveListener(ofEvents().update, this, &TransformNode::update);
		ofRemoveListener(parameters.parameterChangedE(), this, &TransformNode::Changed);
	}

	//--

	void refreshGui() {
		ofLogNotice("TransformNode") << "refreshGui()";

		gui.getGroup(paramsOfNode.getName()).getGroup(scaleParameter.getName()).minimize();
		gui.getGroup(paramsOfNode.getName()).getGroup(positionParameter.getName()).minimize();
		gui.getGroup(paramsResets.getName()).minimize();
	}

	void refreshGuiUserParams(ofxPanel & gui_, ofxGuiGroup & group_) {
		ofLogNotice("TransformNode") << "refreshGui(ofxPanel,ofxGuiGroup)";

		group_.getGroup(paramsOfNode.getName()).getGroup(scaleParameter.getName()).minimize();
		group_.getGroup(paramsOfNode.getName()).getGroup(positionParameter.getName()).minimize();
		group_.getGroup(paramsResets.getName()).minimize();
	}

	//--

private:
	bool bEnableSettings = true;

public:
	// use to disable when settings are handled externaly.
	// as when using a transform vector for multiple objects..
	void setEnableSettings(bool b) {
		bEnableSettings = b;
	}

private:
	std::unique_ptr<ofEventListener> e_vResetScale;
	std::unique_ptr<ofEventListener> e_vResetPosition;
	std::unique_ptr<ofEventListener> e_vResetOrientation;
	std::unique_ptr<ofEventListener> e_vReset;

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
	std::unique_ptr<ofEventListener> e_positionNormalized;
	std::unique_ptr<ofEventListener> e_scaleNormalized;
	std::unique_ptr<ofEventListener> e_scaleNormalizedPow;
#endif

	//std::unique_ptr<ofEventListener> e_rotationEulerChanged; //TODO

	//--

public:
	void setup() {
		ofLogNotice("ofxSurfing3dText") << "setup()";

		startupParameters();
		setupCallbacks();
		setupSettings();
		startup();

		bDoneSetup = true;
	}

	//--------------------------------------------------------------
	void startupParameters() {
		ofLogNotice("ofxSurfing3dText") << "startupParameters()";

		parameters.setName("TRANSFORM");

		paramsOfNode.setName("ofNode");
		paramsOfNode.add(positionParameter);
		paramsOfNode.add(orientationEulerParameter);
		paramsOfNode.add(scaleParameter);
		paramsOfNode.add(bScaleLinkAxis); //TODO

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
		paramsScaleNormalized.setName("Scale Normalized");
		paramsScaleNormalized.add(scaleNormalized);
		paramsScaleNormalized.add(scaleNormalizedPow);
#endif

		parameters.add(bDraw);
		parameters.add(bDebug);

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
		parameters.add(positionNormalized);
		parameters.add(paramsScaleNormalized);
#endif
		parameters.add(paramsOfNode);

		paramsResets.setName("Resets");
		paramsResets.add(vResetPosition);
		paramsResets.add(vResetOrientation);
		paramsResets.add(vResetScale);
		paramsResets.add(vReset);
		paramsResets.setSerializable(false);
		parameters.add(paramsResets);
	}

	//--

	// Get the transforms for each model
	// passing the model index as argument:

public:
	bool isEnabled() const {
		return bDraw;
	}
	bool isDebug() const {
		return bDebug;
	}

	//--

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
	float getScaleNormalized() const {
		return scaleNormalized;
	}

	float getScalePow() const {
		return scaleNormalizedPow;
	}

	glm::vec3 getPositionNormalized() const {
		return positionNormalized;
	}
#endif

	//--

	void doReset() {
		doResetPosition();
		doResetOrientation();
		doResetScale();
	}

	void doResetPosition() {
		setPosition(0, 0, 0);

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
		positionNormalized = glm::vec3(0);
#endif
	}

	void doResetOrientation() {
		orientationEulerParameter = glm::vec3(0, 0, 0);
		//setOrientation(glm::vec3(0,0,0));
	}

	void doResetScale() {
		setScale(1, 1, 1);

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
		//scaleNormalizedPow = scaleNormalizedPowMax/2;
		scaleNormalizedPow = 5;
		scaleNormalized = 0;
#endif
	}

	//--

	//https://forum.openframeworks.cc/t/ofnode-and-ofparameter/24436/2

public:
	void setupCallbacks() {
		positionParameter.addListener(this, &TransformNode::ChangedPositionParameter);
		orientationEulerParameter.addListener(this, &TransformNode::ChangedOrientationEulerParameter);
		scaleParameter.addListener(this, &TransformNode::ChangedScaleParameter);

		//--

		// Transform

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
		e_positionNormalized = std::make_unique<ofEventListener>(positionNormalized.newListener([this](glm::vec3) {
			ofLogNotice(__FUNCTION__);

			refreshPositionFromNormalized();
		}));

		e_scaleNormalized = std::make_unique<ofEventListener>(scaleNormalized.newListener([this](float) {
			ofLogNotice(__FUNCTION__);

			refreshScaleFromNormalized();
		}));

		e_scaleNormalizedPow = std::make_unique<ofEventListener>(scaleNormalizedPow.newListener([this](float) {
			ofLogNotice(__FUNCTION__);

			//adapt param range
			scaleNormalizedUnit = scaleNormalizedRatio * scaleNormalizedPow;
			scaleParameter.setMax(glm::vec3(scaleNormalizedUnit, scaleNormalizedUnit, scaleNormalizedUnit));

			refreshScaleFromNormalized();
		}));
#endif

		//e_rotationEulerChanged = std::make_unique<ofEventListener>(orientationEulerParameter.newListener([this](glm::vec3) {
		//}));

		//--

		// Resets

		e_vReset = std::make_unique<ofEventListener>(vReset.newListener([this](void) {
			doReset();
		}));

		e_vResetPosition = std::make_unique<ofEventListener>(vResetPosition.newListener([this](void) {
			doResetPosition();
		}));

		e_vResetScale = std::make_unique<ofEventListener>(vResetScale.newListener([this](void) {
			doResetScale();
		}));
		e_vResetOrientation = std::make_unique<ofEventListener>(vResetOrientation.newListener([this](void) {
			doResetOrientation();
		}));
	}

	//--

	// Params has changed
	// so we update ofNode from the params!

	void ChangedPositionParameter(glm::vec3 & v) {
		if (bAttendingPosition) return;

		ofLogNotice(__FUNCTION__) << v;

		bAttendingPosition = true;
		setPosition(v);
		bAttendingPosition = false;

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
		refreshPositionToNormalized();
#endif
	}

	void ChangedScaleParameter(glm::vec3 & v) {
		if (bAttendingScale) return;

		ofLogNotice(__FUNCTION__) << v;

		// link all axis to which changed!
		if (bScaleLinkAxis) {
			if (scaleParameter.get() != scaleParameter_) {
				glm::vec3 s;
				if (scaleParameter.get().x != scaleParameter_.x)
					s = glm::vec3(scaleParameter.get().x);
				else if (scaleParameter.get().y != scaleParameter_.y)
					s = glm::vec3(scaleParameter.get().y);
				else if (scaleParameter.get().z != scaleParameter_.z)
					s = glm::vec3(scaleParameter.get().z);

				bAttendingScale = true;
				scaleParameter.set(s);
				bAttendingScale = false;

				bAttendingScale = true;
				setScale(s);
				bAttendingScale = false;

				scaleParameter_ = scaleParameter.get();//store previous
			}
		}

		else {
			bAttendingScale = true;
			setScale(v);
			bAttendingScale = false;
		}
	}

	void ChangedOrientationEulerParameter(glm::vec3 & v) {
		if (bAttendingOrientation) return;

		auto o1 = orientationEulerParameter.get();
		auto o2 = getOrientationEuler();
		bool bDiff = (o1 != o2);

		if (bDiff) {
			ofLogNotice(__FUNCTION__) << orientationEulerParameter.get();

			bAttendingOrientation = true;
			setOrientation(v);
			bAttendingOrientation = false;
		}
	}

	//--

	// Update ofParams from ofNode

	void onPositionChanged() override {
		if (bAttendingPosition) return;

		if (positionParameter.get() != getPosition()) {
			ofLogNotice(__FUNCTION__) << getPosition();

			bAttendingPosition = true;
			positionParameter.set(getPosition());
			bAttendingPosition = false;

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
			refreshPositionToNormalized();
#endif
		}
	}

	void onScaleChanged() override {
		if (bAttendingScale) return;

		if (scaleParameter.get() != getScale()) {
			ofLogNotice(__FUNCTION__) << getScale();

			bAttendingScale = true;
			scaleParameter.set(getScale());
			bAttendingScale = false;

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
			refreshScaleToNormalized();
#endif
		}
	}

	void onOrientationChanged() override {
		if (bAttendingOrientation) return;

#if 1
		auto o1 = orientationEulerParameter.get();
		auto o2 = getOrientationEuler();
		bool bDiff = (o1 != o2);
#else
		// compare the two objects with an epsilon of 0.001
		bool bDiff = glm::all(glm::epsilonEqual(orientationEulerParameter.get(), getOrientationEuler(), 0.001f));
#endif

		if (bDiff) {
			ofLogNotice(__FUNCTION__) << getOrientationEuler();

			//ofLogNotice() << "orientationEulerParameter:" << o1;
			//ofLogNotice() << "getOrientationEuler():" << o2;

			refreshOrientationEulerFromOfNode();
		}
	}

	//----

	void refreshOrientationEulerFromOfNode() {
		ofLogNotice(__FUNCTION__) << getOrientationEuler();

		bAttendingOrientation = true;
		orientationEulerParameter.set(getOrientationEuler());
		bAttendingOrientation = false;
	}

	void refreshOrientationNodeFromOfEuler() {
		ofLogNotice(__FUNCTION__) << orientationEulerParameter.get();

		bAttendingOrientation = true;
		setOrientation(orientationEulerParameter.get());
		bAttendingOrientation = false;
	}

	//--

#ifdef SURFING_TRANSFORM_NOSE___USE__NORMALIZED_CONTROLS
	void refreshPositionFromNormalized() {
		ofLogNotice(__FUNCTION__);

		const float u = unitSize; //distance unit
		float x = ofMap(positionNormalized.get().x, -1, 1, -u, u, true);
		float y = ofMap(positionNormalized.get().y, -1, 1, -u, u, true);
		float z = ofMap(positionNormalized.get().z, -1, 1, -u, u, true);
		setPosition(glm::vec3(x, y, z));
		ChangedPositionParameter(glm::vec3(x, y, z));
	}

	void refreshPositionToNormalized() {
		ofLogNotice(__FUNCTION__);

		//apply to normalized
		const float u = positionParameter.getMax().x; // unit assumed the same for the three axis
		float x = ofMap(positionParameter.get().x, -u, u, -1, 1, true);
		float y = ofMap(positionParameter.get().y, -u, u, -1, 1, true);
		float z = ofMap(positionParameter.get().z, -u, u, -1, 1, true);

		float px = positionNormalized.get().x;
		float py = positionNormalized.get().y;
		float pz = positionNormalized.get().z;
		if (px != x || py != y || pz != z) //update only if changed/differs to reduce callbacks
		{
			positionNormalized.set(glm::vec3(x, y, z));
		}
	}

	void refreshScaleFromNormalized() {
		ofLogNotice(__FUNCTION__);

		float s = 1;
		if (scaleNormalized == 0) {
			s = scaleNormalizedPow;
		} else if (scaleNormalized > 0) {
			s = ofMap(scaleNormalized, 0, 1, scaleNormalizedPow, scaleNormalizedPow * scaleNormalizedRatio, true);
		} else if (scaleNormalized < 0) {
			s = ofMap(scaleNormalized, 0, -1, scaleNormalizedPow, scaleNormalizedPow / scaleNormalizedRatio, true);
		}
		setScale(s, s, s);
		ChangedScaleParameter(glm::vec3(s, s, s));
	}

	void refreshScaleToNormalized() {
		ofLogNotice(__FUNCTION__);

		//TODO
		float sx = 0;
		float s = scaleParameter.get().x;
		const float u = scaleParameter.getMax().x; // unit assumed the same for the three axis

		sx = ofMap(s, scaleParameter.getMin().x, scaleParameter.getMax().x, -1, 1, true);

		//if (s == 0) {
		//	sx = 0;
		//} else if (s < scaleNormalizedPow * scaleNormalizedRatio) {
		//	sx = ofMap(s, 0, scaleNormalizedPow / scaleNormalizedRatio, 0, -1, true);
		//}
		//if (s > 0) {
		//	sx = ofMap(s, 0, scaleNormalizedPow * scaleNormalizedRatio, 0, 1, true);
		//}

		//TODO: link
		if (bScaleLinkAxis) {
			scaleNormalized.set(sx);
		}
	}
#endif

	//--

public:
	ofNode & getNode() { return *this; }
	// Exposed to use push/pop matrix:
	// myObject.getNode().transformGL();
	// draw your "transformed" scene
	// myObject.getNode().restoreTransformGL();
};