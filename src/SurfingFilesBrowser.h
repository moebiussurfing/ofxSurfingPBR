/*
	TODO

	- make aux transform params with makeRef
		to link to loaded object,
		then avoiding to expose all the object 
		transforms folders to the gui

	- add bool draw to each object to allow all models mode arrange.
*/

/*
* SurfingFilesBrowser.h
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

#include "ofxSurfingHelpersLite.h"
#include <functional>
using callback_t = std::function<void()>;

// Optional
// Requires to check previously
// the sanity of the files you pretend to load!
// bc some could freeze the app or too big!

//--

class SurfingFilesBrowser {
public:
	SurfingFilesBrowser() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:SurfingFilesBrowser()";
	}

	~SurfingFilesBrowser() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:~SurfingFilesBrowser()";

		ofRemoveListener(parameters.parameterChangedE(), this, &SurfingFilesBrowser::Changed);

		//TODO: fix auto saver!
		//save();
	}

public:
	ofParameterGroup parameters;

	ofParameter<string> nameFile { "Name", "" };
	ofParameter<int> indexFile { "FILE", -1, -1, -1 };
	ofParameter<void> vNext { "Next" };
	ofParameter<void> vPrev { "Previous" };

	ofParameter<void> vReset { "Reset" };

	ofParameterGroup extraParams;
	ofParameter<bool> bModeAll { "All Models", false };
	ofParameter<bool> bAutoSwitch { "Auto Switch", false }; //auto switch to next model
	ofParameter<float> timeAutoSwitch { "Time Switch", 2, 0.05, 10 };
	ofParameter<int> indexAnimation { "Index Anim", -1, -1, -1 };

public:
	void setup(string path = "") {
		setupDir(path);

		//--

		setupParams();

		setupGui();

		//--

		// Auto saver
		// Register the local save function to be called when saving is required.
		callback_t f = std::bind(&SurfingFilesBrowser::save, this);
		autoSaver.setFunctionSaver(f);
		autoSaver.setName("SurfingFilesBrowser");

		load();
	}

private:
	virtual void setupGui() {
		gui.setup(parameters);

		//gui.getGroup(transformParams.getName()).minimize();

		gui.getGroup(extraParams.getName()).minimize();
	}

public:
	void setupDir(string path = "") {

		if (path == "") path = "models";

		// bin/data/models/

		pathModels = path;

		//supported formats
		vector<string> extensions = {
			"ply",
			"fbx",
			"obj"
		};

		for (size_t i = 0; i < extensions.size(); i++) {
			dir.allowExt(extensions[i]);
		}

		dir.listDir(pathModels);
		dir.sort();

		if (dir.size() == 0) {
			ofLogError("SurfingFilesBrowser") << "setupDir() path or files not found!";
			return;
		}

		indexFile.setMin(0);
		indexFile.setMax(dir.size() - 1);
		indexFile.set(0);
	}

	void setupParams() {
		nameFile.setSerializable(false);

		parameters.setName("MODELS");
		parameters.add(vNext);
		parameters.add(vPrev);
		parameters.add(indexFile);
		parameters.add(nameFile);

		extraParams.setName("Extra");
		extraParams.add(bModeAll);
		extraParams.add(bAutoSwitch);
		extraParams.add(timeAutoSwitch);
		extraParams.add(indexAnimation);
		parameters.add(extraParams);

		//--

		//transformParams.setName("Transforms");
		//transformParams.add(vReset);

		//transforms.clear();
		//for (size_t i = 0; i < dir.size(); i++) {
		//	TransformSimple t = TransformSimple();
		//	t.parameters.setName(getFilename(i));
		//	transforms.emplace_back(t);

		//	transformParams.add(t.parameters);
		//}

		//parameters.add(transformParams);

		//transformParams.setSerializable(false);

		//--

		parameters.add(bHelp);

		//--

		listenerIndexFile = indexFile.newListener([this](int & i) {
			static int i_ = -1;
			if (i != i_) { //check that changed

				//load new index
				if (i < dir.size()) {
					doProcessIndex(i);
				}

				i_ = i;
			}
		});

		listenerAnimationIndex = indexAnimation.newListener([this](int & i) {
			static int i_ = -1;
			if (i != i_) { //check that changed

				i_ = i;
			}
		});

		listenerNext = vNext.newListener([this](void) {
			next();
		});
		listenerPrevious = vPrev.newListener([this](void) {
			previous();
		});

		listenerReset = vReset.newListener([this](void) {
			doReset();
		});

		ofAddListener(parameters.parameterChangedE(), this, &SurfingFilesBrowser::Changed);
	}

	void doProcessIndex(int i) {

		nameFile = dir.getName(i);

		pathModel = dir.getPath(i);
		vLoad.trigger();

		timeIndexChanged = ofGetElapsedTimef();

		refreshGui();

		buildHelp();
	}

	size_t getAmountFiles() {
		return dir.size();
	}

	const size_t getIndexFile() {
		return indexFile.get();
	}

	string getFilename(int i) {
		string s = "NONE";
		if (i < dir.size()) s = dir.getName(i);
		return s;
	}

	string getName(int i) {
		string s = "NONE";
		if (i < dir.size()) {

			ofFile file(dir.getPath(i));
			s = file.getBaseName();
		}
		return s;
	}

	string getFilename() {
		return getFilename(indexFile);
	}

	string getPathModel(int i) {
		string p = "";
		if (i == -1) i = indexFile;
		p = dir.getPath(i);
		return p;
	}

	string getFilenamesList() {
		string s = "";
		for (size_t i = 0; i < dir.size(); i++) {
			bool b = (i == indexFile);
			s += b ? ">" : " ";
			s += ofToString(i);
			s += " ";
			s += dir.getName(i);
			s += "\n";
		}
		return s;
	}

	void next() {
		if (dir.size() > 0) {
			indexFile.setWithoutEventNotifications(indexFile.get() + 1);
			indexFile.set(indexFile % dir.size());
		}
	}
	void previous() {
		if (dir.size() > 0) {
			if (indexFile.get() > 0) {
				indexFile.setWithoutEventNotifications(indexFile.get() - 1);
				indexFile.set(indexFile % dir.size());
			} else {
				if (indexFile.get() <= 0) {
					indexFile.set(dir.size() - 1);
				}
			}
		}
	}

public:
	void updateAutoSwitch() {
		if (bAutoSwitch) {
			float t = ofGetElapsedTimef() - timeIndexChanged;
			if (t > timeAutoSwitch.get()) {
				next();
			}
		}
	}

	//private:
protected:
	ofDirectory dir;

public:
	virtual void doReset() {
		//resetTransform();
	}

private:
	float timeIndexChanged;

private:
	ofEventListener listenerReset;
	ofEventListener listenerAnimationIndex;

public:
	ofParameter<void> vLoad { "LoadBang" };
	// to be listened from parent the scope!
	// then will load the model getting the path:
	// pathModel

protected:
	//private:
	ofEventListener listenerIndexFile;
	ofEventListener listenerNext;
	ofEventListener listenerPrevious;

	void Changed(ofAbstractParameter & e) {
		autoSaver.saveSoon();

		std::string name = e.getName();

		ofLogNotice("ofxSurfingPBR") << "Changed " << name << ": " << e;

		//if (name == bModeAll.getName()) {
		//	//workflow
		//	if (bAutoSwitch) bAutoSwitch.set(false);
		//}
	}

public:
	const string getPathModels() {
		string s = pathModels;
		if (s == "") {
			ofLogError() << "Models path not settled properly or unknown!";
			return s;
		}
		return s;
	}

	string pathModel = ""; //to get the path for the model. ready to load!
	string pathModels = ""; //for display only
	//string extSuffixTransform = "__Transform.json";

	SurfingAutoSaver autoSaver;
	string pathSettings = "SurfingFilesBrowser.json";

	virtual void save() {
		ofLogNotice("ofSurfingModelsApp") << "save()";
		ofxSurfing::saveSettings(parameters, pathSettings);

		//saveTransforms();
	}

	virtual void load() {
		ofLogNotice("SurfingFilesBrowser") << "load()";
		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, pathSettings);
		autoSaver.start();

		//loadTransforms();
	}

	//void saveTransforms() {
	//	ofLogNotice("SurfingFilesBrowser") << "saveTransforms()";
	//	for (size_t i = 0; i < transforms.size(); i++) {
	//		ofJson j;
	//		string p = pathModels + "\\" + getName(i) + extSuffixTransform;

	//		ofJson settings;
	//		ofSerialize(settings, transforms[i].parameters);
	//		bool b = ofSavePrettyJson(p, settings);
	//	}
	//}

	//void loadTransforms() {
	//	for (size_t i = 0; i < transforms.size(); i++) {
	//		string p = pathModels + "\\" + getName(i) + extSuffixTransform;

	//		ofJson settings;
	//		settings = ofLoadJson(p);
	//		ofDeserialize(settings, transforms[i].parameters);
	//	}
	//}

	//--

public:
	ofxPanel gui;

	void drawGui() {
		gui.draw();
	}

	void setGuiPosition(glm::vec2 pos) {
		gui.setPosition(pos.x, pos.y);
	}

	virtual void refreshGui() {
		//for (size_t i = 0; i < transforms.size(); i++) {
		//	TransformSimple t = transforms[i];
		//	string n = getFilename(i);
		//	bool b = (i == indexFile); //selected
		//	auto & g = gui.getGroup(transformParams.getName()).getGroup(n);
		//	b ? g.maximize() : g.minimize();
		//	g.getGroup(t.rot.getName()).minimize();
		//}

		//gui.getGroup(extraParams.getName()).minimize();
	}

private:
	string sHelp;
	void buildHelp() {
		sHelp = "";
		sHelp += "MODEL\n";
		//sHelp += "\n";
		sHelp += this->getFilename() + "\n";
		sHelp += "\n";
		sHelp += "BROWSE MODELS\n";
		sHelp += "\n";
		sHelp += "DOWN Next\n";
		sHelp += "UP   Previous\n";
		sHelp += "\n";
		sHelp += this->getFilenamesList();
	}

public:
	ofParameter<bool> bHelp { "Help", true };

	void drawHelp() {
		if (!bHelp) return;

		//ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
		ofxSurfing::ofDrawBitmapStringBox(sHelp, &gui, ofxSurfing::SURFING_LAYOUT_TOP_RIGHT);
	}

	//--

	//	// Store each model transforms for gizmo
	//
	//private:
	//	vector<TransformSimple> transforms;
	//
	//	//TODO: move to SurfingModelsManager ?
	//	//TODO: and make a more flexible class to be use with other file formats?
	//public:
	//	float getTransformScale(int i = -1) {
	//		float v = 0;
	//		if (i == -1) i = indexFile;
	//		if (i < transforms.size())
	//			v = transforms[i].scale;
	//		return v;
	//	}
	//	float getTransformScalePow(int i = -1) {
	//		int v = 0;
	//		if (i == -1) i = indexFile;
	//		if (i < transforms.size())
	//			v = transforms[i].scalePow;
	//		return v;
	//	}
	//	float getTransformPos(int i = -1) {
	//		float v = 0;
	//		if (i == -1) i = indexFile;
	//		if (i < transforms.size())
	//			v = transforms[i].yPos;
	//		return v;
	//	}
	//	float getTransformRotY(int i = -1) {
	//		float v = 0;
	//		if (i == -1) i = indexFile;
	//		if (i < transforms.size())
	//			v = transforms[i].yRot;
	//		return v;
	//	}
	//	glm::vec3 getTransformRotVec(int i = -1) {
	//		glm::vec3 v = glm::vec3(0);
	//		if (i == -1) i = indexFile;
	//		if (i < transforms.size())
	//			v = transforms[i].rot;
	//		return v;
	//	}
	//
	//	void resetTransform(int i = -1) {
	//		if (i == -1) i = indexFile;
	//		if (i < transforms.size()) {
	//			transforms[i].scalePow = 0;
	//			transforms[i].scale = 0;
	//			transforms[i].yPos = 0;
	//			transforms[i].yRot = 0;
	//			transforms[i].rot = glm::vec3(0);
	//		}
	//	}
	//
	//	ofParameterGroup transformParams;
};