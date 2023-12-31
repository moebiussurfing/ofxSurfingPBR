#pragma once
#include "ofMain.h"

#include "ofxSurfingSerializers.h"
#include "ofxGui.h"

//------

// CONSTANTS

#define SURFING__OFXGUI__PAD_BETWEEN_PANELS 4
#define SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS 8

//--

// ofxGui

// Default font and sizes/colors will be used to customize ofxGui!
#define SURFING__OFXGUI__FONT_DEFAULT_SIZE 9
#define SURFING__OFXGUI__FONT_DEFAULT_SIZE_MINI 7
#define SURFING__OFXGUI__FONT_DEFAULT_PATH "assets/fonts/NotoSansMono-Regular.ttf"

//#define SURFING__OFXGUI__FONT_DEFAULT_SIZE 10
//#define SURFING__OFXGUI__FONT_DEFAULT_PATH "assets/fonts/Montserrat-Regular.ttf"

//#define SURFING__OFXGUI__FONT_DEFAULT_PATH "assets/fonts/Inter-Regular.ttf"
//#define SURFING__OFXGUI__FONT_DEFAULT_PATH "assets/fonts/JetBrainsMonoNL-ExtraBold.ttf"
//#define SURFING__OFXGUI__FONT_DEFAULT_PATH "assets/fonts/JetBrainsMonoNL-SemiBold.ttf"
//#define SURFING__OFXGUI__FONT_DEFAULT_PATH "assets/fonts/JetBrainsMono-Bold.ttf"

namespace ofxSurfing {


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

//------

/*
* ofxGui
* 
	ofxGui Helpers
*/

/*
	SURFING_LAYOUT_TOP_LEFT = 0,
	SURFING_LAYOUT_TOP_CENTER,
	SURFING_LAYOUT_TOP_RIGHT,

	SURFING_LAYOUT_CENTER_LEFT,
	SURFING_LAYOUT_CENTER,
	SURFING_LAYOUT_CENTER_RIGHT,

	SURFING_LAYOUT_BOTTOM_LEFT,
	SURFING_LAYOUT_BOTTOM_CENTER,
	SURFING_LAYOUT_BOTTOM_RIGHT,
*/

//--------------------------------------------------------------
inline void setGuiPositionToLayout(ofxPanel & gui, int layout /* = 0*/) {

	// Move gui panel window to:
	glm::vec2 p;
	int pad = SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS; // to borders

	// top-left
	if (layout == SURFING_LAYOUT_TOP_LEFT) {
		int x = pad;
		int y = pad;
		p = glm::vec2(x, y);
	}
	// top-center
	else if (layout == SURFING_LAYOUT_TOP_CENTER) {
		int x = ofGetWindowWidth() / 2 - gui.getShape().getWidth() / 2;
		int y = pad;
		p = glm::vec2(x, y);
	}
	// top-right
	else if (layout == SURFING_LAYOUT_TOP_RIGHT) {
		int x = ofGetWindowWidth() - gui.getShape().getWidth() - pad;
		int y = pad;
		p = glm::vec2(x, y);
	}

	// center-left
	else if (layout == SURFING_LAYOUT_CENTER_LEFT) {
		int x = pad;
		int y = ofGetWindowHeight() / 2 - gui.getShape().getHeight() / 2;
		p = glm::vec2(x, y);
	}
	// center-center
	else if (layout == SURFING_LAYOUT_CENTER) {
		int x = ofGetWindowWidth() / 2 - gui.getShape().getWidth() / 2;
		int y = ofGetWindowHeight() / 2 - gui.getShape().getHeight() / 2;
		p = glm::vec2(x, y);
	}
	// center-right
	else if (layout == SURFING_LAYOUT_CENTER_RIGHT) {
		int x = ofGetWindowWidth() - gui.getShape().getWidth() - pad;
		int y = ofGetWindowHeight() / 2 - gui.getShape().getHeight() / 2;
		p = glm::vec2(x, y);
	}

	// bottom-left
	else if (layout == SURFING_LAYOUT_BOTTOM_LEFT) {
		int x = pad;
		int y = ofGetWindowHeight() - gui.getShape().getHeight() - pad;
		p = glm::vec2(x, y);
	}
	// bottom-center
	else if (layout == SURFING_LAYOUT_BOTTOM_CENTER) {
		int x = ofGetWindowWidth() / 2 - gui.getShape().getWidth() / 2;
		int y = ofGetWindowHeight() - gui.getShape().getHeight() - pad;
		p = glm::vec2(x, y);
	}
	// bottom-right
	else if (layout == SURFING_LAYOUT_BOTTOM_RIGHT) {
		int x = ofGetWindowWidth() - gui.getShape().getWidth() - pad;
		int y = ofGetWindowHeight() - gui.getShape().getHeight() - pad;
		p = glm::vec2(x, y);
	}

	else {
		int x = pad;
		int y = pad;
		p = glm::vec2(x, y);
	}

	gui.setPosition(p.x, p.y);
}
//--------------------------------------------------------------
inline void setGuiPositionToLayout(ofxPanel & gui, SURFING_LAYOUT layout) {
	setGuiPositionToLayout(gui, (int)layout);
}

// Set position of gui1 at the window bottom and centered
// (gui2 must be externally linked to gui1 with the correct padding).
//TODO: other layouts
//--------------------------------------------------------------
inline void setGuiPositionToLayoutBoth(ofxPanel & gui1, ofxPanel & gui2, SURFING_LAYOUT layout = SURFING_LAYOUT_BOTTOM_CENTER, bool bDoubleSpace = false) {
	//TODO: ADD OTHER LAYOUTS

	float d = float(SURFING__OFXGUI__PAD_BETWEEN_PANELS);
	if (bDoubleSpace) d *= 2;

	if (layout == SURFING_LAYOUT_BOTTOM_CENTER) { // both bottom-center
		int gw = gui1.getShape().getWidth() + gui2.getShape().getWidth() + d;
		int gh = MAX(gui1.getShape().getHeight(), gui2.getShape().getHeight());
		gh += SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS;
		int x = ofGetWidth() / 2 - gw / 2;
		int y = ofGetHeight() - gh;
		y = MAX(y, 0);
		gui1.setPosition(x, y);
	}

	else if (layout == SURFING_LAYOUT_TOP_CENTER) { // both top-center
		int gw = gui1.getShape().getWidth() + gui2.getShape().getWidth() + d;
		int x = ofGetWidth() / 2 - gw / 2;
		int y = SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS;
		y = MAX(y, 0);
		gui1.setPosition(x, y);
	}

	else {
		ofLogWarning("ofxSurfing") << "Layout not implemented for setGuiPositionToLayoutBoth(" + ofToString(layout) + ")";
	}
}

//--------------------------------------------------------------
inline void setGuiPositionToLayoutPanelsCentered(ofxPanel & gui1, size_t amount, SURFING_LAYOUT layout, bool bDoubleSpace = false) {
	//TODO: add other layouts?
	// to be used for a landscape layout
	// each panel right to the previous.

	//SURFING_LAYOUT_BOTTOM_CENTER
	//SURFING_LAYOUT_TOP_CENTER

	float d = float(SURFING__OFXGUI__PAD_BETWEEN_PANELS);
	if (bDoubleSpace) d *= 2;

	float w = gui1.getShape().getWidth() + d;
	float h = gui1.getShape().getHeight();

	float x;
	float y;

	if (layout == SURFING_LAYOUT_BOTTOM_CENTER) { // bottom-center
		x = ofGetWidth() / 2 - (w * amount) / 2;
		y = ofGetHeight() - (h + SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS);
	}

	else if (layout == SURFING_LAYOUT_TOP_CENTER) { // top-center
		x = ofGetWidth() / 2 - (w * amount) / 2;
		y = SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS;
	}

	else {
		ofLogWarning("ofxSurfing") << "Layout not implemented for setGuiPositionToLayoutPanelsCentered(" + ofToString(layout) + ")";

		x = SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS;
		y = SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS;
	}

	gui1.setPosition(x, y);
}

//--------------------------------------------------------------
inline void setGuiPositionRightTo(ofxPanel & guiTarget, ofxPanel & guiAnchor, bool bDoubleSpace = false) {
	auto bb = guiAnchor.getShape();
	float d = float(SURFING__OFXGUI__PAD_BETWEEN_PANELS);
	if (bDoubleSpace) d *= 2;
	auto p = bb.getTopRight() + glm::vec2 { d, 0 };
	guiTarget.setPosition(p);
}
//--------------------------------------------------------------
inline void setGuiPositionBelowTo(ofxPanel & guiTarget, ofxPanel & guiAnchor, bool bDoubleSpace = false) {
	auto bb = guiAnchor.getShape();
	float d = float(SURFING__OFXGUI__PAD_BETWEEN_PANELS);
	if (bDoubleSpace) d *= 2;
	auto p = bb.getBottomLeft() + glm::vec2 { 0, d };
	guiTarget.setPosition(p);
}

//--------------------------------------------------------------
inline void setOfxGuiTheme(bool bMini = 0, std::string pathFont = "") {

	bool bDebugDefault = 0;

	bool bColors = 1;
	bool bSizes = 1;

	bool bFont = 1;
	int size = bMini ? SURFING__OFXGUI__FONT_DEFAULT_SIZE_MINI : SURFING__OFXGUI__FONT_DEFAULT_SIZE;

	if (pathFont == "") pathFont = SURFING__OFXGUI__FONT_DEFAULT_PATH;

	if (bFont) {
		ofFile file(pathFont);
		bool b = file.exists();
		if (b) {
			ofxGuiSetFont(pathFont, size);
		} else {
			ofLogError("ofxSurfing") << "Font file " + pathFont + " not found!";
			ofLogError("ofxSurfing") << "Unable to customize the ofxGui theme font.";
		}
	}

	if (bColors) {
		ofColor cHeadBg;
		ofColor cBg;
		ofColor cBorder;
		ofColor cText;
		ofColor cFill;

#if bDebugDefault
		//Default
		/*
		ofxBaseGui::headerBackgroundColor(80),
		ofxBaseGui::backgroundColor(0),
		ofxBaseGui::borderColor(120, 100),
		ofxBaseGui::textColor(255),
		ofxBaseGui::fillColor(128);
		*/
		cHeadBg = ofColor(80);
		cBg = ofColor(0);
		cBorder = ofColor(120, 100);
		cText = ofColor(255);
		cFill = ofColor(128);
#else
	//TODO
	#if 0
		//Default
		cHeadBg = ofColor(80);
		cBg = ofColor(0);
		cBorder = ofColor(120, 100);
		cText = ofColor(255);
		cFill = ofColor(128);
	#else
		cHeadBg = ofColor(70);
		//cHeadBg = ofColor(80);
		cBg = ofColor(50, 210);
		cBorder = ofColor(120, 100);
		cText = ofColor(255);
		cFill = ofColor(100, 210);
			//cFill = ofColor(120, 210);
	#endif
#endif
		ofxGuiSetHeaderColor(cHeadBg);
		ofxGuiSetBackgroundColor(cBg);
		ofxGuiSetBorderColor(cBorder);
		ofxGuiSetFillColor(cFill);
		ofxGuiSetTextColor(cText);
	}

	if (bSizes) {
		int textPadding;
		int defaultWidth;
		int defaultHeight;

#if bDebugDefault
		// Default
		/*
		int ofxBaseGui::textPadding = 4;
		int ofxBaseGui::defaultWidth = 200;
		int ofxBaseGui::defaultHeight = 18;
		*/
		textPadding = 4;
		defaultWidth = 200;
		defaultHeight = 18;
#else
		if (bMini) {
			textPadding = 6;
			defaultWidth = 150;
			defaultHeight = 17;
		} else {
			textPadding = 6;
			defaultWidth = 200;
			defaultHeight = 18;
		}
#endif
		ofxGuiSetTextPadding(textPadding);
		ofxGuiSetDefaultWidth(defaultWidth);
		ofxGuiSetDefaultHeight(defaultHeight);
	}
}
};

//--

// This class store the ofxGui panel position.
// queue ofxPanels.
// set anchor panel
// set position for linked.
//
// TODO:
// make derived from ofxPanel?
// add bGui

/*

EXAMPLE

ofxPanel gui;
SurfingOfxGuiPanelsManager guiManager;
	
void setup() {
	parameters.add(guiManager.bAutoLayout);
	parameters.add(bHelp);
	
	gui.setup(parameters);
	refreshGui();//apply folder minimizes here
	guiManager.setup(&gui);
}

void refreshGui() {
	if (!guiManager.bAutoLayout) return;
}

*/
//--------------------------------------------------------------
class SurfingOfxGuiPanelsManager {
#define SURFING_USE_GUI_EXTRA 0

#if (SURFING_USE_GUI_EXTRA)
public:
	ofxPanel gui; //visible toggles
	ofParameter<bool> bGui;
#endif

private:
	ofxPanel * guiAnchor; //act as anchor
	vector<ofxPanel *> guis;
	vector<ofParameter<bool>> bGuis;

	ofParameterGroup parameters;


	ofParameter<glm::vec2> position { "Position", 
		glm::vec2(SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS, SURFING__OFXGUI__PAD_TO_WINDOW_BORDERS),
		glm::vec2(0, 0), 
		glm::vec2(3840, 2160) }; //4k

public:
	ofParameter<bool> bAutoLayout { "AutoLayout", true };

private:
	enum SURFING_LAYOUT_ALIGN {
		SURFING_LAYOUT_ALIGN_HORIZONTAL = 0,
		SURFING_LAYOUT_ALIGN_VERTICAL
	};

#if (SURFING_USE_GUI_EXTRA)
	ofParameterGroup bGuiParams;
	ofParameterGroup & getParametersGui() {
		return bGuiParams;
	}
	bool bUseGuiVisibles = false;
#endif

	string name;
	string path;
	bool bDoneStartup = false;

public:
	void setup(ofxPanel * g) {
		if (g == nullptr) return;

		guiAnchor = g;
		name = guiAnchor->getName();

		parameters.setName(name);
		parameters.add(position);

		//add(g); //auto add
	}

#if (SURFING_USE_GUI_EXTRA)
	void setUseGuiVisibles(bool b) {
		bUseGuiVisibles = b;
	}
#endif

	void add(ofxPanel * g, ofParameter<bool> & b) {
		if (g == nullptr) return;

		bGuis.emplace_back(b);

		guis.push_back(g);

#if (SURFING_USE_GUI_EXTRA)
		bGuiParams.add(bGuiParams);
#endif
	}

	void add(ofxPanel * g) {
		if (g == nullptr) return;

		ofParameter<bool> b;
		b.set(g->getName(), true);

		add(g, b);
	}

	void startup() {
#if (SURFING_USE_GUI_EXTRA)
		string n = name;
		//string n = "UI PANELS " + name;

		bGui.set(n, true);

		bGuiParams.setName(n);
		bGuiParams.add(bAutoLayout);

		parameters.add(bGuiParams);

		if (bUseGuiVisibles)
			gui.setup(bGuiParams);
#endif

		//--

		path = name + "_ofxGui.json";
		ofxSurfing::load(parameters, path);

		guiAnchor->setPosition(position.get().x, position.get().y);

		bDoneStartup = true;
	}

	void refreshGui(int layout = -1) {
		if (guiAnchor == nullptr) return;
		if (bGuis.size() == 0) return;

		if (bAutoLayout) {
			ofxSurfing::SURFING_LAYOUT l;

			if (layout == -1) layout = 0; //default

			//TODO
			if (layout == 0) {
				l = ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER;
			} else {
				l = ofxSurfing::SURFING_LAYOUT_TOP_CENTER;
			}

			ofxSurfing::setGuiPositionToLayout(*guiAnchor, l);
			//ofxSurfing::setGuiPositionToLayoutBoth(gui, modelsManager.getGui(), l);
		} else {
			//return;
		}
	}

private:
	void update() {
		if (!bDoneStartup) {
			startup();
		} 

		int i_= 0;// last visible
		for (size_t i = 1; i < guis.size(); i++) {
			if (!bGuis[i]) continue;
			ofxSurfing::setGuiPositionRightTo(*guis[i], *guis[i_]);
			i_ = i;
		}
	}

public:
	void draw() {
		if (guiAnchor == nullptr) return;
		if (bGuis.size() == 0) return;

		update();

		for (size_t i = 0; i < guis.size(); i++) {
			if (!bGuis[i]) continue;
			if (bGuis[i]) guis[i]->draw();
		}
#if (SURFING_USE_GUI_EXTRA)
		if (bGui && bUseGuiVisibles) gui.draw();
#endif
	}

private:
	void exit() {
		if (guiAnchor == nullptr) return;
		position = guiAnchor->getPosition();

		ofxSurfing::save(parameters, path);
	}

public:
	SurfingOfxGuiPanelsManager() {};

	~SurfingOfxGuiPanelsManager() { exit(); };

	/*
	* NOTES / SNIPPETS
		void drawGui() {
			gui.draw();

			if (lights.bGui) {
				ofxSurfing::setGuiPositionRightTo(lights.gui, gui);
				lights.drawGui();
			}

			if (bGui_Materials) {
				ofxSurfing::setGuiPositionRightTo(guiMaterials, lights.bGui ? lights.gui : gui);
				guiMaterials.draw();
			}

			if (bGui_Colors) {
				ofxSurfing::setGuiPositionRightTo(guiColors, guiMaterials);
				guiColors.draw();
			}
		}

		// Responsive layout
		//auto l = pbr.getLayoutHelp();
		//if (l == ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT || l == ofxSurfing::SURFING_LAYOUT_CENTER_LEFT)
		//	ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
		//else
		//	ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT);

	*/
};
