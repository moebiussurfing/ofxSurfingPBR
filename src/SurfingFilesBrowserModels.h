/*
	TODO

	- make aux transform params with makeRef
		to link to loaded object,
		then avoiding to expose all the object 
		transforms folders to the gui

	- add bool draw to each object to allow all models mode arrange.
*/

/*
* SurfingFilesBrowserModels.h
* 
	A class to help loading the many model files
	from a path directory (default bin/data/models/)
	Then allows browsing them and apply some basic transforms
	that are persistent.

	Transforms are stored as JSON paired to the model file name with a suffix.
	So we can add and remove more files to the path 
	without breaking persistent settings!

	Another parent (scope) class would be able to load files easily.
	It has help info and displays to debug selected browser,
	get the path to be passed to the model/mesh loaders..etc

	See the EXAMPLE at the bottom of this file.
*/

//--

#pragma once

#include "ofMain.h"

#include "SurfingFilesBrowser.h"

//--

class Transform {
public:
	ofParameter<int> scalePow { "ScalePow", 0, -100, 100 };
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "y Pos", 0, -1.f, 1.f };
	ofParameter<float> yRot { "y Rot", 0, -1.f, 1.f };
	ofParameter<glm::vec3> rot { "Rot", glm::vec3(0), glm::vec3(-180), glm::vec3(180) };

	ofParameterGroup parameters {
		"Transform",
		scalePow, scale, yPos, yRot, rot
	};

	Transform() {
	}

	~Transform() {
	}
};

//--

class SurfingFilesBrowserModels : public SurfingFilesBrowser {
public:
	SurfingFilesBrowserModels() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:SurfingFilesBrowserModels()";
	}

	~SurfingFilesBrowserModels() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:~SurfingFilesBrowserModels()";
	}

private:
	void setupGui() override {
		gui.setup(parameters);

		setupParamsTransforms();
		gui.getGroup(transformParams.getName()).minimize();

		gui.getGroup(extraParams.getName()).minimize();
	}

	void setupParamsTransforms() {

		//--

		transformParams.setName("Transforms");
		transformParams.add(vReset);

		transforms.clear();
		for (size_t i = 0; i < dir.size(); i++) {
			Transform t = Transform();
			t.parameters.setName(getFilename(i));
			transforms.emplace_back(t);

			transformParams.add(t.parameters);
		}

		parameters.add(transformParams);

		transformParams.setSerializable(false);
	}

public:
	void doReset() override {
		resetTransform();
	}

public:
	string extSuffixTransform = "__Transform.json";

	void saveTransforms() {
		ofLogNotice("SurfingFilesBrowserModels") << "saveTransforms()";
		for (size_t i = 0; i < transforms.size(); i++) {
			ofJson j;
			string p = pathModels + "\\" + getName(i) + extSuffixTransform;

			ofJson settings;
			ofSerialize(settings, transforms[i].parameters);
			bool b = ofSavePrettyJson(p, settings);
		}
	}

	void loadTransforms() {
		for (size_t i = 0; i < transforms.size(); i++) {
			string p = pathModels + "\\" + getName(i) + extSuffixTransform;

			ofJson settings;
			settings = ofLoadJson(p);
			ofDeserialize(settings, transforms[i].parameters);
		}
	}

	void save() override {
		ofLogNotice("SurfingFilesBrowserModels") << "save()";
		ofxSurfing::saveSettings(parameters, pathSettings);

		saveTransforms();
	}

	void load() override {
		ofLogNotice("SurfingFilesBrowserModels") << "load()";
		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, pathSettings);
		autoSaver.start();

		loadTransforms();
	}

	//--

public:
	void refreshGui() override {
		ofLogNotice("SurfingFilesBrowserModels") << "refreshGui()";

		for (size_t i = 0; i < transforms.size(); i++) {
			Transform t = transforms[i];
			string n = getFilename(i);
			bool b = (i == indexFile); //selected
			auto & g = gui.getGroup(transformParams.getName()).getGroup(n);
			b ? g.maximize() : g.minimize();
			g.getGroup(t.rot.getName()).minimize();
		}

		//gui.getGroup(extraParams.getName()).minimize();
	}

	//--

	// Store each model transforms for gizmo

private:
	vector<Transform> transforms;

public:
	float getTransformScale(int i = -1) {
		float v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].scale;
		return v;
	}
	float getTransformScalePow(int i = -1) {
		int v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].scalePow;
		return v;
	}
	float getTransformPos(int i = -1) {
		float v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].yPos;
		return v;
	}
	float getTransformRotY(int i = -1) {
		float v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].yRot;
		return v;
	}
	glm::vec3 getTransformRotVec(int i = -1) {
		glm::vec3 v = glm::vec3(0);
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].rot;
		return v;
	}

	void resetTransform(int i = -1) {
		if (i == -1) i = indexFile;
		if (i < transforms.size()) {
			transforms[i].scalePow = 0;
			transforms[i].scale = 0;
			transforms[i].yPos = 0;
			transforms[i].yRot = 0;
			transforms[i].rot = glm::vec3(0);
		}
	}

	ofParameterGroup transformParams;
};