#include "SurfingBg.h"

//--------------------------------------------------------------
SurfingBg::SurfingBg() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: SurfingBg()";

	ofAddListener(ofEvents().update, this, &SurfingBg::update);
}

//--------------------------------------------------------------
SurfingBg::~SurfingBg() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: ~SurfingBg()";

	ofRemoveListener(ofEvents().update, this, &SurfingBg::update);

	ofRemoveListener(paramsScene.parameterChangedE(), this, &SurfingBg::ChangedScene);
	ofRemoveListener(paramsColors.parameterChangedE(), this, &SurfingBg::ChangedColors);
}

//--------------------------------------------------------------
void SurfingBg::exit() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: exit()";

#ifndef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	save();
#endif
}

//--------------------------------------------------------------
void SurfingBg::save() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: save()";

	ofxSurfing::saveGroup(parameters, pathSettings);
}

//--------------------------------------------------------------
bool SurfingBg::load() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: load()";

	bool b = false;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	b = ofxSurfing::loadGroup(parameters, pathSettings);

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	return b;
}

//--------------------------------------------------------------
void SurfingBg::setup() {

	// Bg Tex
	{
		// Push arb
		bool b = ofGetUsingArbTex();
		{
			if (ofGetUsingArbTex())
				ofDisableArbTex();
		}

		img.load("images/001.jpg");

		// Pop arb
		{
			if (b != ofGetUsingArbTex())
				if (b)
					ofEnableArbTex();
				else
					ofDisableArbTex();
		}
	}

	center = glm::vec3(0, 0, 0);

	//--

	setupParameters();

	setupGui();

	//--

	//TODO: fix
	startup();
}

//--------------------------------------------------------------
void SurfingBg::setupParameters() {

	parameters.setName("SCENE_BG");

	//bInCam.set("InCam", false);
	// Background Scene
	//bAutoSetColor.set("Auto Color", false);
	//Bg_AutoSetColorPick.set("Bg AutoPick", false);

	//--

#ifdef SURFING__PBR__BG_COLORS_NO_ALPHA
	globalColor.set("Global Color",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	globalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

#ifdef SURFING__PBR__BG_COLORS_NO_ALPHA
	colorGroup.set("Group Color", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	diffuse.set("Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	ambient.set("Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	specular.set("Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	emissive.set("Emissive", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
#else
	colorGroup.set("Group Color", ofFloatColor(0, 1), ofFloatColor(0, 0), ofFloatColor(1, 1));
	diffuse.set("Diffuse", ofFloatColor(0, 1), ofFloatColor(0, 0), ofFloatColor(1, 1));
	ambient.set("Ambient", ofFloatColor(0, 1), ofFloatColor(0, 0), ofFloatColor(1, 1));
	specular.set("Specular", ofFloatColor(0, 1), ofFloatColor(0, 0), ofFloatColor(1, 1));
	emissive.set("Emissive", ofFloatColor(0, 1), ofFloatColor(0, 0), ofFloatColor(1, 1));
#endif

	brightGlobal.set("Bright", 0.5f, 0.05f, SURFING__PBR__MAX_BRIGHT_LIMIT); //avoid reset at black..

	shininess.set("Shininess", 120, 0, 120);

	//--

	bGui.set("UI Bg", true);

	bRotate.set("Rotate", true);
	speedRotate.set("Speed", 0, -1, 1);

	bDrawObject.set("Draw Bg Object", true);
	bDrawWireframe.set("Draw Wireframe", false);
	bDrawBox.set("Mode Box", false);
	bDrawSphere.set("Mode Sphere", false);

	sizeScene.set("Size", 0.5, 0, 1.f);

	bDrawTex.set("Use Tex", false);

	bSmoothLights.set("Smooth Lights", false);
	// Default Low Poly

	vResetAll.set("Reset All");
	vResetScene.set("Reset Scene");
	vResetColors.set("Reset Colors");

	//-

	// Bg Scene Object
	// sphere/box with or (colored) without textured

	paramsScene.setName("Scene");

	paramsObject.setName("Object");
	paramsObject.add(bDrawObject);
	paramsObject.add(bDrawBox);
	paramsObject.add(bDrawSphere);
	paramsObject.add(bDrawTex);
	paramsObject.add(bDrawWireframe);
	paramsObject.add(sizeScene);
	paramsScene.add(paramsObject);
	//paramsScene.add(bDrawBgColorPlain);

	paramsExtra.setName("Extra");
	paramsExtra.add(bSmoothLights);
	paramsScene.add(paramsExtra);

	paramsScene.add(bRotate);
	paramsScene.add(speedRotate);

	paramsScene.add(vResetScene);

	//paramsScene.add(bInCam);
	//paramsScene.add(bAutoSetColor);
	//paramsScene.add(Bg_AutoSetColorPick);//disabled bc break brightGlobal..
	//paramsScene.add(bAnimLights);

	paramsColors.setName("Colors");

	paramsColorsGlobal.setName("Global");
	paramsColorsGlobal.add(shininess);
	paramsColorsGlobal.add(globalColor);
	paramsColorsGlobal.add(colorGroup);
	paramsColors.add(paramsColorsGlobal);

	paramsColors.add(brightGlobal);
	paramsColors.add(diffuse);
	paramsColors.add(ambient);
	paramsColors.add(specular);
	paramsColors.add(emissive);
	paramsColors.add(vResetColors);

	// Bg
	bDrawBgColorPlain.set("Draw Bg Plain Color", false);
	bgColorPlain.set("Bg Plain Color", ofFloatColor::darkGrey, ofFloatColor(0.f), ofFloatColor(1.f));
	backgroundColorPlainParams.setName("BG Plain Color");
	backgroundColorPlainParams.add(bDrawBgColorPlain);
	backgroundColorPlainParams.add(bgColorPlain);
	paramsColors.add(backgroundColorPlainParams);

	//parameters.add(bMini_Scene);

	parameters.add(paramsScene);
	parameters.add(paramsColors);
	parameters.add(vResetAll);

	// Callbacks
	ofAddListener(paramsScene.parameterChangedE(), this, &SurfingBg::ChangedScene);
	ofAddListener(paramsColors.parameterChangedE(), this, &SurfingBg::ChangedColors);

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	// auto saver
	callback_t f = std::bind(&SurfingBg::save, this);
	//register the local save function to be called when saving is required.
	autoSaver.setFunctionSaver(f);
#endif
}

//--------------------------------------------------------------
void SurfingBg::startup() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: startup()";

	//--

	// Settings
	bool bNoFileSettingsFound = !load();

	//TODO:
	// Settings file not found!
	// Should trig the callbacks to update engine!
	if (bNoFileSettingsFound) {
		ofLogError("SurfingBg") << " Settings file " << pathSettings << " not found !";
		ofLogError("SurfingBg") << " Initializing...";

		doResetAll();
	}

	bDoneStartup = true;
}

//--------------------------------------------------------------
void SurfingBg::setupGui() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: setupGui()";

	gui.setup(parameters);

	static ofEventListener listenerSave;
	static ofEventListener listenerLoad;
	listenerSave = gui.savePressedE.newListener([this] {
		save();
	});
	listenerLoad = gui.loadPressedE.newListener([this] {
		load();
	});

	refreshGui();
}

//--------------------------------------------------------------
void SurfingBg::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: refreshGui()";

	//gui.getGroup(paramsScene.getName()).minimize();

	gui.getGroup(paramsScene.getName())
		.getGroup(paramsExtra.getName())
		.minimize();

	gui.getGroup(paramsColors.getName()).minimize();

	gui.getGroup(paramsColors.getName())
		.getGroup(backgroundColorPlainParams.getName())
		.minimize();
}

//--------------------------------------------------------------
void SurfingBg::setGuiPosition(glm::vec2 pos) {
	gui.setPosition(pos.x, pos.y);
}

//--------------------------------------------------------------
void SurfingBg::drawGui() {
	if (!bGui) return;

	gui.draw();
}

//--------------------------------------------------------------
void SurfingBg::draw() {

	if (!bAppRunning && ofGetFrameNum() > 60) {
		bAppRunning = true;
	}

	// Alternative Bg
	if (bDrawBgColorPlain) {
		ofClear(bgColorPlain);
	}

	if (bDrawObject) drawScene();
}

//--------------------------------------------------------------
void SurfingBg::update(ofEventArgs & args) {

	if (bFlagSetColorBgGroup) {
		bFlagSetColorBgGroup = false;

		setColorBgGroup(colorGroup.get());

#if 1
		//workflow
		//TODO
		//setBrightToColorGroup(brightGlobal);
		bFlagSetBrightToColorGroup = true;
#endif
	}

	if (bFlagSetBrightToColorGroup) {
		bFlagSetBrightToColorGroup = false;

		setBrightToColorGroup(brightGlobal.get());
	}
}

//--------------------------------------------------------------
void SurfingBg::drawObject(float r) {
	bool bInCam = true;

	ofPushMatrix();
	{
		if (bInCam) { // scene center
			ofTranslate(center.x, center.y, center.z);
		} else { // app window center
			ofTranslate(ofGetWidth() / 2.f, ofGetHeight() / 2.f, 0);
		}

		if (bRotate) {
			float rmul = 2.f;
			float rmag = speedRotate * rmul * RAD_TO_DEG;
			float rot = ofMap(speedRotate, -1, 1, -rmag, rmag, true);
			ofRotateYDeg(ofGetElapsedTimef() * rmag);
		}

		//--

		// sphere
		if (bDrawSphere) {
			if (bDrawSphere) ofDrawSphere(0, 0, 0, r);
		}

		// box
		if (bDrawBox) {
			if (bDrawBox) ofDrawBox(0, 0, 0, 2.f * r);
		}
	}
	ofPopMatrix();
}
//--------------------------------------------------------------
void SurfingBg::drawScene(/*bool bInCam*/) {
#define SURFING_BG_MIN 1.f
#define SURFING_BG_MAX 5.f

	bool bInCam = true;
	//if (bInCam)
	//	r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
	//		SURFING__SCENE_SIZE_UNIT * SURFING_BG_MIN,
	//		SURFING__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
	//else

	float r = SURFING__SCENE_SIZE_UNIT;

	material.begin();
	{

		if (bDrawSphere) {
			r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MIN,
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
		}

		else if (bDrawBox) {
			r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MIN,
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
		}

		//--

		ofPushMatrix();
		ofPushStyle();
		{
			if (bDrawTex) img.getTexture().bind();

			ofSetColor(255, 255, 255, 255);

			drawObject(r);

			if (bDrawTex) img.getTexture().unbind();
		}
		ofPopStyle();
		ofPopMatrix();
	}
	material.end();

	//--

	if (bDrawWireframe) {
		ofPushMatrix();
		ofPushStyle();
		ofNoFill();
		ofSetColor(255, 255, 255, 200);
		ofScale(0.99f);

		drawObject(r);

		ofPopStyle();
		ofPopMatrix();
	}
}

// Bg
//--------------------------------------------------------------
void SurfingBg::doResetAll() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: doResetAll()";

	doResetColors();
	doResetScene();
}

//--------------------------------------------------------------
void SurfingBg::doResetColors() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: doResetColors()";

	if (brightGlobal != 0.15f) brightGlobal = 0.15f;
	setBrightToColorGroup(brightGlobal);

	//if (materials.size() > 0)
	//	if (bAutoSetColor) {
	//		auto c = materials[materials.size() - 1].color.get();
	//		setColor(c);
	//	}

	ofFloatColor c = ofFloatColor(0.5f, 1.f);
	globalColor.set(c);

	//--

	bgColorPlain.set(ofFloatColor(0.2f));
#ifdef SURFING__USE_CUBE_MAP
	if (bDrawBgColorPlain) bDrawBgColorPlain = false;
#else
	bDrawBg = true;
#endif
}

//--------------------------------------------------------------
void SurfingBg::doResetScene() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: doResetScene()";

	ofSetSmoothLighting(bSmoothLights);

	if (bDrawBox) bDrawBox = false;
	if (!bDrawSphere) bDrawSphere = true;

	if (sizeScene != 0.5f) sizeScene = 0.5f;
	if (speedRotate != 0.01) speedRotate = 0.01;

	//if (bRotate) bRotate = false;
	//if (!bAutoSetColor) bAutoSetColor = true;
	//if (bDrawTex) bDrawTex = false;
	//bAnimLights = false;
}

//--------------------------------------------------------------
void SurfingBg::setBgColorPlain(ofFloatColor c) {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: setBgColorPlain(" << c << ")";

	bgColorPlain = c;
}

//--------------------------------------------------------------
void SurfingBg::setColorBgGroup(ofFloatColor c) {

	if (colorGroup.get() != c) colorGroup.setWithoutEventNotifications(c);

		//TODO fix
		// bypass some callbacks
#if 0
	if (colorGroup.get() != colorGroup_) {
		colorGroup_ = colorGroup.get();
	} else {
		ofLogNotice("ofxSurfingPBR")<<" SurfingBg: Skip setColorBgGroup(" << ofToString(c) << ")";
		return;
	}
#endif

	//--

	ofLogNotice("ofxSurfingPBR") << " SurfingBg: setColorBgGroup(" << c << ")";

	diffuse.set(c);
	ambient.set(c);
	specular.set(c);
	emissive.set(c);

	////TODO
	////setBrightToColorGroup(brightGlobal);
	//bFlagSetBrightToColorGroup = true;

	//--

	bgColorPlain.set(c);
}

//--------------------------------------------------------------
void SurfingBg::setBrightToColorGroup(float brg) {
//TODO fix
// bypass some callbacks
#if 0
	if (brightGlobal.get() != brg)
		brightGlobal.set(brg);
	else {
		ofLogNotice("ofxSurfingPBR")<<" SurfingBg: Skip setBrightToColorGroup(" << brg << ")";
		return;
	}
#endif

//TODO fix
// bypass some callbacks
#if 0
	if (brightGlobal.get() != brightGlobal_) {
		brightGlobal_ = brightGlobal.get();
	} else {
		ofLogNotice("ofxSurfingPBR")<<" SurfingBg: Skip setBrightToColorGroup(" << brg << ")";
		return;
	}
#endif

	//--

	ofLogNotice("ofxSurfingPBR") << " SurfingBg: setBrightToColorGroup(" << brg << ")";

	auto cc = colorGroup.get();

	auto cd = diffuse.get();
	auto ca = ambient.get();
	auto cs = specular.get();
	auto ce = emissive.get();

	cc.setBrightness(brightGlobal);

	ca.setBrightness(brightGlobal);
	cd.setBrightness(brightGlobal);
	cs.setBrightness(brightGlobal * 0.75f);
	ce.setBrightness(brightGlobal * 0.75f);

	colorGroup.setWithoutEventNotifications(cc); //fix callbacks/feedback
	//colorGroup.set(cc);

	diffuse.set(cd);
	ambient.set(ca);
	specular.set(cs);
	emissive.set(ce);

	//--

	bgColorPlain.set(cc);
}

//--------------------------------------------------------------
void SurfingBg::ChangedColors(ofAbstractParameter & e) {
	string name = e.getName();

	////TODO fix
	//if (name == color.getName()) {
	//	return;
	//	//if (!bDoneStartup) return;
	//	//if (!bAppRunning) return;
	//}

	//--

	ofLogNotice("ofxSurfingPBR") << " SurfingBg: ChangedColors " << name << " : " << e;

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (0) {
	}

	//--

	else if (name == globalColor.getName()) {
		colorGroup.set(globalColor.get());
	}

	else if (name == colorGroup.getName()) {
		////TODO fix
		//if (!bDoneStartup) return;
		//if (!bAppRunning) return;

		bFlagSetColorBgGroup = true;
	}

	else if (name == brightGlobal.getName()) {
		bFlagSetBrightToColorGroup = true;
	}

//workflow
#if 0
	else if (name == bDrawBgColorPlain.getName()) {
		if (bDrawBgColorPlain) {
			if (bDrawObject) bDrawObject.set(false);
		} else {
			if (!bDrawObject) bDrawObject.set(true);
		}
	}
#endif

	//--

	else if (name == diffuse.getName()) {
		material.setDiffuseColor(diffuse.get());
	} else if (name == ambient.getName()) {
		material.setAmbientColor(ambient.get());
	} else if (name == specular.getName()) {
		material.setSpecularColor(specular.get());
	} else if (name == emissive.getName()) {
		material.setEmissiveColor(emissive.get());
	}

	else if (name == vResetColors.getName()) {
		doResetColors();
	}
}

//--------------------------------------------------------------
void SurfingBg::ChangedScene(ofAbstractParameter & e) {
	string name = e.getName();

	//--

	////TODO fix
	//if (name == color.getName()) {
	//	return;
	//	//if (!bDoneStartup) return;
	//	//if (!bAppRunning) return;
	//}

	//--

	ofLogNotice("ofxSurfingPBR") << " SurfingBg: ChangedScene " << name << " : " << e;

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (0) {
	}

//--

//workflow
#if 0
	else if (name == bDrawObject.getName()) {
		if (bDrawObject) {
			if (bDrawBgColorPlain) bDrawBgColorPlain.set(false);
		} else {
			if (!bDrawBgColorPlain) bDrawBgColorPlain.set(true);
		}
	}
#endif

	//force one of both enabled
	else if (name == bDrawBox.getName()) {
		if (bDrawBox) {
			if (bDrawSphere) bDrawSphere.set(false);
		} else {
			if (!bDrawSphere) bDrawSphere.set(true);
		}
	} else if (name == bDrawSphere.getName()) {
		if (bDrawSphere) {
			if (bDrawBox) bDrawBox.set(false);
		} else {
			if (!bDrawBox) bDrawBox.set(true);
		}
	}

	else if (name == shininess.getName()) {
		material.setShininess(shininess);
	}

	//--

	else if (name == vResetAll.getName()) {
		doResetAll();
	}

	else if (name == vResetScene.getName()) {
		doResetScene();
	}

	//--

	//disabled bc break brightGlobal..
	//else if (name == Bg_AutoSetColorPick.getName())
	//{
	//	if (Bg_AutoSetColorPick) bAutoSetColor = false;
	//	//bAutoSetColor = !Bg_AutoSetColorPick;
	//}
	//else if (name == bAutoSetColor.getName())
	//{
	//	if (bAutoSetColor) Bg_AutoSetColorPick = false;
	//	//Bg_AutoSetColorPick = !bAutoSetColor;
	//}

	//--

	////#ifdef SURFING__USE_CUBE_MAP
	////	if (name == surfingBg.bDrawBgColorPlain.getName()) {
	////		if (!bLoadedCubeMap) return; //skip
	////		//workflow
	////		if (surfingBg.bDrawBgColorPlain)
	////			if (bDrawCubeMap) bDrawCubeMap = false;
	////	}
	////#endif
}