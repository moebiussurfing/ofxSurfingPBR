
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
*/

//--

/*
	TODO

	- make aux transform params with makeRef
		to link to loaded object,
		then avoiding to expose all the object 
		transforms folders to the gui
		and show only the selected object!
*/

//--

#pragma once

#include "ofMain.h"

#include "SurfingTransformNode.h"
#include "SurfingFilesBrowser.h"

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
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:setupGui()";

		setupParamsTransforms();

		parameters.add(paramsExtra);
		parameters.add(bHelp);

		gui.setup(parameters);

		//gui.getGroup(transformParams.getName()).minimize();
		gui.getGroup(paramsExtra.getName()).minimize();
	}

	void setupParamsTransforms() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:setupParamsTransforms()";

		vReset.setName("Reset Model");

		transformParams.clear();
		transformParams.setName("Transforms");
		transformParams.add(vReset);

		transforms.clear();//clear all objects nodes

		for (size_t i = 0; i < dir.size(); i++) {
			transforms.push_back(std::make_unique<TransformNode>());

			string n = getFilename(i);
			//transforms.back()->setName(n);
			transforms.back()->setEnableSettings(false);//in this case are handled by this class!!
			transforms.back()->setup();
			transforms.back()->parameters.setName(n);

			// queue all objects nodes params
			transformParams.add(transforms.back()->parameters);
		}

		transformParams.setSerializable(false);

		parameters.add(transformParams);
	}

public:
	void doReset() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:doReset()";

		doResetTransform();
	}

public:
	string extSuffixTransform = "__Transform.json";

	void saveTransforms() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:saveTransforms()";

		for (size_t i = 0; i < transforms.size(); i++) {
			ofJson j;
			string p = pathFiles + "\\" + getName(i) + extSuffixTransform;

			ofJson settings;
			ofSerialize(settings, transforms[i]->parameters);
			bool b = ofSavePrettyJson(p, settings);
		}
	}

	void loadTransforms() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:loadTransforms()";

		for (size_t i = 0; i < transforms.size(); i++) {
			string p = pathFiles + "\\" + getName(i) + extSuffixTransform;

			ofJson settings;
			settings = ofLoadJson(p);
			ofDeserialize(settings, transforms[i]->parameters);
		}
	}

	void save() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:save()";

		ofxSurfing::saveSettings(parameters, pathSettings);

		saveTransforms();
	}

	void load() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:load()";

		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, pathSettings);
		autoSaver.start();

		loadTransforms();
	}

	//--

public:
	void refreshGui() override {
		//return;
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:refreshGui()";

		for (size_t i = 0; i < transforms.size(); i++) {
//			TransformNode t = transforms[i];
		
			string n = getFilename(i);
			bool b = (i == indexFile); //selected
			auto & g = gui.getGroup(transformParams.getName()).getGroup(n);
			b ? g.maximize() : g.minimize();

			transforms[i]->refreshGuiUserParams(gui,g);
		}

	}

	//--

	// Store each model transforms for gizmo.

private:
	std::vector<std::unique_ptr<TransformNode>> transforms;

	//--

	// Get the transforms for each model
	// passing the model index as argument:

public:
	bool isEnabled(int i = -1) const {
		bool b;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			b = transforms[i]->bDraw;
		return b;
	}

	void drawOfNodeIfDebug(int i = -1) const {
		if (!transforms[i]->bDebug) return;

		if (i == -1) i = indexFile;
		transforms[i]->draw();
	}

	ofNode & getNode(int i=-1) {
		if (i == -1) i = indexFile;
		return transforms[i]->getNode();
	}

	ofParameterGroup transformParams;

	void doResetTransform(int i = -1) {
		if (i == -1) i = indexFile;
		if (i < transforms.size()) {
			transforms[i]->doReset();
			//transforms[i]->scaleNormalizedPow = 0;
			//transforms[i]->scaleNormalized = 0;
			//transforms[i]->positionNormalized = glm::vec3(0);
			//transforms[i]->rotationEuler = glm::vec3(0);
		}
	}

	//float getTransformScale(int i = -1) const {
	//	float v = 0;
	//	if (i == -1) i = indexFile;
	//	if (i < transforms.size())
	//		v = transforms[i]->scaleNormalized;
	//	return v;
	//}

	//float getTransformScalePow(int i = -1) const {
	//	int v = 0;
	//	if (i == -1) i = indexFile;
	//	if (i < transforms.size())
	//		v = transforms[i]->scaleNormalizedPow;
	//	return v;
	//}

	//glm::vec3 getTransformPosition(int i = -1) const {
	//	glm::vec3 v = glm::vec3(0);
	//	if (i == -1) i = indexFile;
	//	if (i < transforms.size())
	//		v = transforms[i]->positionNormalized;
	//	return v;
	//}

	//glm::vec3 getTransformRotation(int i = -1) const {
	//	glm::vec3 v = glm::vec3(0);
	//	if (i == -1) i = indexFile;
	//	if (i < transforms.size())
	//		v = transforms[i]->rotationEuler;
	//	return v;
	//}
};