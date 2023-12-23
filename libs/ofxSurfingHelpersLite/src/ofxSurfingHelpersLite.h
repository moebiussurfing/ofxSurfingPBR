#pragma once
#include "ofMain.h"

#include "ofxSurfingOfxGui.h"

//------

// CONSTANTS

#define SURFING__PAD_TO_WINDOW_BORDERS 8

#define SURFING__STRING_BOX__DEFAULT_XPAD 20
#define SURFING__STRING_BOX__DEFAULT_YPAD 20

//#define SURFING__STRING_BOX__DEFAULT_ROUND 5.f
#define SURFING__STRING_BOX__DEFAULT_ROUND 2.f // 0.f to not rounded rectangle

//#define SURFING__STRING_BOX__INCLUDE_EXTRA_LAYOUTS // include mouse pos

//#define SURFING__STRING_BOX___USE_TTF_INSTEAD_OF_BITMAP_FONT___WIP//TODO

//------

namespace ofxSurfing {

/*
* App Window
* 
	App window tools to handle position, settings or name.
*/

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
//	ofxSurfing::setWindowAtMonitor(-1, true); // Left monitor portrait
//	ofxSurfing::setWindowAtMonitor(-1, false); // Left monitor landscape
//	ofxSurfing::setWindowAtMonitor(1, false , 120, true); // Right monitor landscape, 60fps, vSync on

// Examples:
// Move and shape the window app.
// Customizes settings too: 60fps and vSync off.
//ofxSurfing::setWindowAtMonitor(); // Stay at main display (center in my setup) and landscape.
//ofxSurfing::setWindowAtMonitor(-1); // Move to left display and set landscape.
//ofxSurfing::setWindowAtMonitor(1, true); // Move to right display and set portrait.
//--------------------------------------------------------------
inline void setWindowAtMonitor(int pos = 0, bool bPortrait = false, int fps = 60, bool vSync = false) {

	ofSetFrameRate(fps);
	ofSetVerticalSync(vSync);

	// My PERSONAL displays setup
	// pos monitors
	// -1: FHD, landscape
	//  0: FHD, landscape / main display
	//  1: FHD, portrait
	// My main display
	int wMain = 1920;
	int hMain = 1080;

	//fix window bar height on WIN_32.
	int hBar = 32;
	int yPos = hBar; //move a bit down to move it inside the window.
	int hOffset = hBar; //make the window height smaller.

	int x, y, w, h;

	x = 0;
	y = 0;
	y += yPos;

	w = bPortrait ? hMain : wMain;
	h = bPortrait ? wMain : hMain;

	h -= hOffset;

	if (pos == -1)
		x = bPortrait ? -hMain : -wMain;
	else if (pos == 1)
		x = wMain;

	ofSetWindowPosition(x, y);
	ofSetWindowShape(w, h);
}

//--------------------------------------------------------------
inline void setWindowMaxFrameRate() {
	ofSetFrameRate(0);
	ofSetVerticalSync(false);
}

//--------------------------------------------------------------
inline string setWindowShapeSquared(int sz = 800) {
	ofSetWindowShape(sz, sz);

	string s = "Squared " + ofToString(sz) + "x" + ofToString(sz) + " (1:1)";
	return s;
}

//--------------------------------------------------------------
inline string setWindowShapeForInstagram(size_t i = 0, bool bForcePos = false) {
	/*
	* Instagram Social media 
	* Image type sizes
	* Dimensions in pixels
	*/

	vector<string> names = {
		"1080x1920 9:16\nIG Story",
		"1080x1080 1:1\nIG Square",
		"1080x1350 4:5\nIG Portrait",
		"1080x566 1.91:1\nIG Landscape",
		"420x654 1:1.55\nIGTV Cover",
	};

	i = ofClamp(i, 0, names.size() - 1);

	//if (bForcePos) ofSetWindowPosition(0, 0);

	if (i == 0) ofSetWindowShape(1080, 1920); // IG Story
	if (i == 1) ofSetWindowShape(1080, 1080); // IG Square
	if (i == 2) ofSetWindowShape(1080, 1350); // IG Portrait
	if (i == 3) ofSetWindowShape(1080, 566); // IG Landscape
	if (i == 4) ofSetWindowShape(420, 654); // IGTV Cover

	if (bForcePos) ofSetWindowPosition(0, 0); //fix force

	return names[i];

	//// TODO:
	//// allow resize on runtime
	//// set instagram size
	//case OF_KEY_F6: {
	//	int w, h;
	//	w = 864;
	//	h = 1080 + 19;
	//	ofSetWindowShape(w, h);
	//	windowResized(w, h);
	//	cap_w = w;
	//	cap_h = h;
	//	buildAllocateFbo();
	//} break;
}

//------

/*
*  ofDrawBitmapStringBox
* 
	A text box widget with 12 layout positions management.
*/

//--------------------------------------------------------------
inline void ofDrawBitmapStringBox(string s, int x, int y, float round = SURFING__STRING_BOX__DEFAULT_ROUND) {
	bool bdebug = 0;

#ifdef SURFING__STRING_BOX___USE_TTF_INSTEAD_OF_BITMAP_FONT___WIP
	static bool bDone = 0;
	static ofTrueTypeFont font;
	if (!bDone) {
		bDone = 1;
		string path = SURFING__OFXGUI__FONT_DEFAULT_PATH;
		int sz = SURFING__OFXGUI__FONT_DEFAULT_SIZE;
		font.loadFont(path, sz);
	}
#endif

	int a1 = 255;
	int a2 = 200;
	ofColor c1 = ofColor(ofColor::white, a1);
	ofColor c2 = ofColor(ofColor::black, a2);

	int xpad = SURFING__STRING_BOX__DEFAULT_XPAD;
	int ypad = SURFING__STRING_BOX__DEFAULT_YPAD;

	x += xpad;
	y += ypad;

	ofBitmapFont bf;
	auto bb = bf.getBoundingBox(s, 0, 0);

	//fix
	int xoffset = 0;
	int yoffset = 0;

	//xoffset = 0; //TODO
	yoffset = 11; //TODO

	ofRectangle bb1;
	bb1 = { (float)x, (float)y, bb.width, bb.height };

	ofRectangle bb2;
	bb2 = { (float)x - xpad, (float)y - ypad, bb1.width + 2 * xpad, bb1.height + 2 * ypad };

	{
		ofPushStyle();
		ofFill();
		ofSetColor(c2);

		if (round == 0)
			ofDrawRectangle(bb2);
		else
			ofDrawRectRounded(bb2, round);

		ofPopStyle();
	}

#if (bdebug)
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(2);
	ofSetColor(ofColor::yellow);

	ofDrawRectangle(bb1);

	ofSetColor(ofColor::red);
	ofDrawRectangle(bb2);

	ofPopStyle();
#endif

	ofPushStyle();
	ofSetColor(c1);

#ifdef SURFING__STRING_BOX___USE_TTF_INSTEAD_OF_BITMAP_FONT___WIP
	if (font.isLoaded())
		font.drawString(s, x + xoffset, y + yoffset);
	else
		ofDrawBitmapString(s, x + xoffset, y + yoffset);
#else
	ofDrawBitmapString(s, x + xoffset, y + yoffset);
#endif

	ofPopStyle();
}

//--------------------------------------------------------------
inline ofRectangle getBBBitmapStringBox(string s, int x = 0, int y = 0) {
	int xpad = SURFING__STRING_BOX__DEFAULT_XPAD;
	int ypad = SURFING__STRING_BOX__DEFAULT_YPAD;

	x += xpad;
	y += ypad;

	ofRectangle bb;

#ifdef SURFING__STRING_BOX___USE_TTF_INSTEAD_OF_BITMAP_FONT___WIP
	static bool bDone = 0;
	static ofTrueTypeFont font;
	if (!bDone) {
		bDone = 1;
		string path = SURFING__OFXGUI__FONT_DEFAULT_PATH;
		int sz = SURFING__OFXGUI__FONT_DEFAULT_SIZE;
		font.loadFont(path, sz);
	}
	if (font.isLoaded())
		bb = font.getStringBoundingBox(s, 0, 0);
	else {
		ofBitmapFont bf;
		bb = bf.getBoundingBox(s, 0, 0);
	}
#else
	ofBitmapFont bf;
	bb = bf.getBoundingBox(s, 0, 0);
#endif

	ofRectangle bb1;
	bb1 = { (float)x, (float)y, bb.width, bb.height };

	ofRectangle bb2;
	bb2 = { (float)x - xpad, (float)y - ypad, bb1.width + 2 * xpad, bb1.height + 2 * ypad };

	return bb2;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToCenterLeft(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = xpad;
	int y = ofGetHeight() / 2 - bb.getHeight() / 2;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToCenter(string s) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = ofGetWidth() / 2 - bb.getWidth() / 2;
	int y = ofGetHeight() / 2 - bb.getHeight() / 2;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToCenterRight(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = ofGetWidth() - bb.getWidth() - xpad;
	int y = ofGetHeight() / 2 - bb.getHeight() / 2;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToTopCenter(string s, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = ofGetWidth() / 2 - bb.getWidth() / 2;
	int y = ypad;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToTopLeft(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = xpad;
	int y = ypad;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToTopRight(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = ofGetWidth() - bb.getWidth() - xpad;
	int y = ypad;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToBottomLeft(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = xpad;
	int y = ofGetHeight() - bb.getHeight() - ypad;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToBottomCenter(string s, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = ofGetWidth() / 2 - bb.getWidth() / 2;
	int y = ofGetHeight() - bb.getHeight() - ypad;
	glm::vec2 p { x, y };
	return p;
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToBottomRight(string s, int xpad = SURFING__PAD_TO_WINDOW_BORDERS, int ypad = SURFING__PAD_TO_WINDOW_BORDERS) {
	auto bb = getBBBitmapStringBox(s, 0, 0);
	int x = ofGetWidth() - bb.getWidth() - xpad;
	int y = ofGetHeight() - bb.getHeight() - ypad;
	glm::vec2 p { x, y };
	return p;
}

/*
enum SURFING_LAYOUT {
	SURFING_LAYOUT_TOP_LEFT = 0,
	SURFING_LAYOUT_TOP_CENTER,
	SURFING_LAYOUT_TOP_RIGHT,

	SURFING_LAYOUT_CENTER_LEFT,
	SURFING_LAYOUT_CENTER,
	SURFING_LAYOUT_CENTER_RIGHT,

	SURFING_LAYOUT_BOTTOM_LEFT,
	SURFING_LAYOUT_BOTTOM_CENTER,
	SURFING_LAYOUT_BOTTOM_RIGHT,

//--

#ifdef SURFING__STRING_BOX__INCLUDE_EXTRA_LAYOUTS
	SURFING_LAYOUT_MOUSE_POS,
	SURFING_LAYOUT_MOUSE_POS_CENTER,
#endif

	SURFING_LAYOUT_AMOUNT
};
*/

//--------------------------------------------------------------
inline string getLayoutName(SURFING_LAYOUT layout) {
	string s;
	switch (layout) {

	case SURFING_LAYOUT_TOP_LEFT:
		s = "TOP LEFT";
		break;
	case SURFING_LAYOUT_TOP_CENTER:
		s = "TOP CENTER";
		break;
	case SURFING_LAYOUT_TOP_RIGHT:
		s = "TOP RIGHT";
		break;

	case SURFING_LAYOUT_CENTER_LEFT:
		s = "CENTER LEFT";
		break;
	case SURFING_LAYOUT_CENTER:
		s = "CENTER";
		break;
	case SURFING_LAYOUT_CENTER_RIGHT:
		s = "CENTER RIGHT";
		break;

	case SURFING_LAYOUT_BOTTOM_LEFT:
		s = "BOTTOM LEFT";
		break;
	case SURFING_LAYOUT_BOTTOM_CENTER:
		s = "BOTTOM CENTER";
		break;
	case SURFING_LAYOUT_BOTTOM_RIGHT:
		s = "BOTTOM RIGHT";
		break;

		//--

#ifdef SURFING__STRING_BOX__INCLUDE_EXTRA_LAYOUTS
	case SURFING_LAYOUT_MOUSE_POS:
		s = "MOUSE POS";
		break;
	case SURFING_LAYOUT_MOUSE_POS_CENTER:
		s = "MOUSE POS C";
		break;
#endif

	default:
		s = "NONE";
		break;
	}
	return s;
}
//--------------------------------------------------------------
inline string getLayoutName(int layout) {
	return getLayoutName((SURFING_LAYOUT)layout);
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToLayout(string s, SURFING_LAYOUT layout) {
	glm::vec2 p { 0, 0 };

	if (layout == SURFING_LAYOUT_TOP_LEFT) {
		// top-left
		p = getBitmapStringBoxPosToTopLeft(s);
	} else if (layout == SURFING_LAYOUT_TOP_CENTER) {
		// top-center
		p = getBitmapStringBoxPosToTopCenter(s);
	} else if (layout == SURFING_LAYOUT_TOP_RIGHT) {
		// top-right
		p = getBitmapStringBoxPosToTopRight(s);
	}

	else if (layout == SURFING_LAYOUT_CENTER_LEFT) {
		// center-left
		p = getBitmapStringBoxPosToCenterLeft(s);
	} else if (layout == SURFING_LAYOUT_CENTER) {
		// center-center
		p = getBitmapStringBoxPosToCenter(s);
	} else if (layout == SURFING_LAYOUT_CENTER_RIGHT) {
		// center-right
		p = getBitmapStringBoxPosToCenterRight(s);
	}

	else if (layout == SURFING_LAYOUT_BOTTOM_LEFT) {
		// bottom-left
		p = getBitmapStringBoxPosToBottomLeft(s);
	} else if (layout == SURFING_LAYOUT_BOTTOM_CENTER) {
		// bottom-center
		p = getBitmapStringBoxPosToBottomCenter(s);
	} else if (layout == SURFING_LAYOUT_BOTTOM_RIGHT) {
		// bottom-right
		p = getBitmapStringBoxPosToBottomRight(s);
	}

	//--

#ifdef SURFING__STRING_BOX__INCLUDE_EXTRA_LAYOUTS
	else if (layout == SURFING_LAYOUT_MOUSE_POS) {
		// mouse pos clamped inside the window
		auto bb = getBBBitmapStringBox(s);
		int xm = ofGetMouseX();
		int ym = ofGetMouseY();
		int x = ofClamp(xm, 0, ofGetWidth() - bb.width);
		int y = ofClamp(ym, 0, ofGetHeight() - bb.height);
		p = { x, y };
	} else if (layout == SURFING_LAYOUT_MOUSE_POS_CENTER) {
		// mouse pos centered clamped inside the window
		auto bb = getBBBitmapStringBox(s);
		int xm = ofGetMouseX() - bb.width / 2;
		int ym = ofGetMouseY() - bb.height / 2;
		int x = ofClamp(xm, 0, ofGetWidth() - bb.width);
		int y = ofClamp(ym, 0, ofGetHeight() - bb.height);
		p = { x, y };
	}
#endif

	return p;
}

//--------------------------------------------------------------
inline ofRectangle getBBBitmapStringBoxToLayout(string s, SURFING_LAYOUT layout) {
	glm::vec2 p = getBitmapStringBoxPosToLayout(s, layout);
	return getBBBitmapStringBox(s, p.x, p.y);
}
//--------------------------------------------------------------
inline ofRectangle getBBBitmapStringBoxToLayout(string s, int layout) {
	return getBBBitmapStringBoxToLayout(s, (SURFING_LAYOUT)layout);
}

//--------------------------------------------------------------
inline glm::vec2 getBitmapStringBoxPosToLayout(string s, int layout) {
	return getBitmapStringBoxPosToLayout(s, (SURFING_LAYOUT)layout);
}

//--------------------------------------------------------------
inline void ofDrawBitmapStringBox(string s, SURFING_LAYOUT layout = SURFING_LAYOUT_TOP_LEFT) {
	glm::vec2 p = ofxSurfing::getBitmapStringBoxPosToLayout(s, layout);
	ofxSurfing::ofDrawBitmapStringBox(s, p.x, p.y);
}
//--------------------------------------------------------------
inline void ofDrawBitmapStringBox(string s, int layout /* = 0*/) {
	ofDrawBitmapStringBox(s, (SURFING_LAYOUT)layout);
}

//--------------------------------------------------------------
inline void ofDrawBitmapStringBox(string s, ofxPanel * gui, SURFING_LAYOUT layout = SURFING_LAYOUT_TOP_LEFT) {
	// Set text box position linked to the ofxPanel
	// but setting the layout position.

	glm::vec2 p { 0, 0 };

	auto rgui = gui->getShape();
	auto rbb = getBBBitmapStringBox(s);
	int pad = SURFING__OFXGUI__PAD_BETWEEN_PANELS;

	if (layout == SURFING_LAYOUT_TOP_LEFT) {
		p = rgui.getTopLeft();
		p += glm::vec2(-rbb.getWidth() - pad, 0);
	} else if (layout == SURFING_LAYOUT_TOP_RIGHT) {
		p = rgui.getTopRight();
		p += glm::vec2(pad, 0);
	} else if (layout == SURFING_LAYOUT_TOP_CENTER) {
		p = rgui.getTopLeft();
		p += glm::vec2(pad, -rbb.getHeight() - pad);
	} else if (layout == SURFING_LAYOUT_BOTTOM_CENTER) {
		p = rgui.getBottomLeft();
		p += glm::vec2(pad, pad);
	} else {
	}

	ofDrawBitmapStringBox(s, p.x, p.y);
}

/*
	EXAMPLES

	// simple
	{
		ofxSurfing::ofDrawBitmapStringBox(s, layout);//centered
	}
	
	// debug fps
	{
		float fps = ofGetFrameRate();
		string s = ofToString(fps, 1);
		s += " FPS";
		ofxSurfing::ofDrawBitmapStringBox(s, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
	}

	// browse layouts
	{
		keyPressed(int key) 
		if (key == 'l') {
			helpLayout++;
			helpLayout = helpLayout % ((int)ofxSurfing::SURFING_LAYOUT_AMOUNT + 1);
			buildHelp();
		}
	}
*/
}

//------

namespace ofxSurfing {

//---
/*
* String helpers
*/

// original code copied from: ofxFilikaUtils.h
#define SECS_PER_MIN 60
#define SECS_PER_HOUR 3600
//--------------------------------------------------------------
inline std::string calculateTime(float _time) {

	int seconds;
	int minutes;
	int mins_left;
	int secs_left;

	seconds = (/*gameTimeSeconds - */ int(_time));
	minutes = (/*gameTimeSeconds - */ int(_time)) / SECS_PER_MIN;
	mins_left = minutes % SECS_PER_MIN;
	secs_left = seconds % SECS_PER_MIN;

	std::string mins;
	std::string secs;

	if (mins_left < 10) {
		mins = "0" + ofToString(mins_left);
	} else {
		mins = ofToString(mins_left);
	}

	if (secs_left < 10) {
		secs = "0" + ofToString(secs_left);
	} else {
		secs = ofToString(secs_left);
	}

	//cout << ofGetElapsedTimeMillis() / 1000 << endl;
	//cout << "remaining time : " << mins_left << " : " <<  secs_left << endl;
	//cout << "remaining time : " << mins << " : " <<  secs << endl;

	if (mins_left < 0 || secs_left < 0)
		return "00:00";
	else
		return (mins + ":" + secs);
}

//------

/*
* Push/Pop ArbTex state
*/

//--------------------------------------------------------------
inline bool PushSetArbTex(bool bEnable) {
	bool b = ofGetUsingArbTex(); //return pre state

	if (bEnable) {
		if (!ofGetUsingArbTex())
			ofEnableArbTex();
	} else {
		if (ofGetUsingArbTex())
			ofDisableArbTex();
	}

	return b;
}

//--------------------------------------------------------------
inline void PopSetArbTex(bool bPrePush) {
	if (bPrePush != ofGetUsingArbTex()) {
		if (bPrePush)
			ofEnableArbTex();
		else
			ofDisableArbTex();
	}
}

}