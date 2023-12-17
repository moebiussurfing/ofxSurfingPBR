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
	ofParameterGroup browseParams;

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
	void setup(string path = "") { //set path for files. default is bin/data/files/

		setupDir(path);
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
		parameters.add(extraParams);

		gui.setup(parameters);

		gui.getGroup(extraParams.getName()).minimize();
	}

	//supported formats. in this case for 3d models.
	vector<string> extensions = {
		"ply",
		"fbx",
		"obj"
	};

public:
	void setFileExtension(vector<string> e) { 
		extensions = e;
	}
	
public:
	void setupDir(string path = "") {

		if (path == "") path = "files";
		//bin/data/files/

		pathFiles = path;

		for (size_t i = 0; i < extensions.size(); i++) {
			dir.allowExt(extensions[i]);
		}

		dir.listDir(pathFiles);
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

		browseParams.setName("FILES");
		browseParams.add(vNext);
		browseParams.add(vPrev);
		browseParams.add(indexFile);
		browseParams.add(nameFile);

		extraParams.setName("Extra");
		extraParams.add(bModeAll);
		extraParams.add(bAutoSwitch);
		extraParams.add(timeAutoSwitch);
		extraParams.add(indexAnimation);

		parameters.setName(sTitle);
		parameters.add(browseParams);
		//parameters.add(extraParams);

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

		pathFile = dir.getPath(i);
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

protected:
	ofDirectory dir;

public:
	virtual void doReset() {
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
	// pathFile

protected:
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
		string s = pathFiles;
		if (s == "") {
			ofLogError() << "Models path not settled properly or unknown!";
			return s;
		}
		return s;
	}

	string pathFile = ""; //to get the path for the FILE. ready to load!
	string pathFiles = ""; //for display only

	SurfingAutoSaver autoSaver;
	string pathSettings = "SurfingFilesBrowser.json";

	virtual void save() {
		ofLogNotice("ofSurfingModelsApp") << "save()";
		ofxSurfing::saveSettings(parameters, pathSettings);
	}

	virtual void load() {
		ofLogNotice("SurfingFilesBrowser") << "load()";
		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, pathSettings);
		autoSaver.start();
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

	virtual void refreshGui() {
		//gui.getGroup(extraParams.getName()).minimize();
	}

	void setTitle(string s) {
		sTitle = s;
	}

private:
	string sTitle = "FILE";
	string sHelp;

	void buildHelp() {
		sHelp = "";
		sHelp += sTitle + "\n";
		sHelp += this->getFilename() + "\n";
		sHelp += "\n";
		sHelp += "BROWSE FILES\n";
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
};