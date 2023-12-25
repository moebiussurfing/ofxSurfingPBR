#include "SurfingBg.h"

//--------------------------------------------------------------
SurfingBg::SurfingBg() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:SurfingBg()";

	ofAddListener(ofEvents().update, this, &SurfingBg::update);
}

//--------------------------------------------------------------
SurfingBg::~SurfingBg() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:~SurfingBg()";

	ofRemoveListener(ofEvents().update, this, &SurfingBg::update);

	ofRemoveListener(paramsScene.parameterChangedE(), this, &SurfingBg::ChangedScene);
	ofRemoveListener(paramsBgColor.parameterChangedE(), this, &SurfingBg::ChangedBgColor);
	ofRemoveListener(paramsColorObject.parameterChangedE(), this, &SurfingBg::ChangedBgColorObject);
	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingBg::Changed);
}

//--------------------------------------------------------------
void SurfingBg::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:exit()";

#ifdef SURFING__PBR__USE_AUTO_CALL_EXIT_ON_DESTRUCTOR_IF_REQUIRED
	save();
#endif
}

//--------------------------------------------------------------
void SurfingBg::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:save()";

	ofxSurfing::saveSettings(parameters, pathSettings);
}

//--------------------------------------------------------------
bool SurfingBg::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:load()";

	bool b = false;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	b = ofxSurfing::loadSettings(parameters, pathSettings);

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	return b;
}

//--------------------------------------------------------------
void SurfingBg::setup() {

	center = glm::vec3(0, 0, 0);

	//--

	setupParameters();
	setupCallbacks();
	setupGui();

	startup();
}

//--------------------------------------------------------------
void SurfingBg::setupParameters() {

	parameters.setName("BG");

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

	shininess.set("Shininess", 0.5, 0, SURFING__PBR__MAX_SHININESS);

	//--

	bGui.set("UI BG", false);

	bRotate.set("Rotate", true);
	speedRotate.set("Speed", 0, -1, 1);

	bDrawBgColorObject.set("Draw BG Color Object", false);
	bDrawWireframe.set("Draw BG Wireframe", false);
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

	vResetAll.set("Reset All");
	vResetScene.set("Reset BG Scene");
	vResetColors.set("Reset Colors");

	//-

	// Bg Scene Object
	// sphere/box with or (colored) without textured

	bDrawBgColorPlain.set("Draw BG Color Plain", false);
	bgColorPlain.set("Color Plain", ofFloatColor::darkGrey, ofFloatColor(0.f), ofFloatColor(1.f));

	paramsScene.setName("BG Scene");
	paramsScene.add(bDrawBgColorPlain);
	paramsScene.add(bDrawBgColorObject);
	paramsScene.add(bDrawWireframe);

	paramsBgColor.setName("BG Color");
	paramsBgColor.add(globalColor);
	paramsBgColor.add(wireColor);
	paramsBgColor.add(vResetColors);

	paramsColorMaterial.setName("Colors");
	paramsColorMaterial.add(diffuse);
	paramsColorMaterial.add(ambient);
	paramsColorMaterial.add(specular);
	paramsColorMaterial.add(emissive);

	paramsColorObject.setName("BG Color Object");
	paramsColorObject.add(bModeBox);
	paramsColorObject.add(bModeSphere);
	paramsColorObject.add(resolutionBox);
	paramsColorObject.add(resolutionSphere);
	paramsColorObject.add(sizeScene);
	paramsColorObject.add(shininess);
	paramsColorObject.add(bUseTexture);
	paramsColorObject.add(pathTexture);
	paramsColorObject.add(vOpenTexture);
	paramsColorObject.add(paramsColorMaterial);

	paramsColorPlain.setName("BG Color Plain");
	paramsColorPlain.add(bDrawBgColorPlain);
	paramsColorPlain.add(bgColorPlain);
	paramsScene.add(paramsBgColor);

	paramsScene.add(paramsColorObject);
	paramsScene.add(paramsColorPlain);

	paramsRotate.setName("Anim");
	paramsRotate.add(bRotate);
	paramsRotate.add(speedRotate);
	paramsScene.add(paramsRotate);

	paramsScene.add(vResetScene);

	parameters.add(paramsScene);
	parameters.add(vResetAll);
}

//--------------------------------------------------------------
void SurfingBg::setupCallbacks() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:setupCallbacks()";

	eResetAll = vResetAll.newListener([this](void) {
		doResetAll();
	});
	eResetScene = vResetScene.newListener([this](void) {
		doResetScene();
	});
	eResetColors = vResetColors.newListener([this](void) {
		doResetColors();
	});

	// Callbacks
	ofAddListener(paramsScene.parameterChangedE(), this, &SurfingBg::ChangedScene);
	ofAddListener(paramsBgColor.parameterChangedE(), this, &SurfingBg::ChangedBgColor);
	ofAddListener(paramsColorObject.parameterChangedE(), this, &SurfingBg::ChangedBgColorObject);
	ofAddListener(parameters.parameterChangedE(), this, &SurfingBg::Changed);

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	// auto saver
	callback_t f = std::bind(&SurfingBg::save, this);
	//register the local save function to be called when saving is required.
	autoSaver.setFunctionSaver(f);
#endif
}

//--------------------------------------------------------------
void SurfingBg::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:startup()";

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
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:setupGui()";

	gui.setup(parameters);

	eSaveOfxGui = gui.savePressedE.newListener([this] {
		save();
	});
	eLoadOfxGui = gui.loadPressedE.newListener([this] {
		load();
	});

	refreshGui();
}

//--------------------------------------------------------------
void SurfingBg::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:refreshGui()";

	gui.getGroup(paramsScene.getName())
		.getGroup(paramsRotate.getName())
		.minimize();

	gui.getGroup(paramsScene.getName())
		.getGroup(paramsColorObject.getName())
		.getGroup(paramsColorMaterial.getName())
		.minimize();

	if (!bDrawBgColorPlain) {
		gui.getGroup(paramsScene.getName())
			.getGroup(paramsColorPlain.getName())
			.minimize();
	} else {
		gui.getGroup(paramsScene.getName())
			.getGroup(paramsColorPlain.getName())
			.maximize();
	}

	if (!bDrawBgColorObject) {
		gui.getGroup(paramsScene.getName())
			.getGroup(paramsColorObject.getName())
			.minimize();
	} else {
		gui.getGroup(paramsScene.getName())
			.getGroup(paramsColorObject.getName())
			.maximize();
	}
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

	if (bDrawBgColorObject) drawScene();
}

//--------------------------------------------------------------
void SurfingBg::update(ofEventArgs & args) {

	//TODO: improve this settings pattern..
	if (!bAppRunning && ofGetFrameNum() > 1) {
		bAppRunning = true;
	}

	if (bFlagSetColorBgGroup) {
		bFlagSetColorBgGroup = false;

		setColorBgGroup(colorGroup.get());
	}
}

//--------------------------------------------------------------
void SurfingBg::drawObject(float r) {
	if (!bModeSphere && !bModeBox) return;

	ofPushMatrix();
	{
		// scene center
		ofTranslate(center.x, center.y, center.z);

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

	float r = SURFING__PBR__SCENE_SIZE_UNIT;

	// Face

	material.begin();
	{

		if (bModeSphere) {
			r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
				SURFING__PBR__SCENE_SIZE_UNIT * SURFING_BG_MIN,
				SURFING__PBR__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
		}

		else if (bModeBox) {
			r = ofMap(sizeScene, sizeScene.getMin(), sizeScene.getMax(),
				SURFING__PBR__SCENE_SIZE_UNIT * SURFING_BG_MIN,
				SURFING__PBR__SCENE_SIZE_UNIT * SURFING_BG_MAX, true);
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
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:doResetAll()";

	doResetColors();
	doResetScene();
}

//--------------------------------------------------------------
void SurfingBg::doResetColors() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:doResetColors()";

	ofFloatColor c = ofFloatColor(0.5f, 1.f);
	globalColor.set(c);

	ofFloatColor cw = ofFloatColor(0.f, 0.25f);
	wireColor.set(cw);

	//--

	bgColorPlain.set(ofFloatColor(0.2f));
#ifdef SURFING__PBR__USE_CUBE_MAP
	if (bDrawBgColorPlain) bDrawBgColorPlain = false;
#else
	bDrawBg = true;
#endif
}

//--------------------------------------------------------------
void SurfingBg::doResetScene() {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:doResetScene()";

	//force
	if (pathTexture.get() == "" || pathTexture.get() == "NONE" || !img.isAllocated()) {
		pathTexture.setWithoutEventNotifications("images/001.jpg");//file should be there!
	}

#if 0
	if (bModeBox) bModeBox = false;
	if (!bModeSphere) bModeSphere = true;
#else
	if (!bModeBox) bModeBox = true;
	if (bModeSphere) bModeSphere = false;
#endif

	if (!bDrawBgColorObject) bDrawBgColorObject.set(true);

	resolutionSphere = 1.f;
	resolutionBox = 1.f;

	if (sizeScene != 0.5f) sizeScene = 0.f;
	if (speedRotate != 0.01) speedRotate = 0.01;
}

//--------------------------------------------------------------
void SurfingBg::setColorGlobal(ofFloatColor color) {
	globalColor.set(color);
}

//--------------------------------------------------------------
void SurfingBg::setBgColorPlain(ofFloatColor c) {
	ofLogNotice("ofxSurfingPBR") << "SurfingBg:setBgColorPlain(" << c << ")";

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

	ofLogNotice("ofxSurfingPBR") << "SurfingBg:setColorBgGroup(" << c << ")";

	diffuse.set(c);
	ambient.set(c);
	specular.set(c);
	emissive.set(c);

	//--

	bgColorPlain.set(c);
}

//--------------------------------------------------------------
void SurfingBg::Changed(ofAbstractParameter & e) {
#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif
}

//--------------------------------------------------------------
void SurfingBg::ChangedBgColor(ofAbstractParameter & e) {
	string name = e.getName();

	//--

	ofLogNotice("ofxSurfingPBR") << "SurfingBg:ChangedBgColor: " << name << ": " << e;

	if (name == globalColor.getName()) {
		colorGroup.set(globalColor.get());

		bFlagSetColorBgGroup = true;
	}
}

//--------------------------------------------------------------
void SurfingBg::ChangedBgColorObject(ofAbstractParameter & e) {
	string name = e.getName();

	//--

	ofLogNotice("ofxSurfingPBR") << "SurfingBg:ChangedBgColorObject: " << name << ": " << e;

	//force one of both enabled
	if (name == bModeBox.getName()) {
		if (bModeBox.get()) {
			if (bModeSphere.get()) bModeSphere.set(false);
		} else {
			if (!bModeSphere.get()) bModeSphere.set(true);
		}
	} else if (name == bModeSphere.getName()) {
		if (bModeSphere.get()) {
			if (bModeBox.get()) bModeBox.set(false);
		} else {
			if (!bModeBox.get()) bModeBox.set(true);
		}
	}

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
}

//--------------------------------------------------------------
void SurfingBg::ChangedScene(ofAbstractParameter & e) {
	//if (bDisableCallbacks) return;

	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingBg:ChangedScene: " << name << ": " << e;

	//--

	if (0) {
	}

	//workflow
	else if (name == bDrawBgColorObject.getName()) {
		if (bDrawBgColorObject) {
			if (bDrawBgColorPlain) bDrawBgColorPlain.set(false);
		}
		refreshGui();
	}

	else if (name == bDrawBgColorPlain.getName()) {
		if (bDrawBgColorPlain) {
			if (bDrawBgColorObject) bDrawBgColorObject.set(false);
		}
		refreshGui();
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
	bool b = ofxSurfing::PushSetArbTex(false);
	{
		img.load(path);
	}
	// Pop arb
	ofxSurfing::PopSetArbTex(b);
}