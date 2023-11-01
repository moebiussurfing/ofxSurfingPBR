#pragma once
#include "ofMain.h"

namespace ofxSurfing {

// Load ofParameterGroup
//--------------------------------------------------------------
inline bool loadSettings(ofParameterGroup & parameters, string path) {
	ofFile f;
	bool b = f.doesFileExist(path);
	if (b)
		ofLogNotice("ofxSurfing") << "Found file: " << path;
	else
		ofLogError("ofxSurfing") << "File " << path << " not found!";
	if (b) {
		ofJson settings;
		settings = ofLoadJson(path);
		ofDeserialize(settings, parameters);
	}

	return b;
}
// Save ofParameterGroup
//--------------------------------------------------------------
inline bool saveSettings(ofParameterGroup & parameters, string path) {
	if (!ofDirectory::doesDirectoryExist(ofFilePath::getEnclosingDirectory(path))) {
		if (ofFilePath::createEnclosingDirectory(path))
			ofLogWarning("ofxSurfing") << "Created enclosing folder for: " << path;
		else
			ofLogError("ofxSurfing") << "Unable to create enclosing folder for: " << path;
	}
	ofJson settings;
	ofSerialize(settings, parameters);
	bool b = ofSavePrettyJson(path, settings);
	if (b)
		ofLogNotice("ofxSurfing") << "Saved: " << parameters.getName() << " to " << path;
	else
		ofLogError("ofxSurfing") << "Error saving: " << parameters.getName() << " to " << path;

	return b;
}

// Get the OF project file path name and set to the window title.
//--------------------------------------------------------------
inline string getProjectName() {
	of::filesystem::path path = ofToDataPath("", true);
	string s = (path.parent_path().parent_path().filename()).string();
	return s;
}
//--------------------------------------------------------------
inline void setWindowTitleAsProjectName() {
	ofSetWindowTitle(getProjectName());
}

// WARNING!
// For my personal setup!
// I have three FHD (1920x1080) monitors:
// 2nd monitor (central) is settled as the main system display.
// Central and Left monitor are landscape.
// Right monitor is portrait.
// Examples:
//	ofxSurfing::setWindowAtMonitor(); // Center monitor landscape 
//	ofxSurfing::setWindowAtMonitor(-1); // Move to left monitor landscape 
//	ofxSurfing::setWindowAtMonitor(1, true); // Move to right monitor portrait
//	ofxSurfing:setWindowAtMonitor();//main monitor, landscape, 60 fps, vsync= false
//	ofxSurfing::setWindowAtMonitor(-1, true, true); // Left monitor portrait
//	ofxSurfing::setWindowAtMonitor(-1, false, true); // Left monitor landscape
//--------------------------------------------------------------
inline void setWindowAtMonitor(int pos = 0, bool bPortrait = false, int fps = 60, bool vSync = false) {

	ofSetFrameRate(fps);
	ofSetVerticalSync(vSync);

	int x, y, w, h;

	//fix window bar height on WIN_32.
	int hBar = 32;
	int yPos = hBar;
	int hOffset = hBar;

	x = 0;
	y = yPos;

	w = bPortrait ? 1080 : 1920;
	h = bPortrait ? 1920 : 1080;

	h -= hOffset;

	if (pos == -1)
		x = bPortrait ? -1080 : -1920;
	else if (pos == 1)
		x = 1920;

	ofSetWindowPosition(x, y);
	ofSetWindowShape(w, h);
}

//--------------------------------------------------------------
inline void setWindowSquared(int sz = 800) {
	ofSetWindowShape(sz, sz);
}

};
