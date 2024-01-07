
/*
* SurfingFilesBrowser.h
* 
	A class to help loading the many files
	from a path directory (default bin/data/files/)
	Then allows browsing them.

	Another parent (scope) class would be able to load files easily.

	This class can be derived to personalize a browser for different file types,
	as we do with the SurfingFilesBrowserModels.h and his use into SurfingModelsManager.h class, 
	that we use together to browse 3d models.
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
		ofLogNotice("SurfingFilesBrowser") << "SurfingFilesBrowser:SurfingFilesBrowser()";

		ofAddListener(ofEvents().update, this, &SurfingFilesBrowser::update);
		ofAddListener(ofEvents().keyPressed, this, &SurfingFilesBrowser::keyPressed);
	}

	~SurfingFilesBrowser() {
		ofLogNotice("SurfingFilesBrowser") << "SurfingFilesBrowser:~SurfingFilesBrowser()";

		ofRemoveListener(ofEvents().update, this, &SurfingFilesBrowser::update);
		ofRemoveListener(ofEvents().keyPressed, this, &SurfingFilesBrowser::keyPressed);

		ofRemoveListener(parameters.parameterChangedE(), this, &SurfingFilesBrowser::Changed);

		//TODO: fix auto saver!
		//save();
	}

	void update(ofEventArgs & args) {
		updateAutoSwitch();
	}

	void keyPressed(int key) {
		if (!bKeys) return;
		ofLogNotice("SurfingFilesBrowser") << "keyPressed: " << key;

		if (key == OF_KEY_UP) {
			doPrevious();
		}

		else if (key == OF_KEY_DOWN) {
			doNext();
		}
	}

	void keyPressed(ofKeyEventArgs & eventArgs) {
		const int key = eventArgs.key;

		// modifiers
		bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
		bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
		bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
		bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

		ofLogVerbose("ofxSurfing3dText") << "keyPressed";

		keyPressed(key);
	}

public:
	ofParameterGroup parameters;
	ofParameterGroup paramsBrowse;

	ofParameter<string> nameFile { "Name", "" };
	ofParameter<int> indexFile { "FILE", -1, -1, -1 };
	ofParameter<void> vNext { "Next" };
	ofParameter<void> vPrev { "Previous" };
	ofParameter<void> vReset { "Reset" };

	ofParameterGroup paramsExtra;
	ofParameterGroup switcherParams;
	ofParameter<bool> bModeAll { "Mode All", false };
	ofParameter<bool> bAutoSwitch { "Auto Switch", false }; //auto switch to next model
	ofParameter<float> timeAutoSwitch { "Time Switch", 2, 0.05, 10 };
	ofParameter<int> indexAnimation { "Index Anim", -1, -1, -1 };

public:
	void setup(string path = "") { //set path for files. default is bin/data/files/
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:setup(" << path << ")";

		setupDir(path);
		setupParams();
		setupGui();

		buildHelp();

		//--

		// Auto saver
		// Register the local save function to be called when saving is required.
		callback_t f = std::bind(&SurfingFilesBrowser::save, this);
		autoSaver.setFunctionSaver(f);
		autoSaver.setName("SurfingFilesBrowser");

		load();
	}

	virtual void setupParametersExtra() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:setupParametersExtra()";
	}

private:
	virtual void setupGui() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:setupGui()";

		parameters.add(paramsExtra);
		parameters.add(bHelp);
		parameters.add(bKeys);

		gui.setup(parameters);

		gui.getGroup(paramsExtra.getName()).minimize();
	}

private:
	// Default supported formats. in this case for 3d models.
	vector<string> extensions = {
		"ply",
		"fbx",
		"obj"
	};

public:
	// Customize file types. Call before calling setup!
	void setFileExtensions(vector<string> e) {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:setFileExtensions()";
		ofLogNotice("ofxSurfingPBR") << ofToString(e);

		extensions = e;
	}

public:
	void setupDir(string path = "") {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:setupDir(" << path << ")";

		if (path == "") path = "files";
		//bin/data/files/

		pathFiles = path;

		for (size_t i = 0; i < extensions.size(); i++) {
			dir.allowExt(extensions[i]);
		}

		dir.listDir(pathFiles);
		dir.sort();

		if (dir.size() == 0) {
			ofLogError("ofxSurfingPBR") << "SurfingFilesBrowser:setupDir() path or files not found!";
			return;
		}

		indexFile.setMin(0);
		indexFile.setMax(dir.size() - 1);
		indexFile.set(0);
	}

	void setupParams() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:setupParams()";

		nameFile.setSerializable(false);

		paramsBrowse.setName("Files");
		paramsBrowse.add(vNext);
		paramsBrowse.add(vPrev);
		paramsBrowse.add(indexFile);
		paramsBrowse.add(nameFile);

		switcherParams.setName("Switcher");
		switcherParams.add(bAutoSwitch);
		switcherParams.add(timeAutoSwitch);
		switcherParams.add(indexAnimation);

		paramsExtra.setName("Extra");
		paramsExtra.add(bModeAll);
		paramsExtra.add(switcherParams);

		parameters.setName(sTitle);
		parameters.add(paramsBrowse);

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
			doNext();
		});
		listenerPrevious = vPrev.newListener([this](void) {
			doPrevious();
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

	string getPathFile(int i) {
		string p = "";
		if (i == -1) i = indexFile;
		p = dir.getPath(i);
		return p;
	}

	string getFilenamesList() {
		string s = "";
		for (size_t i = 0; i < dir.size(); i++) {
			bool b = (i == indexFile);
			s += b ? ">" : " "; //left
			s += (i < 10 ? "0" : "") + ofToString(i);
			s += " ";
			s += dir.getName(i);
			//s += b ? " <" : "  ";//right
			s += "\n";
		}
		return s;
	}

	void doNext() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:doNext()";

		if (dir.size() > 0) {
			indexFile.setWithoutEventNotifications(indexFile.get() + 1);
			indexFile.set(indexFile % dir.size());
		}
	}

	void doPrevious() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:doPrevious()";

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

	//public:
protected:
	void updateAutoSwitch() {
		if (bAutoSwitch) {
			float t = ofGetElapsedTimef() - timeIndexChanged;
			if (t > timeAutoSwitch.get()) {
				doNext();
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

		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:Changed " << name << ": " << e;

		//if (name == bModeAll.getName()) {
		//	//workflow
		//	if (bAutoSwitch) bAutoSwitch.set(false);
		//}
	}

public:
	const string getPathFiles() {
		string s = pathFiles;
		if (s == "") {
			ofLogError("ofxSurfingPBR") << "SurfingFilesBrowser:Models path not settled properly or unknown!";
			return s;
		}
		return s;
	}

	string pathFile = ""; //to get the path for the FILE. ready to load!
	string pathFiles = ""; //for display only

	SurfingAutoSaver autoSaver;
	string pathSettings = "SurfingFilesBrowser.json";

	virtual void save() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:save()";
		ofxSurfing::saveSettings(parameters, pathSettings);
	}

	virtual void load() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:load()";
		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, pathSettings);
		autoSaver.start();
	}

	//--

public:
	ofxPanel gui;

	void drawGui() {
		if (!bGui) return;
		gui.draw();
	}

	void setGuiPosition(glm::vec2 pos) {
		gui.setPosition(pos.x, pos.y);
	}

	virtual void refreshGui() {
		//gui.getGroup(paramsExtra.getName()).minimize();
	}

	void setTitle(string s) { //call before setup
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:setTitle(" << s << ")";

		sTitle = s;

		string n;
		n = "UI " + s;
		bGui.setName(n);
	}

private:
	string sTitle = "FILE";
	string sHelp;

	void buildHelp() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowser:buildHelp()";

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
	ofParameter<bool> bGui { "UI FilesBrowser", true };
	ofParameter<bool> bHelp { "Help", true };
	ofParameter<bool> bKeys { "Keys", false };

	void drawHelp(bool bBottom = false) {
		if (!bHelp) return;

		if (bBottom)
			ofxSurfing::ofDrawBitmapStringBox(sHelp, &gui, ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER);
		else
			ofxSurfing::ofDrawBitmapStringBox(sHelp, &gui, ofxSurfing::SURFING_LAYOUT_TOP_RIGHT);

		//ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
	}
};