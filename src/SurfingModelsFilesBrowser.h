/*
* SurfingModelsFilesBrowser.h
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

class Transform {
public:
	ofParameter<int> scalePow { "ScalePow", 0, -100, 100 };
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "y Pos", 0, -1.f, 1.f };
	ofParameter<float> yRot { "y Rot", 0, -1.f, 1.f };

	ofParameterGroup parameters {
		"Transform",
		scalePow, scale, yPos, yRot
	};

	Transform() {
	}

	~Transform() {
	}
};

//--

class SurfingModelsFilesBrowser {
public:
	SurfingModelsFilesBrowser();
	~SurfingModelsFilesBrowser();

public:
	void setup(string path = "") {
		setupDir(path);

		//--

		setupParams();

		// Gui
		gui.setup(parameters);
		gui.getGroup(transformParams.getName()).minimize();

		//--

		// Auto saver
		// Register the local save function to be called when saving is required.
		callback_t f = std::bind(&SurfingModelsFilesBrowser::save, this);
		autoSaver.setFunctionSaver(f);
		autoSaver.setName("SurfingModelsFilesBrowser");

		load();
	}

	void setupDir(string path = "") {

		if (path == "") path = "models";

		// bin/data/models/

		pathModels = path;

		//supported formats
		dir.allowExt("ply");
		dir.allowExt("fbx");
		dir.allowExt("obj");

		dir.listDir(pathModels);
		dir.sort();

		if (dir.size() == 0) {
			ofLogError("SurfingModelsFilesBrowser") << "setupDir() path or files not found!";
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

		//--

		parameters.add(transformParams);

		transformParams.setSerializable(false);

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

		listenerNext = vNext.newListener([this](void) {
			next();
		});
		listenerPrevious = vPrev.newListener([this](void) {
			previous();
		});

		listenerReset = vReset.newListener([this](void) {
			doReset();
		});

		ofAddListener(parameters.parameterChangedE(), this, &SurfingModelsFilesBrowser::Changed);
	}

	void doProcessIndex(int i) {

		nameFile = dir.getName(i);

		pathModel = dir.getPath(i);
		vLoad.trigger();

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

private:
	ofDirectory dir;

public:
	ofParameterGroup parameters;

	ofParameter<string> nameFile { "Name", "" };
	ofParameter<int> indexFile { "FILE", -1, -1, -1 };
	ofParameter<void> vNext { "Next" };
	ofParameter<void> vPrev { "Previous" };

	ofParameter<void> vReset { "Reset" };

	void doReset() {
		resetTransform();
	}

private:
	ofEventListener listenerReset;

public:
	ofParameter<void> vLoad { "LoadBang" };
	// to be listened from parent the scope!
	// then will load the model getting the path:
	// pathModel

private:
	ofEventListener listenerIndexFile;
	ofEventListener listenerNext;
	ofEventListener listenerPrevious;

	void Changed(ofAbstractParameter & e) {
		autoSaver.saveSoon();
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
	string extSuffixTransform = "__Transform.json";

	SurfingAutoSaver autoSaver;
	string pathSettings = "SurfingModelsFilesBrowser.json";

	void save() {
		ofLogNotice("ofSurfingModelsApp") << "save()";
		ofxSurfing::saveSettings(parameters, pathSettings);

		saveTransforms();
	}
	void load() {
		ofLogNotice("SurfingModelsFilesBrowser") << "load()";
		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, pathSettings);
		autoSaver.start();

		loadTransforms();
	}
	void saveTransforms() {
		ofLogNotice("SurfingModelsFilesBrowser") << "saveTransforms()";
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

	//--

public:
	ofxPanel gui;

	void drawGui() {
		gui.draw();
	}

	void setGuiPosition(glm::vec2 pos) {
		gui.setPosition(pos.x, pos.y);
	}

	void refreshGui() {
		for (size_t i = 0; i < transforms.size(); i++) {
			Transform t = transforms[i];
			string n = getFilename(i);
			bool b = (i == indexFile); //selected
			auto & g = gui.getGroup(transformParams.getName()).getGroup(n);
			b ? g.maximize() : g.minimize();
		}
	}

private:
	string sHelp;
	void buildHelp() {
		sHelp = "";
		sHelp += "MODELS\n";
		sHelp += "\n";
		sHelp += " " + this->getFilename() + "\n";
		sHelp += "\n";
		sHelp += "BROWSE\n";
		sHelp += "\n";
		sHelp += "UP   Prev\n";
		sHelp += "DOWN Next\n";
		sHelp += "\n";
		sHelp += this->getFilenamesList();
	}

public:
	ofParameter<bool> bHelp { "Help", true };

	void drawHelp() {
		if (!bHelp) return;
		ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
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
	float getTransformRot(int i = -1) {
		float v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].yRot;
		return v;
	}

	void resetTransform(int i = -1) {
		if (i == -1) i = indexFile;
		if (i < transforms.size()) {
			transforms[i].scalePow = 0;
			transforms[i].yPos = 0;
			transforms[i].yRot = 0;
			transforms[i].scale = 0;
		}
	}

	ofParameterGroup transformParams;
};

SurfingModelsFilesBrowser::SurfingModelsFilesBrowser() {
}

SurfingModelsFilesBrowser::~SurfingModelsFilesBrowser() {
	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingModelsFilesBrowser::Changed);

	//TODO: fix auto saver!
	//save();
}

//------

/*
	EXAMPLE

	// .h
	{
		#include "SurfingModelsFilesBrowser.h"

		SurfingModelsFilesBrowser filesBrowser;
		ofEventListener listenerLoadModel;
		ofEventListener listenerIndexModel;
	}

	// .cpp
	// setup
	{
		filesBrowser.setup();

		// Callback to trig the model file loading.
		// The model path is ready on filesBrowser.pathModel!
		listenerLoadModel = filesBrowser.vLoad.newListener([this](void) {
			string path = filesBrowser.pathModel;
			this->loadModel(path);
		});

		listenerIndexModel = filesBrowser.indexFile.newListener([this](int & i) {
			//index changed
		});
	
		gui.add(filesBrowser.parameters);
	}
	// draw
	{
		// example of transforms applier
		{
			float yUnit = 500;
			float scaleUnit = 1000;
			float scalePow = filesBrowser.getTransformScalePow();
			if (scalePow == 0) {
			} else if (scalePow < 1) {
				scaleUnit = scaleUnit / (float)abs(scalePow - 1);
			} else if (scalePow > 1) {
				scaleUnit = scaleUnit * (float)abs(scalePow + 1);
			}

			float y = ofMap(filesBrowser.getTransformPos(), -1, 1, -yUnit, yUnit, true);
			float s = ofMap(filesBrowser.getTransformScale(), -1, 1, 1.f / scaleUnit, scaleUnit, true);
			float r = ofMap(filesBrowser.getTransformRot(), -1, 1, -180, 180, true);

			ofPushMatrix();
			ofTranslate(0, y, 0);
			ofScale(s, s, s);
			ofRotateYDeg(r);
			{
				// DRAW MODEL OBJECT
			}
			ofPopMatrix();
		}

		filesBrowser.drawGui();
		filesBrowser.drawHelp();
	}
	// keyPressed
	{
		if (key == OF_KEY_RIGHT) {
			filesBrowser.next();
		}
		if (key == OF_KEY_LEFT) {
			filesBrowser.previous();
		}
	}
*/