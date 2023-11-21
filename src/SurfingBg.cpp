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
	ofRemoveListener(paramsColorizers.parameterChangedE(), this, &SurfingBg::ChangedColors);
	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingBg::Changed);
}

//--------------------------------------------------------------
void SurfingBg::exit() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: exit()";

#if defined(SURFING__USE_AUTOSAVE_FORCE_ON_EXIT) || !defined(SURFING__USE_AUTOSAVE_SETTINGS_ENGINE)
	save();
#endif
}

//--------------------------------------------------------------
void SurfingBg::save() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: save()";

	ofxSurfing::saveSettings(parameters, pathSettings);
}

//--------------------------------------------------------------
bool SurfingBg::load() {
	ofLogNotice("ofxSurfingPBR") << " SurfingBg: load()";

	bool b = false;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	b = ofxSurfing::loadSettings(parameters, pathSettings);

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	return b;
}

//--------------------------------------------------------------
void SurfingBg::setup() {

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

	parameters.setName("PBR_BG");

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
	//TODO: could be removed. We use main global only.
	colorGroup.setSerializable(false);

	brightGlobal.set("Bright", 0.5f, 0.05f, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX); //avoid reset at black..

	shininess.set("Shininess", 120, 0, 120);

	//--

	bGui.set("UI BG", false);

	bRotate.set("Rotate", true);
	speedRotate.set("Speed", 0, -1, 1);

	bDrawObject.set("Draw Bg Object", false);
	bDrawWireframe.set("Draw Wireframe", false);
	wireColor.set("Wire Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));

	bModeBox.set("Mode Box", false);
	bModeSphere.set("Mode Sphere", false);
	resolutionSphere.set("Resolution SPH", 0.5, 0, 1);
	resolutionBox.set("Resolution BOX", 0.5, 0, 1);

	sizeScene.set("Size", 0.5, 0, 1.f);

	bUseTexture.set("Use Texture", false);
	pathTexture.set("File Texture", "NONE");
	vOpenTexture.set("Open Texture");

	bSmoothLights.set("Smooth Lights", false);
	// Default Low Poly

	vResetAll.set("Reset All");
	vResetScene.set("Reset Scene");
	vResetColors.set("Reset Colors");

	//-

	// Bg Scene Object
	// sphere/box with or (colored) without textured

	paramsScene.setName("Scene");

	paramsScene.add(bDrawObject);
	paramsScene.add(bDrawBgColorPlain);

	paramsObject.setName("Object");
	paramsObject.add(bModeBox);
	paramsObject.add(bModeSphere);
	paramsObject.add(resolutionBox);
	paramsObject.add(resolutionSphere);
	paramsObject.add(sizeScene);

	paramsObject.add(bUseTexture);
	paramsObject.add(pathTexture);
	paramsObject.add(vOpenTexture);

	paramsObject.add(bDrawWireframe);
	paramsObject.add(wireColor);
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

	paramsColorizers.setName("Colorizers");
	paramsColorizers.add(shininess);

	paramsColorsGlobal.setName("Global");
	paramsColorsGlobal.add(brightGlobal);
	paramsColorsGlobal.add(globalColor);
	//paramsColorsGlobal.add(colorGroup);
	paramsColorizers.add(paramsColorsGlobal);

	paramsColor.setName("Colors");
	paramsColor.add(diffuse);
	paramsColor.add(ambient);
	paramsColor.add(specular);
	paramsColor.add(emissive);
	paramsColorizers.add(paramsColor);

	paramsColorizers.add(vResetColors);

	// Bg
	bDrawBgColorPlain.set("Draw Bg Plain Color", false);
	bgColorPlain.set("Bg Plain Color", ofFloatColor::darkGrey, ofFloatColor(0.f), ofFloatColor(1.f));
	bgColorPlainParams.setName("BG Plain Color");
	bgColorPlainParams.add(bDrawBgColorPlain);
	bgColorPlainParams.add(bgColorPlain);
	paramsColorizers.add(bgColorPlainParams);

	//parameters.add(bMini_Scene);

	parameters.add(paramsScene);
	parameters.add(paramsColorizers);
	parameters.add(vResetAll);

	listenerResetAll = vResetAll.newListener([this](void) {
		doResetAll();
	});

	// Callbacks
	ofAddListener(paramsScene.parameterChangedE(), this, &SurfingBg::ChangedScene);
	ofAddListener(paramsColorizers.parameterChangedE(), this, &SurfingBg::ChangedColors);
	ofAddListener(parameters.parameterChangedE(), this, &SurfingBg::Changed);

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

	gui.getGroup(paramsColorizers.getName()).minimize();

	gui.getGroup(paramsColorizers.getName())
		.getGroup(paramsColor.getName())
		.minimize();

	gui.getGroup(paramsColorizers.getName())
		.getGroup(bgColorPlainParams.getName())
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

	// Plain color Bg
	if (bDrawBgColorPlain) {
		ofClear(bgColorPlain);
	}

	if (bDrawObject) drawScene();
}

//--------------------------------------------------------------
void SurfingBg::update(ofEventArgs & args) {

	if (!bAppRunning && ofGetFrameNum() > 1) {
		bAppRunning = true;
	}

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
	if (!bModeSphere && !bModeBox) return;

	ofPushMatrix();
	{
		// scene center
		ofTranslate(center.x, center.y, center.z);

		//bool bInCam = true;
		//if (bInCam) { // scene center
		//	ofTranslate(center.x, center.y, center.z);
		//} else { // app window center
		//	ofTranslate(ofGetWidth() / 2.f, ofGetHeight() / 2.f, 0);
		//}

		if (bRotate) {
			float rmul = 2.f;
			float rmag = speedRotate * rmul * RAD_TO_DEG;
			float rot = ofMap(speedRotate, -1, 1, -rmag, rmag, true);
			ofRotateYDeg(ofGetElapsedTimef() * rmag);
		}

		//--

		ofPushStyle();

		// sphere
		if (bModeSphere) {
			ofSetSphereResolution(resolutionSphere_);
			ofDrawSphere(0, 0, 0, r);
		}

		// box
		if (bModeBox) {
			ofSetBoxResolution(resolutionBox_);
			ofDrawBox(0, 0, 0, 2.f * r);
		}

		ofPopStyle();
	}
	ofPopMatrix();
}
//--------------------------------------------------------------
void SurfingBg::drawScene() {
	if (!bModeSphere && !bModeBox) return;

		//--

#define SURFING_BG_MIN 1.f
#define SURFING_BG_MAX 5.f

	//bool bInCam = true;
	//if (bInCam)
	//	r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
	//		SURFING__SCENE_SIZE_UNIT * SURFING_BG_MIN,
	//		SURFING__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
	//else

	float r = SURFING__SCENE_SIZE_UNIT;

	// Face

	material.begin();
	{

		if (bModeSphere) {
			r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MIN,
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
		}

		else if (bModeBox) {
			r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MIN,
				SURFING__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
		}

		//--

		ofPushMatrix();
		ofPushStyle();
		{
			if (bUseTexture && img.isAllocated()) img.getTexture().bind();

			ofSetColor(255, 255, 255, 255);

			drawObject(r);

			if (bUseTexture && img.isAllocated()) img.getTexture().unbind();
		}
		ofPopStyle();
		ofPopMatrix();
	}
	material.end();

	//--

	// Wire

	if (bDrawWireframe) {
		ofPushMatrix();
		ofPushStyle();
		ofNoFill();
		ofSetColor(wireColor.get());
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

	//force
	if (pathTexture.get() == "" || pathTexture.get() == "NONE" || !img.isAllocated()) {
		pathTexture.setWithoutEventNotifications("images/001.jpg");
	}

	if (bModeBox) bModeBox = false;
	if (!bModeSphere) bModeSphere = true;

	resolutionSphere = 0.2f;
	resolutionBox = 0.2f;

	if (sizeScene != 0.5f) sizeScene = 0.5f;
	if (speedRotate != 0.01) speedRotate = 0.01;

	//if (bRotate) bRotate = false;
	//if (!bAutoSetColor) bAutoSetColor = true;
	//if (bUseTexture) bUseTexture = false;
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
void SurfingBg::Changed(ofAbstractParameter & e) {
#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif
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

	//#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	//	autoSaver.saveSoon();
	//#endif

	//--

	if (0) {
	}

	//--

	else if (name == globalColor.getName()) {
		colorGroup.set(globalColor.get());

		bFlagSetColorBgGroup = true;
	}
	//else if (name == colorGroup.getName()) {
	//	////TODO fix
	//	//if (!bDoneStartup) return;
	//	//if (!bAppRunning) return;
	//	bFlagSetColorBgGroup = true;
	//}

	else if (name == brightGlobal.getName()) {
		bFlagSetBrightToColorGroup = true;
	}

	//--

	else if (name == shininess.getName()) {
		material.setShininess(shininess);
	}

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

	//#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	//	autoSaver.saveSoon();
	//#endif

	//--

	if (0) {
	}

//--

//workflow
#if 1
	else if (name == bDrawObject.getName()) {
		if (bDrawObject) {
			if (bDrawBgColorPlain) bDrawBgColorPlain.set(false);
		}
	}
#endif
#if 0
	else if (name == bDrawObject.getName()) {
		if (bDrawObject) {
			if (bDrawBgColorPlain) bDrawBgColorPlain.set(false);
		} else {
			if (!bDrawBgColorPlain) bDrawBgColorPlain.set(true);
		}
	}
#endif

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
#if 1
	else if (name == bDrawBgColorPlain.getName()) {
		if (bDrawBgColorPlain) {
			if (bDrawObject) bDrawObject.set(false);
		}
	}
#endif

	//force one of both enabled
	//else if (name == bModeBox.getName()) {
	//	if (bModeBox.get()) {
	//		if (bModeSphere.get()) bModeSphere.set(false);
	//	} else {
	//		if (!bModeSphere.get()) bModeSphere.set(true);
	//	}
	//} else if (name == bModeSphere.getName()) {
	//	if (bModeSphere.get()) {
	//		if (bModeBox.get()) bModeBox.set(false);
	//	} else {
	//		if (!bModeBox.get()) bModeBox.set(true);
	//	}
	//}

	else if (name == bModeBox.getName()) {
		if (bModeBox.get()) {
			if (bModeSphere.get()) bModeSphere.setWithoutEventNotifications(false); //fix crash
		} else {
			if (!bModeSphere.get()) bModeSphere.setWithoutEventNotifications(true); //fix crash
		}
	} else if (name == bModeSphere.getName()) {
		if (bModeSphere.get()) {
			if (bModeBox.get()) bModeBox.setWithoutEventNotifications(false); //fix crash
		} else {
			if (!bModeBox.get()) bModeBox.setWithoutEventNotifications(true); //fix crash
		}
	}

	else if (name == resolutionSphere.getName()) {
		int RESOLUTION_SPHERE_MIN = 2;
		int RESOLUTION_SPHERE_MAX = 100;
		float r = ofMap(resolutionSphere.get(),
			resolutionSphere.getMin(), resolutionSphere.getMax(),
			RESOLUTION_SPHERE_MIN, RESOLUTION_SPHERE_MAX, true);
		resolutionSphere_ = (r);
	}

	else if (name == resolutionBox.getName()) {
		int RESOLUTION_BOX_MIN = 1;
		int RESOLUTION_BOX_MAX = 100;
		float r = ofMap(resolutionBox.get(),
			resolutionBox.getMin(), resolutionBox.getMax(),
			RESOLUTION_BOX_MIN, RESOLUTION_BOX_MAX, true);
		resolutionBox_ = r;
	}

	//--

	else if (name == vOpenTexture.getName()) {
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a PNG or JPG.");
		if (openFileResult.bSuccess) {
			ofLogNotice("SurfingBg") << ("User selected a file");
			processOpenFileSelection(openFileResult);
		} else {
			ofLogNotice("SurfingBg") << ("User hit cancel");
		}
	}

	else if (name == pathTexture.getName()) {

		//force
		if (pathTexture.get() == "" || pathTexture.get() == "NONE") {
			//string s = "001.jpg";
			string s = "003.jpg";
			pathTexture.setWithoutEventNotifications("images/" + s);
		}

		loadTexture(pathTexture.get());
	}

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

void SurfingBg::processOpenFileSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("SurfingBg") << "Name: " + openFileResult.getName();
	ofLogNotice("SurfingBg") << "Path: " + openFileResult.getPath();

	ofFile file(openFileResult.getPath());

	if (file.exists()) {
		ofLogVerbose("SurfingBg") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		if (fileExtension == "png" || fileExtension == "PNG" || fileExtension == "jpg" || fileExtension == "JPG") {
			pathTexture = openFileResult.getPath();
			img.load(pathTexture.get());
		} else {
			ofLogError("SurfingBg") << "Wrong file extension/format: " + fileExtension;
		}
	}
}

void SurfingBg::loadTexture(string path) {
	ofLogNotice("SurfingBg") << "loadTexture(" + path + ")";

	// Push arb
	bool b = ofxSurfing::pushSetArbTex(false);
	{
		//ofDisableArbTex();
		//img.load("images/001.jpg");
		img.load(path);
	}
	// Pop arb
	ofxSurfing::popSetArbTex(b);
}