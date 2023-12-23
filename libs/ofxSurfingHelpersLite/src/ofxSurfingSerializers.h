#pragma once
#include "ofMain.h"

//------

namespace ofxSurfing {

//------

/*
* Settings Management
* 
	Settings de/serializers for ofParameterGroup.
*/

// Load
//--------------------------------------------------------------
inline bool loadSettings(ofParameterGroup & parameters, string path = "") {
	if (path == "") path = parameters.getName() + ".json";

	//verify that the file exist
	ofFile f;
	bool b = f.doesFileExist(path);
	if (b)
		ofLogVerbose("ofxSurfing") << "Found settings file: " << path << " for ofParameterGroup: " << parameters.getName();
	else
		ofLogError("ofxSurfing") << "File " << path
								 << " for ofParameterGroup " << parameters.getName() << " not found!";
	//load if exist
	if (b) {
		ofJson settings;
		settings = ofLoadJson(path);
		ofDeserialize(settings, parameters);
	}

	return b;
}

// Save
//--------------------------------------------------------------
inline bool saveSettings(ofParameterGroup & parameters, string path = "") {
	if (path == "") path = parameters.getName() + ".json";

	//if the destination folder do not exists, will be created.
	if (!ofDirectory::doesDirectoryExist(ofFilePath::getEnclosingDirectory(path))) {
		if (ofFilePath::createEnclosingDirectory(path))
			ofLogWarning("ofxSurfing") << "Created enclosing folder for: " << path;
		else
			ofLogError("ofxSurfing") << "Unable to create enclosing folder for: " << path;
	}
	//try to save the file
	ofJson settings;
	ofSerialize(settings, parameters);
	bool b = ofSavePrettyJson(path, settings);
	if (b)
		ofLogVerbose("ofxSurfing") << "Saved ofParameterGroup: " << parameters.getName() << " to " << path;
	else
		ofLogError("ofxSurfing") << "Error saving: " << parameters.getName() << " to " << path;

	return b;
}

// Create if a folder path is found or not
//--------------------------------------------------------------
inline void checkFolderOrCreate(string path) {
	if (!ofDirectory::doesDirectoryExist(ofFilePath::getEnclosingDirectory(path))) {
		if (ofFilePath::createEnclosingDirectory(path, true))
			ofLogWarning("ofxSurfing") << "Created enclosing folder for: " << path;
		else
			ofLogError("ofxSurfing") << "Unable to create enclosing folder for: " << path;
	}
	ofLogVerbose("ofxSurfing") << "Found enclosing folder for: " << path;
}

// Create if a file path is found or not
//--------------------------------------------------------------
inline bool checkFileExist(string path) {
	ofFile f2;
	bool b2 = f2.doesFileExist(path);
	if (b2) {
		ofLogVerbose("ofxSurfing") << "Found file: " << path;
	} else {
		ofLogWarning("ofxSurfing") << "File: " << path << " not found!";
	}
	return b2;
}

// LEGACY
// for ofxSurfingHelpers
inline bool loadGroup(ofParameterGroup & parameters, string path = "") {
	return loadSettings(parameters, path);
}
inline bool saveGroup(ofParameterGroup & parameters, string path = "") {
	return saveSettings(parameters, path);
}
inline bool load(ofParameterGroup & parameters, string path = "") {
	return loadSettings(parameters, path);
}
inline bool save(ofParameterGroup & parameters, string path = "") {
	return saveSettings(parameters, path);
}
}

//------

/*
* SurfingAutoSaver
* 
	A class to auto save an ofParameterGroup.
	settings are saved one second 
	after any change to avoid overflow o file savings.

	EXAMPLE

	//.h
	SurfingAutoSaver autoSaver;

	//setup()
	callback_t f = std::bind(&ofxSurfingPBR::save, this);
	autoSaver.setFunctionSaver(f);
	//autoSaver.setName("ofApp");//for debug
	internalParams.add(autoSaver.bEnable);

	void ofxSurfingPBR::Changed(ofAbstractParameter & e) {
		// ...	
		//if (e.isSerializable())//to exclude saving 
		//for non required parameters like void types.
		autoSaver.saveSoon();
	}

	void load() {
		autoSaver.pause();//stop saving
		// load //
		autoSaver.start();//start
	}
*/

#include <functional>
using callback_t = std::function<void()>;
using callback_tb = std::function<void(bool)>; //TODO

//--------------------------------------------------------------
class SurfingAutoSaver {
public:
	SurfingAutoSaver() {
		ofAddListener(ofEvents().update, this, &SurfingAutoSaver::update);

		bEnable.setSerializable(false); //force always enable
	}

	~SurfingAutoSaver() {
		ofRemoveListener(ofEvents().update, this, &SurfingAutoSaver::update);
	}

	// auto saver workflow
	// we will auto save after every param change,
	// but after waiting some ms. reducing saving overflow.
	// we will save also when app exit.
	ofParameter<bool> bEnable { "Auto Save", true };

	void saveSoon(bool b = true) {
		if (b) {
			// flag to save delayed
			if (bEnable) {
				//only log once/first call
				if (!bFlagSave) ofLogVerbose("SurfingAutoSaver") << "saveSoon() " << name;

				bFlagSave = true;
				timeLastChange = ofGetElapsedTimeMillis();
			}
		}
	}

	const string getName() {
		return name;
	}

	/// <summary>
	/// not mandatory but for debug
	/// </summary>
	/// <param name="n"></param>
	void setName(string n) {
		name = n;
	}

	void pause() {
		// Pause auto save
		// disables auto save
		// to avoid save after loading the settings,
		// as the params will change and would trig the auto save!
		bAutoSave_ = bEnable; //store state
		if (bEnable) {
			bEnable.setWithoutEventNotifications(false);
		}

		bFlagSave = false; //fix just in case
	}

	void start() {
		if (bAutoSave_) {
			bEnable.setWithoutEventNotifications(true); //restore state
		}
	}

private:
	uint64_t timeLastChange = 0;

	const int timeSaveDelay = 500;
	// save delayed x milliseconds.
	// all the params that change will be saved all in one take, during the delay time.

	bool bFlagSave = false;

	bool bAutoSave_ = false;
	string name = "";

private:
	callback_t f_Saver = nullptr;

public:
	void setFunctionSaver(callback_t f = nullptr) {
		f_Saver = f;
	}

private:
	void update(ofEventArgs & args) {
		update();
	}

public:
	/// <summary>
	///  Warning. Should be private! Expected to be called automaticaly as a listener to ofEvents().update() is registered..
	/// </summary>
	void update() {
		if (bEnable) {
			auto t = ofGetElapsedTimeMillis() - timeLastChange;
			if (bFlagSave && t > timeSaveDelay) {
				bFlagSave = false;

				save();
			}
		}
	}

public:
	void save() {
		if (!bEnable) {
			ofLogWarning("SurfingAutoSaver") << "Skipped / Save as disabled " << name;
			return;
		}
		if (f_Saver != nullptr) f_Saver();
		ofLogVerbose("SurfingAutoSaver") << "Save() " << name;
	}
};