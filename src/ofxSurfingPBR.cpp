#include "ofxSurfingPBR.h"
//--------------------------------------------------------------
ofxSurfingPBR::ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "ofxSurfingPBR() Constructor";

	ofAddListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofAddListener(ofEvents().windowResized, this, &ofxSurfingPBR::windowResized);
	ofAddListener(ofEvents().keyPressed, this, &ofxSurfingPBR::keyPressed);
}

//--------------------------------------------------------------
ofxSurfingPBR::~ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "~ofxSurfingPBR() Destructor";

	ofRemoveListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofRemoveListener(ofEvents().windowResized, this, &ofxSurfingPBR::windowResized);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingPBR::keyPressed);

	//--

	ofRemoveListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);

	ofRemoveListener(paramsDraw.parameterChangedE(), this, &ofxSurfingPBR::ChangedDraw);
	ofRemoveListener(paramsInternal.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofRemoveListener(paramsTestScene.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofRemoveListener(paramsCamera.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);

#ifdef SURFING__PBR__USE_CUBE_MAP
	ofRemoveListener(paramsCubeMap.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
#endif

	//--

#ifdef SURFING__PBR__USE_AUTO_CALL_EXIT_ON_DESTRUCTOR_IF_REQUIRED
	if (!bDoneExit) {
		ofLogWarning("ofxSurfingPBR") << "Force calling exit()!";
		ofLogWarning("ofxSurfingPBR") << "That's bc has not been called until now!";

		exit();
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::windowResized(ofResizeEventArgs & resize) {
	glm::vec2 sz { resize.width, resize.height };
	static glm::vec2 sz_ { -1, -1 };
	if (sz != sz_) {
		sz_ = sz;
		ofLogNotice("ofxSurfingPBR") << "windowResized() Size: " << sz_.x << "," << sz_.y;

		sWindowDimensions = "";

		buildHelp();
		refreshGui();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::buildHelp() {

	ofLogNotice("ofxSurfingPBR") << "buildHelp()";

	sHelp = "";
	sHelp += "HELP\n";
	sHelp += "ofxSurfingPBR\n";
	sHelp += "\n";
	sHelp += "KEYS " + string(bKeys ? "ON" : "OFF") + "\n";
	sHelp += "\n";
	if (bKeys) {
		sHelp += "h      Help\n";
		sHelp += "L-l    Help Layout\n";
		sHelp += "d      Debug\n";
		sHelp += "g-G    Gui-ofxGui\n";
		sHelp += "\n";
		sHelp += "DRAW\n";
		sHelp += "b      BG Mode\n";
		sHelp += "c      BG CubeMap\n";
		sHelp += "p      Floor Plane\n";
		sHelp += "B      Floor Box\n";
		sHelp += "i      Floor Infinite\n";
		sHelp += "s      Shadows \n";
		sHelp += "\n";
	}
	sHelp += "WINDOW\n";
#if 0 //will require build every frame!
	string s = "";
	float fps = ofGetFrameRate();
	s += ofToString(fps, fps > 100 ? 0 : 1);
	s += " FPS";
	s += "\n";
	sHelp += "       " + s;
#endif
	sHelp += "       " + ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
	if (bKeys) {
		sHelp += "f      FullScreen\n";
		sHelp += "u      Unlock FPS " + string(bUnlockFps ? "ON " : "OFF") + "\n";
		sHelp += "q-Q    Squared\n";
		sHelp += "1-5    Instagram Sizes\n";
		sHelp += sWindowDimensions + "\n";
	}
	if (bKeys) {
		sHelp += "\n";
		sHelp += "HELPERS\nMATERIAL\n";
		sHelp += "\n";
		sHelp += "RESET\n";
		sHelp += "F1     Full\n";
		sHelp += "\n";
		sHelp += "RANDOM\n";
		sHelp += "F2     Full\n";
		sHelp += "F3     Settings\n";
		sHelp += "\n";
		sHelp += "COLORS\n";
		sHelp += "F4     GlobalNoAlpha\n";
		sHelp += "F5     NoAlpha\n";
		sHelp += "F6     WithAlpha\n";
		sHelp += "F7     OnlyAlphas\n";
		sHelp += "\n";
		sHelp += "HISTORY BROWSER\n";
		sHelp += "z-x    Prev-Next\n";
		sHelp += "r      Recall\n";
		sHelp += "s-S    Store/Update\n";
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::doNextLayoutHelp() {
	ofLogNotice("ofxSurfingPBR") << "doNextLayoutHelp()";

	int i = helpLayout.get();
	i++;
	i = i % (helpLayout.getMax() + 1);
	helpLayout.set(i);

	buildHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::doPrevLayoutHelp() {
	ofLogNotice("ofxSurfingPBR") << "doPrevLayoutHelp()";

	int i = helpLayout.get();
	i--;
	if (i < 0) i = helpLayout.getMax();
	helpLayout.set(i);

	buildHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::doNextLayouGui() {
	ofLogNotice("ofxSurfingPBR") << "doNextLayouGui()";

	int i = indexGuiLayout.get();
	i = ofClamp(i, indexGuiLayout.getMin(), indexGuiLayout.getMax());
	i++;
	//i = i % indexGuiLayout.getMax();
	if (i > indexGuiLayout.getMax()) i = 0;
	indexGuiLayout.set(i);

	buildHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "setupParams()";

	bGui.set("PBR", true);
	bGuiOfxGui.set("PBR ofxGui", true);

	bDebug.set("Debug", false);
	bKeys.set("Keys", true);
	bHelp.set("Help", false);

	vMinimizeAllGui.set("Gui Minimize");
	vMaximizeAllGui.set("Gui Maximize");
	indexGuiLayout.set("Gui Layout", 0, 0, 1);
	nameGuiLayout.set("Gui LN ", "NONE");

	int imax = (int)ofxSurfing::SURFING_LAYOUT_AMOUNT - 1;
	int idef = (int)ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;
	helpLayout.set("Help Layout", idef, 0, imax);
	nameHelpLayout.set("Help LN", "NONE");
	nameGuiLayout.setSerializable(false);
	nameHelpLayout.setSerializable(false);

	//--

	parameters.setName("SCENE");

	vLoadAll.set("Load All");
	vSaveAll.set("Save All");

//TODO: make a full project save/load
#if 0
	parameters.add(vLoadAll);
	parameters.add(vSaveAll);
#endif

	floor.helpLayout.makeReferenceTo(helpLayout);
	floor.bDebug.makeReferenceTo(bDebug);

	paramsShowGui.setName("UI");
	paramsShowGui.add(bg.bGui);
	paramsShowGui.add(floor.bGui);
	paramsShowGui.add(material.bGui);
#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	paramsShowGui.add(lights.bGui);
#endif
	parameters.add(paramsShowGui);

	bDrawBg.set("Draw Bg", true);
#ifdef SURFING__PBR__USE_CUBE_MAP
	bDrawCubeMap.set("Draw Bg CubeMap", true);
#endif

	paramsDraw.setName("DRAW");

	paramsDraw.add(bDrawBg);
	paramsDrawBg.setName("BG");
#if 1
	paramsDrawBg.add(bg.bDrawBgColorObject);
	paramsDrawBg.add(bg.bDrawBgColorPlain);

	#ifdef SURFING__PBR__USE_CUBE_MAP
	paramsDrawBg.add(bDrawCubeMap);
	#endif
#endif
	paramsDraw.add(paramsDrawBg);

	paramsDraw.add(floor.bDraw);
#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	paramsDraw.add(lights.bDrawShadow);
#endif
	parameters.add(paramsDraw);

	//--

#ifdef SURFING__PBR__USE_CUBE_MAP
	setupCubeMap();
#endif

	//--

	paramsTestScene.setName("Test Scene");
	bGuiDrawTestScene.set("Draw Scene", true);
	scaleTestScene.set("Scale", 0, -1.f, 1.f);
	positionTestScene.set("Pos", { 0, 0, 0 }, { -1, -1, -1 }, { 1, 1, 1 });
	vResetTestScene.set("Reset TestScene");
	paramsTestScene.add(bGuiDrawTestScene);
	paramsTestScene.add(scaleTestScene);
	paramsTestScene.add(positionTestScene);
	paramsTestScene.add(vResetTestScene);

	//--

	paramsCamera.setName("Camera");
	vLoadCamera.set("Load");
	vSaveCamera.set("Save");
	bEnableCameraAutosave.set("Auto Save", true);
	vResetCamera.set("Reset Camera");
	paramsCamera.add(vLoadCamera);
	paramsCamera.add(vSaveCamera);
	paramsCamera.add(bEnableCameraAutosave);
	paramsCamera.add(vResetCamera);

	//--

	parameters.add(paramsCamera);
	parameters.add(paramsTestScene);

	//paramsInternal.add(bDebug);

	//--

	//paramsAdvanced.add(bGui); //workflow: added to add to settings. could hide the toggle..

	paramsAdvanced.setName("Advanced");
	paramsInternal.setName("Internal");

	paramsGui.setName("Gui");
	paramsGui.add(guiManager.bAutoLayout);
	paramsGui.add(vMinimizeAllGui);
	paramsGui.add(vMaximizeAllGui);
	paramsGui.add(indexGuiLayout);
	paramsGui.add(nameGuiLayout);
	paramsGui.add(helpLayout);
	paramsGui.add(nameHelpLayout);
	paramsGui.add(bGui);
	paramsGui.add(bGuiOfxGui);
	paramsGui.add(material.bGui);
	paramsAdvanced.add(paramsGui);

	paramsInternal.add(paramsAdvanced);
	paramsInternal.add(bKeys);
	parameters.add(paramsInternal);

	vResetAll.set("Reset All");

	parameters.add(vResetAll);
	parameters.add(bDebug);
	parameters.add(bHelp);

	//--

	// Callbacks
	setupCallbacks();

	//--

	bDoneSetupParams = true;
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupCallbacks() {
	ofLogNotice("ofxSurfingPBR") << "setupCallbacks()";

	eResetAll = vResetAll.newListener([this](void) {
		doResetAll(true);
	});

	eDebug = bDebug.newListener([this](bool) {
		lights.bDebug = bDebug;
	});

	eSaveAll = vSaveAll.newListener([this](void) {
		saveAll();
	});
	eLoadAll = vLoadAll.newListener([this](void) {
		loadAll();
	});

	// draw bg
	e_bDrawBgColorPlain = bg.bDrawBgColorPlain.newListener([this](bool & b) {
		if (bg.bDrawBgColorPlain) {
			if (bDrawCubeMap) bDrawCubeMap.set(false);
		}
	});
	e_bDrawBgColorObject = bg.bDrawBgColorObject.newListener([this](bool & b) {
		if (bg.bDrawBgColorObject) {
			if (bDrawCubeMap) bDrawCubeMap.set(false);
		}
	});
	// draw cubeMap
	e_bDrawCubeMap = bDrawCubeMap.newListener([this](bool & b) {
		if (!bLoadedCubeMap) return; //skip
		if (bDrawCubeMap) {
			if (bg.bDrawBgColorPlain) bg.bDrawBgColorPlain.set(false);
			if (bg.bDrawBgColorObject) bg.bDrawBgColorObject.set(false);
		}
	});

	ofAddListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);

	ofAddListener(paramsDraw.parameterChangedE(), this, &ofxSurfingPBR::ChangedDraw);
	ofAddListener(paramsInternal.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofAddListener(paramsTestScene.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofAddListener(paramsCamera.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);

#ifdef SURFING__PBR__USE_CUBE_MAP
	ofAddListener(paramsCubeMap.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
#endif

	//--

	//TODO: not works
	//#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	//		lights.bDebug.makeReferenceTo(bDebug);
	//#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupBg() {
	ofLogNotice("ofxSurfingPBR") << "setupBg()";

	bg.setup();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setup() {
	ofLogNotice("ofxSurfingPBR") << "setup() Start";

	// for using on any objects
	material.setup("MATERIAL");

	floor.setup();

	//--

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	lights.setup();

	//--

	//TODO
	//link
	//bDebug.makeReferenceTo(lights.bDebug);
#endif

	//--

	setupBg();

	setupParams();

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	callback_t f = std::bind(&ofxSurfingPBR::save, this);
	autoSaver.setFunctionSaver(f);
#endif

	//--

	setupGui();

	buildHelp();

	//--

	bDoneSetup = true;
	ofLogNotice("ofxSurfingPBR") << "setup() Done!";

	//--

	startup();
}

// Camera
//--------------------------------------------------------------
void ofxSurfingPBR::setup(ofCamera & camera_) {
	setCameraPtr(dynamic_cast<ofCamera *>(&camera_));

	this->setup();
}
//--------------------------------------------------------------
void ofxSurfingPBR::setCameraPtr(ofCamera & camera_) {
	setCameraPtr(dynamic_cast<ofCamera *>(&camera_));
}
//--------------------------------------------------------------
void ofxSurfingPBR::setCameraPtr(ofCamera * camera_) {
	camera = camera_;

	//FORCED
	camera->setFarClip(SURFING__PBR__SCENE_CAMERA_FAR_CLIP);
}
//--------------------------------------------------------------
ofCamera * ofxSurfingPBR::getOfCameraPtr() {
	if (camera != nullptr)
		return camera;
	else
		return nullptr;
}
//--------------------------------------------------------------
ofEasyCam * ofxSurfingPBR::getOfEasyCamPtr() {
	ofEasyCam * easyCam = dynamic_cast<ofEasyCam *>(camera);
	if (easyCam != nullptr)
		return easyCam;
	else
		return nullptr;
}
//--------------------------------------------------------------
ofRectangle ofxSurfingPBR::getGuiShape() const {
	ofRectangle r1 = gui.getShape();
	ofRectangle r2 = material.gui.getShape();
	ofRectangle bb = r1.getUnion(r2);
	return bb;
}
//--------------------------------------------------------------
const int ofxSurfingPBR::getGuiLayout() {
	return indexGuiLayout.get();
}
//--------------------------------------------------------------
const ofxSurfing::SURFING_LAYOUT ofxSurfingPBR::getLayoutHelp() {
	return ofxSurfing::SURFING_LAYOUT(helpLayout.get());
}

//--------------------------------------------------------------
bool ofxSurfingPBR::isWindowPortrait() {
	return ofGetHeight() > ofGetWidth();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setFunctionRenderScene(callback_t f) {
	ofLogNotice("ofxSurfingPBR") << "setFunctionRenderScene()";

	if (f_RenderScene == nullptr) {
		ofLogError("ofxSurfingPBR") << "setFunctionRenderScene(). Wrong callback_t nullptr";
	}
	f_RenderScene = f;

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	lights.setFunctionRenderScene(f_RenderScene);
#endif
}

//--------------------------------------------------------------
ofParameterGroup & ofxSurfingPBR::getMaterialParameters() { //mainly to expose to external gui's like ImGui
	return material.parameters;
}

//--------------------------------------------------------------
void ofxSurfingPBR::startup() {
	ofLogNotice("ofxSurfingPBR") << "startup() Begins!";

	bool b = load();

	if (!b) {
		ofLogWarning("ofxSurfingPBR") << "No file settings found!";
		ofLogWarning("ofxSurfingPBR") << "Initialize settings for the first time!";
		ofLogWarning("ofxSurfingPBR") << "Potential Newbie User Found!";

		//--
#if 1
		// Initialize a Default Startup Scene!
		ofLogWarning("ofxSurfingPBR") << "Forcing reset to a hardcoded Default Scene!";

		doResetDefaultScene();
#endif

		//--

		// Force enable Help and Debug states!
		// Ready to a newbie user!
		// All important/learning controls will be visible.
		ofLogWarning("ofxSurfingPBR") << "Forcing states and some default stuff visible:";
		ofLogWarning("ofxSurfingPBR") << "Enable help, debug, reset camera and settings, etc...";

		bGui = true;
		bGuiOfxGui = true;
		bHelp = true;
		bDebug = true;
	}

	else {
		ofLogNotice("ofxSurfingPBR") << "Located scene settings!";
		ofLogNotice("ofxSurfingPBR") << "App has been opened before. Not the first time!";
	}

	//--

	ofLogNotice("ofxSurfingPBR") << "startup() Done! at frame number: " << ofGetFrameNum();
	bDoneStartup = true;
}

// Will be called on the first update frame.
// Use this pattern carefully, as could be a bad practice in some scenarios!
//--------------------------------------------------------------
void ofxSurfingPBR::startupDelayed() {
	ofLogNotice("ofxSurfingPBR") << "startupDelayed(). Start at frame number: " << ofGetFrameNum();

	//--

#if 1
	// camera settings
	{
		bool b = this->getSettingsFileFoundForCamera();
		if (!b) {
			doResetCamera();
		} else if (camera != nullptr) {
			if (bEnableCameraAutosave) doLoadCamera();
		}
	}
#endif

	//--

//TODO fix crash callbacks
// some bypassed callbacks that makes the app crash...
#if 1
	indexGuiLayout = indexGuiLayout.get(); //trig crashes..
#else
	if (indexGuiLayout.get() == 0)
		nameGuiLayout.set(namesGuiLayouts[0]);
	else if (indexGuiLayout.get() == 1)
		nameGuiLayout.set(namesGuiLayouts[1]);
#endif

	//--

	string s = ofxSurfing::getLayoutName(helpLayout.get());
	nameHelpLayout.set(s);

	//--

	bDoneStartupDelayed = true;
	ofLogNotice("ofxSurfingPBR") << "startupDelayed() Done!";
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupGui() {

	gui.setup(parameters);
	refreshGui();

	//--

	guiManager.setup(&gui);

	// all ofxPanels positions and/or drawing are handled by guiManager
	guiManager.add(&gui, bGui);
	guiManager.add(&bg.gui, bg.bGui);
	guiManager.add(&floor.gui, floor.bGui);
	guiManager.add(&material.gui, material.bGui, ofxSurfing::SURFING__OFXGUI__MODE_ONLY_POSITION);
#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	guiManager.add(&lights.gui, lights.bGui, ofxSurfing::SURFING__OFXGUI__MODE_ONLY_POSITION);
#endif

	guiManager.startup();

	//--

	// assign to ofxGui icons
	eSaveOfxGui = gui.savePressedE.newListener([this] {
		save();
	});
	eLoadOfxGui = gui.loadPressedE.newListener([this] {
		load();
	});
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshGui() {
	//if (!guiManager.bAutoLayout) return;

	ofLogNotice("ofxSurfingPBR") << "refreshGui()";

	// anchor gui top-left
	gui.setPosition(SURFING__PAD_TO_WINDOW_BORDERS, SURFING__PAD_TO_WINDOW_BORDERS);

	// minimize

	gui.getGroup(paramsTestScene.getName()).minimize();
	gui.getGroup(paramsCamera.getName()).minimize();
	gui.getGroup(paramsInternal.getName()).minimize();

	gui.getGroup(paramsInternal.getName())
		.getGroup(paramsAdvanced.getName())
		.minimize();

	gui.getGroup(paramsDraw.getName())
		.getGroup(paramsDrawBg.getName())
		.minimize();

#ifdef SURFING__PBR__USE_CUBE_MAP
	gui.getGroup(paramsCubeMap.getName()).minimize();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void ofxSurfingPBR::update() {

	// App flow controls
	{
		// On the 1st frame
		int f = (int)(ofGetFrameNum());
		if (f >= 0) {
			if (!bAppRunning) {
				bAppRunning = true;
				ofLogNotice("ofxSurfingPBR") << "Starting app at frame number: " << ofGetFrameNum();
			}
		}

		// After the 1st frame
		if (f > 0 && bAppRunning && !bDoneStartupDelayed) {
			//TODO fix crash callbacks
			startupDelayed();
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::updateScene() {

	drawScene();
}

//--------------------------------------------------------------
void ofxSurfingPBR::beginMaterial() {
	material.begin();
}
//--------------------------------------------------------------
void ofxSurfingPBR::endMaterial() {
	material.end();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawOfxGui() {

	if (bGuiOfxGui) {
		gui.draw();

		guiManager.draw();

		// all ofxPanels positions and/or drawing are handled by guiManager
		material.drawGui();

		floor.drawGui();

		lights.drawGui();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawGui() {
	if (!bGui) return;

	ofDisableDepthTest();

	//--

	drawOfxGui();

	//--

	if (bDebug) drawDebug();
	if (bHelp) drawHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawHelp() {
	ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT(helpLayout.get()));

	//ofxSurfing::SURFING_LAYOUT l;
	//if (helpLayout.get() == 0) l= ofxSurfing::SURFING_LAYOUT_TOP_RIGHT;
	//else if (helpLayout.get() == 1) l= ofxSurfing::SURFING_LAYOUT_TOP_LEFT;
	//ofxSurfing::ofDrawBitmapStringBox(sHelp, l);
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawDebugFPS() {
	//TODO
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (!floor.bShaderToPlane && !floor.bDisplaceToMaterial)
#endif
	{
		string s = "";
		float fps = ofGetFrameRate();
		s += ofToString(fps, fps > 100 ? 0 : 1);
		s += " FPS";
		s += "\n";

		if (sWindowDimensions == "")
			s += ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
		else
			s += sWindowDimensions + "\n";

		ofxSurfing::SURFING_LAYOUT l = ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT;
#if 0
		if (indexGuiLayout.get() == 0)
			l = ofxSurfing::SURFING_LAYOUT_TOP_CENTER;
		else if (indexGuiLayout.get() == 1)
			l = ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER;
		else
			l = ofxSurfing::SURFING_LAYOUT_CENTER;
#endif

		ofxSurfing::ofDrawBitmapStringBox(s, l);
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawDebug() {

	// Fps and window size
	drawDebugFPS();

	floor.drawDebug();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawBg() {
	if (!bDrawBg) return;

	bg.draw();

#ifdef SURFING__PBR__USE_CUBE_MAP
	drawCubeMap();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawFloor() {
	floor.draw();
}

//--------------------------------------------------------------
void ofxSurfingPBR::draw() {
	if (f_RenderScene == nullptr) return;

	ofEnableDepthTest();

	//--

	updateScene();

	//--

	// camera
	camera->begin();
	{

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
		lights.begin();
#endif

		drawBg();

		f_RenderScene();

		//----

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
		lights.end();
#endif

		//--

		drawDebugScene();
	}
	camera->end();
}

#ifdef SURFING__PBR__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::drawCubeMap() {

	if (bDrawCubeMap && bLoadedCubeMap) {
		// Update debug cubeMap
		// drawing of the cube map renders at max depth, so it can be drawn last
		// this will allow for the benefit of depth clipping
		if (cubeMapMode == 2) {
			if (cubeMap.hasPrefilteredMap()) {
				cubeMap.drawPrefilteredCube(cubeMapprefilterRoughness.get());
			} else {
				ofLogWarning("ofxSurfingPBR") << "DOES NOT HAVE PRE FILTERED CUBE MAP " << ofGetFrameNum();
			}
		} else if (cubeMapMode == 3) {
			if (cubeMap.hasIrradianceMap()) {
				cubeMap.drawIrradiance();
			}
		} else {
			if (cubeMap.hasCubeMap()) {
				cubeMap.drawCubeMap();
			}
		}

		cubeMap.setExposure(cubeMapExposure);
	}
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::drawScene() {

	/*
		Usually to be called with ofEnableDepthTest() and before camera.begin() 
		void ofApp::draw(){
			ofEnableDepthTest();
			//computeLights();
			camera.begin();
	*/

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	lights.updateLights();
#endif

	//--

	//#ifdef SURFING__PBR__USE_CUBE_MAP
	//	drawCubeMap();
	//#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawDebugScene() {
	if (!bGui) return;

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	// Debug lights and shadows
	lights.drawDebugLights();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::Changed(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogVerbose("ofxSurfingPBR") << "Changed: " << name << ": " << e;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedInternal(ofAbstractParameter & e) {
	//if (bDisableCallbacks) return;

	std::string name = e.getName();

	//TODO fix crash callbacks
	//Exception thrown : read access violation.std::shared_ptr<of::priv::BaseEvent<of::priv::Function<ofAbstractParameter, std::recursive_mutex>, std::recursive_mutex>::Data>::operator->**<of::priv::BaseEvent<of::priv::Function<ofAbstractParameter, std::recursive_mutex>, std::recursive_mutex>::Data, 0> **(...) returned nullptr.

	//// Exclude to avoid crashes..
	//if (name == nameGuiLayout.getName()) return;

	ofLogNotice("ofxSurfingPBR") << "ChangedInternal: " << name << ": " << e;

	//--

	if (name == bKeys.getName()) {
		buildHelp();
	}

	else if (name == helpLayout.getName()) {
		string s = ofxSurfing::getLayoutName(helpLayout.get());

		if (nameHelpLayout.get() != s) nameHelpLayout.set(s);
		//if (nameHelpLayout.get() != s) nameHelpLayout.setWithoutEventNotifications(s);
	}

	else if (name == indexGuiLayout.getName()) {
		//do not updates the gui!
		if (indexGuiLayout.get() == 0) {
			string n = namesGuiLayouts[0];
			if (nameGuiLayout.get() != n)
				nameGuiLayout.setWithoutEventNotifications(n);
		}

		else if (indexGuiLayout.get() == 1) {
			string n = namesGuiLayouts[1];
			if (nameGuiLayout.get() != n)
				nameGuiLayout.setWithoutEventNotifications(n);
		}

		//--

		//TODO fix crash callbacks
		if (!bDoneStartup) return;

		//--

		if (indexGuiLayout.get() == 0) {
			string n = namesGuiLayouts[0];
			if (nameGuiLayout.get() != n)
				nameGuiLayout.set(n);
		}

		else if (indexGuiLayout.get() == 1) {
			string n = namesGuiLayouts[1];
			if (nameGuiLayout.get() != n)
				nameGuiLayout.set(n);
		}
	}

	else if (name == vMinimizeAllGui.getName()) {
		gui.minimizeAll();

		material.gui.minimizeAll();
		material.guiHelpers.minimizeAll();
	}

	else if (name == vMaximizeAllGui.getName()) {
		gui.maximizeAll();
		material.gui.maximizeAll();
		material.guiHelpers.maximizeAll();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedDraw(ofAbstractParameter & e) {
	//if (bDisableCallbacks) return;

	std::string name = e.getName();

	//if (name == bDrawCubeMap.getName()) return;

	ofLogNotice("ofxSurfingPBR") << "ChangedDraw: " << name << ": " << e;

	//--

#if 0
	//workflow
	if (name == bg.bDrawBgColorPlain.getName() && bg.bDrawBgColorPlain) {
		//bDisableCallbacks = 1;
		if (bDrawCubeMap) bDrawCubeMap.set(false);
		//bDisableCallbacks = 0;
	}

	else if (name == bg.bDrawBgColorObject.getName() && bg.bDrawBgColorObject) {
		//bDisableCallbacks = 1;
		if (bDrawCubeMap) bDrawCubeMap.set(false);
		//bDisableCallbacks = 0;
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedTestScene(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedTestScene: " << name << ": " << e;

	//--

	if (name == vResetTestScene.getName()) {
		doResetTestScene();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedCamera(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedCamera: " << name << ": " << e;

	//--

	if (name == vSaveCamera.getName()) {
		doSaveCamera();
	} else if (name == vLoadCamera.getName()) {
		doLoadCamera();
	} else if (name == vResetCamera.getName()) {
		doResetCamera();

		// workflow: save
		if (bEnableCameraAutosave) doSaveCamera();
	}
}

#ifdef SURFING__PBR__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::ChangedCubeMaps(ofAbstractParameter & e) {
	//if (bDisableCallbacks) return;

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedCubeMaps: " << name << ": " << e;

	//--

	if (name == vOpenCubeMap.getName()) {
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a EXR, HDR or JPG (limited mode) files.");
		if (openFileResult.bSuccess) {
			ofLogNotice("ofxSurfingPBR") << ("User selected a file");
			processOpenFileSelection(openFileResult);

		} else {
			ofLogNotice("ofxSurfingPBR") << ("User hit cancel");
		}
	}

	else if (name == path_CubemapFileAbsPath.getName()) {
		string path = path_CubemapFileAbsPath.get();
		if (path == "" || path == "NONE") {
			//ignore
		} else {
			loadCubeMap(path);
		}
	}

	else if (name == vResetCubeMap.getName()) {
		if (!bLoadedCubeMap) return; //skip
		doResetCubeMap();
	}

	else if (name == cubeMapMode.getName()) {
		//TODO:
		//return;//fix crash
		if (!bLoadedCubeMap) return; //skip

		string s = "NONE";

		if (cubeMapMode.get() == 1) {
			s = "Cube Map";
			if (cubeMapModeName.get() != s) cubeMapModeName.set(s);
		}

		else if (cubeMapMode.get() == 2) {
			s = "PreFilter Roughness";
			if (cubeMapModeName.get() != s) cubeMapModeName.set(s);
		}

		else if (cubeMapMode.get() == 3) {
			s = "Irradiance";
			if (cubeMapModeName.get() != s) cubeMapModeName.set(s);
		}
	}

	//--

	//workflow:
	#if 0
	else if (name == bDrawCubeMap.getName()) {
		//bDisableCallbacks = 1;

		if (!bLoadedCubeMap) return; //skip

		if (bDrawCubeMap) {
			if (bg.bDrawBgColorPlain) bg.bDrawBgColorPlain.set(false);
			if (bg.bDrawBgColorObject) bg.bDrawBgColorObject.set(false);
		}

		//bDisableCallbacks = 0;
	}
	#endif
}
#endif

//--

//--------------------------------------------------------------
void ofxSurfingPBR::drawTestScene() {
	if (!bGuiDrawTestScene) return;

	//----

	ofPushStyle();
	ofSetConeResolution(50, 10, 2);
	ofSetSphereResolution(50);

	//--

	pushTestSceneTransform();

	{
		float u = SURFING__PBR__SCENE_SIZE_UNIT / 10.f;

		// Cone
		ofPushMatrix();
		ofTranslate(-2 * u, u, 0);
		ofRotateXDeg(180);
		ofDrawCone(0, 0, 0, u * 0.65f, u);
		ofPopMatrix();

		// Box
		ofPushMatrix();
		float spd = 10; //secs per lap
		auto t = ofWrap(ofGetElapsedTimef(), 0, spd);
		auto d = ofMap(t, 0, spd, 0, 360, true);
		ofRotateYDeg(d);

		ofDrawBox(0, u, 0, u);
		ofPopMatrix();

		// Sphere
		ofDrawSphere(2 * u, u, 0, u / 2.f);
	}

	popTestSceneTransform();

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxSurfingPBR::pushTestSceneTransform() {
#if (DO_SCENE_TEST_TRANSFORMS)
	// Scene transforms
	ofPushMatrix();

	// Position
	float unit = SURFING__PBR__SCENE_SIZE_UNIT / 2.f;

	float x = ofMap(positionTestScene.get().x, -1.f, 1.f, -unit, unit, true);
	float y = ofMap(positionTestScene.get().y, -1.f, 1.f, -unit, unit, true);
	float z = ofMap(positionTestScene.get().z, -1.f, 1.f, -unit, unit, true);

	// Scale
	float s = ofMap(scaleTestScene, -1.f, 1.f,
		1.f / SURFING__PBR__SCENE_TEST_UNIT_SCALE, SURFING__PBR__SCENE_TEST_UNIT_SCALE, true);

	ofTranslate(x, y, z);
	ofScale(s);
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::popTestSceneTransform() {
#if DO_SCENE_TEST_TRANSFORMS
	ofPopMatrix();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::exit() {
	ofLogNotice("ofxSurfingPBR") << "exit()";

#ifndef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	// Should not mandatory as settings should be internally auto saved when changing.

	// Not required to be called bc it's using the auto saver!
	save();

	material.exit();
	bg.exit();

	#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	lights.exit();
	#endif
#endif

	if (bEnableCameraAutosave) doSaveCamera();

	bDoneExit = true;
}

//--------------------------------------------------------------
void ofxSurfingPBR::save() {
	ofLogNotice("ofxSurfingPBR") << "Save Scene -> " << path;

	// save scene
	ofxSurfing::saveSettings(parameters, path);
}

//--------------------------------------------------------------
bool ofxSurfingPBR::load() {
	ofLogNotice("ofxSurfingPBR") << "Load Scene -> " << path;

	bool b;

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	//--

	// load scene
	b = ofxSurfing::loadSettings(parameters, path);

	// CubeMap
	loadCubeMap(path_CubemapFileAbsPath.get());

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	return b;
}

//--------------------------------------------------------------
void ofxSurfingPBR::saveAll() {
	ofLogNotice("ofxSurfingPBR") << "saveAll -> " << path;

	// save scene
	ofxSurfing::saveSettings(parameters, path);

	// save all: material, bg, lights and camera.
	material.save();
	bg.save();

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	lights.save();
#endif

	doSaveCamera();
}

//--------------------------------------------------------------
bool ofxSurfingPBR::loadAll() {
	ofLogNotice("ofxSurfingPBR") << "loadAll -> " << path;

	bool b;

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	//--

	b = ofxSurfing::loadSettings(parameters, path);

	// CubeMap
	loadCubeMap(path_CubemapFileAbsPath.get());

	// Load all: material, bg, lights and camera.
	material.load();
	bg.load();

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	lights.load();
#endif

	doLoadCamera();

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	return b;
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFoundForScene() {

	// search for the mandatory settings file
	// to consider if the app is opened for the first time.
	bool b = ofxSurfing::checkFileExist(path);
	if (b) {
		ofLogNotice("ofxSurfingPBR") << "getSettingsFileFound(): Found file settings!";
		ofLogNotice("ofxSurfingPBR") << "Found PBR SCENE settings file: " << path;
	} else {
		ofLogWarning("ofxSurfingPBR") << "PBR SCENE settings file: " << path << " not found!";
	}
	return b;
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFound() {
	/*
		We will search for the settings files:
		ofxSurfingPBR_Scene.json -> mandatory to consider that the app open for the first time!
		ofxSurfingPBR_Material.json -> optional
		ofxSurfingPBR_Camera.json -> optional
	*/

	bool b = getSettingsFileFoundForScene();

	//--

	// not required, but we log if it's located or not.
	getSettingsFileFoundForMaterial();

	// not required, but we log if it's located or not.
	getSettingsFileFoundForCamera();

	//--

	if (!b) ofLogWarning("ofxSurfingPBR") << "Probably opening the App for the first time!";

	return b;
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFoundForMaterial() {
	bool b = ofxSurfing::checkFileExist(material.path);
	if (b) {
		ofLogNotice("ofxSurfing") << "Found MATERIAL settings file: " << path;
	} else {
		ofLogWarning("ofxSurfing") << "MATERIAL settings file: " << path << " not found!";
	}
	return b;
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFoundForCamera() {
	bool b = ofxSurfing::checkFileExist(pathCamera);
	if (b) {
		ofLogNotice("ofxSurfingPBR") << "Found CAMERA settings file: " << pathCamera;
	} else {
		ofLogWarning("ofxSurfingPBR") << "CAMERA settings file: " << pathCamera << " not found!";
	}
	return b;
}

//--

#ifdef SURFING__PBR__USE_CUBE_MAP
//--------------------------------------------------------------
bool ofxSurfingPBR::loadCubeMap(string path) {
	ofCubeMap::ofCubeMapSettings csettings;

	// It will uses the passed path only if it's not empty or "NONE"!
	// If not, it will use the default hardcoded path!
	if (path == "" || path == "NONE")
		path = path_CubemapsFolder + "/" + path_CubemapFilenameDefaultData;

	csettings.filePath = path;

	// loading a cube map will enable image based lighting on PBR materials.
	// cube map will create a prefiltered light cube map and an irradiance cube map
	// cube map texture from https://polyhaven.com
	// comment out the loading of the cube map image to see added cube map lighting without image
	// fake environment lighting is added in the pbr shader

	// uncomment to load from a cache or make one if it doesn't exist
	// 	csettings.useCache = true;

	// uncomment to use the maximum precision available. OpenGL ES is float16 and OpenGL is float32;
	//	csettings.useMaximumPrecision = true;

	// make sure the defaults are the same for making and loading the cache
	// ie opengl es defaults are smaller and the file names to load are based on the resolution
	csettings.irradianceRes = 32;
	csettings.preFilterRes = 128;

	bLoadedCubeMap = cubeMap.load(csettings);
	// below is another method for loading a cube map without passing settings class
	// cubeMap.load("modern_buildings_2_1k.exr", 512, true );

	//--

	if (bLoadedCubeMap) {
		ofLogNotice("ofxSurfingPBR") << "Successfully loaded cubeMap from path: " << path;

		cubeMapMode.set(cubeMapMode.get()); //refresh

		//get basename
		cubeMapName = ofFilePath::getBaseName(path);

		//remove extension
		string n = cubeMapName.get();
		string s = ofFilePath::removeExt(n);
		if (cubeMapName.get() != s) cubeMapName.set(s);

		path = ofToDataPath(path, true);

		if (path_CubemapFileAbsPath.get() != path) path_CubemapFileAbsPath.set(path);

		ofLogNotice("ofxSurfingPBR") << "CubeMap path: " << path_CubemapFileAbsPath.get();
	}

	else {
		ofLogError("ofxSurfingPBR") << "Error loading cubeMap: " << path;

		string s = "NONE";
		cubeMapModeName = s;
		cubeMapName = s;

		if (path_CubemapFileAbsPath.get() != s) path_CubemapFileAbsPath.set(s);
	}

	return bLoadedCubeMap;
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupCubeMap() {
	ofLogNotice("ofxSurfingPBR") << "setupCubeMap()";

	cubeMapModeName.set("Type", "NONE");
	cubeMapMode.set("Mode", 2, 1, 3);
	cubeMapName.set("N", "NONE");
	path_CubemapFileAbsPath.set("P", "NONE");
	cubeMapprefilterRoughness.set("Roughness", 0.25f, 0, 1.f);
	cubeMapExposure.set("Exposure", 0.25, 0.0, 1.0);
	vOpenCubeMap.set("Open File");
	vResetCubeMap.set("Reset CubeMap");

	cubeMapName.setSerializable(false);
	cubeMapModeName.setSerializable(false);

	//--

	paramsCubeMap.setName("CubeMap");

	paramsCubeMap.add(bDrawCubeMap);
	paramsCubeMap.add(cubeMapMode); //index
	paramsCubeMap.add(cubeMapModeName); //display
	paramsCubeMap.add(cubeMapExposure);
	paramsCubeMap.add(cubeMapprefilterRoughness);
	paramsCubeMap.add(cubeMapName);
	paramsCubeMap.add(path_CubemapFileAbsPath);
	paramsCubeMap.add(vOpenCubeMap);
	paramsCubeMap.add(vResetCubeMap);

	parameters.add(paramsCubeMap);
}

//--------------------------------------------------------------
void ofxSurfingPBR::processOpenFileSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("ofxSurfingPBR") << "processOpenFileSelection()";
	ofLogNotice("ofxSurfingPBR") << "Name: " + openFileResult.getName();
	ofLogNotice("ofxSurfingPBR") << "Path: " + openFileResult.getPath();

	ofFile file(openFileResult.getPath());

	if (file.exists()) {
		ofLogVerbose("ofxSurfingPBR") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		if (fileExtension == "exr" || fileExtension == "EXR" || fileExtension == "hdr" || fileExtension == "HDR" || fileExtension == "jpg" || fileExtension == "JPG") {

			loadCubeMap(openFileResult.getPath());

			if (fileExtension == "jpg" || fileExtension == "JPG") {
				//only cube map mode is supported for jpg's!
				cubeMapMode = 1;
			}
		} else {
			ofLogError("ofxSurfingPBR") << "Wrong file extension/format: " + fileExtension;
		}
	}
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::keyPressed(ofKeyEventArgs & eventArgs) {
	const int key = eventArgs.key;

	// modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	ofLogVerbose("ofxSurfingPBR") << "keyPressed: " << key;

	keyPressed(key);
}

//--------------------------------------------------------------
void ofxSurfingPBR::keyPressed(int key) {
	if (!bKeys) return;

	ofLogNotice("ofxSurfingPBR") << "keyPressed(" << key << ")";

	if (key == 'g') bGui = !bGui;
	if (key == 'G') bGuiOfxGui = !bGuiOfxGui;

	floor.keyPressed(key);

	if (key == 'b') {
		if (bg.bDrawBgColorObject) {
			bg.bDrawBgColorPlain = true;
		}

		else if (bg.bDrawBgColorPlain) {
			bDrawCubeMap = true;
		}

		else if (bDrawCubeMap) {
			bDrawCubeMap = false;
		}

		else if (!bDrawCubeMap && !bg.bDrawBgColorObject && !bg.bDrawBgColorPlain) {
			bg.bDrawBgColorObject = true;
		}
	}

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	if (key == 's') lights.bDrawShadow = !lights.bDrawShadow;
#endif

#ifdef SURFING__PBR__USE_CUBE_MAP
	if (key == 'c') bDrawCubeMap = !bDrawCubeMap;
#endif

	if (key == 'h') bHelp = !bHelp;
	if (key == 'd') bDebug = !bDebug;

	if (key == 'l') doNextLayoutHelp(); //next layout help
	if (key == 'L') doPrevLayoutHelp(); //prev layout help
	//if (key == OF_KEY_TAB) doNextLayouGui(); //next layout gui

	if (key == 'f') ofToggleFullscreen();
	if (key == 'u') {
		bUnlockFps = !bUnlockFps;
		if (bUnlockFps) {
			ofSetVerticalSync(false);
			ofSetFrameRate(0);
		} else {
			ofSetVerticalSync(true);
			ofSetFrameRate(60);
		}
	}

	sWindowDimensions = ofxSurfing::keyPressedToSetWindowShape(key);
	// 'q' 800 x 800
	// 'Q' w x w
	// '1' IGTV Cover Photo
	// '2' IG Landscape Photo
	// '3' IG Portrait
	// '4' IG Story
	// '5' IG Square

	material.keyPressed(key);

	buildHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setLogLevel(ofLogLevel logLevel) {
	ofLogNotice("ofxSurfingPBR") << "setLogLevel(" << logLevel << ")";

	ofSetLogLevel("ofxSurfingPBR", logLevel);
}

#ifdef SURFING__PBR__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::doResetCubeMap() {
	ofLogNotice("ofxSurfingPBR") << "doResetCubeMap()";

	cubeMapMode = 2;
	cubeMapprefilterRoughness = 0.25f;
	cubeMapExposure = 0.25f;
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::doLoadCamera() {
	if (camera == nullptr) return;
	ofLogNotice("ofxSurfingPBR") << "doLoadCamera()";

	ofxLoadCamera(*camera, pathCamera);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doSaveCamera() {
	if (camera == nullptr) return;
	ofLogNotice("ofxSurfingPBR") << "doSaveCamera()";

	ofxSaveCamera(*camera, pathCamera);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetCamera() {
	if (camera == nullptr) return;
	ofLogNotice("ofxSurfingPBR") << "doResetCamera()";

	ofEasyCam * easyCam = dynamic_cast<ofEasyCam *>(camera);

	if (easyCam != nullptr) {
		easyCam->reset();

		camera->setFarClip(SURFING__PBR__SCENE_CAMERA_FAR_CLIP);

		//camera->setVFlip(true);

		easyCam->setPosition({ 8.35512, 581.536, 696.76 });
		easyCam->setOrientation({ 0.940131, -0.340762, 0.00563653, 0.00204303 });
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetAll(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "doResetAll()";

	bDebug = false;

	// cubeMap
#ifdef SURFING__PBR__USE_CUBE_MAP
	doResetCubeMap();
#endif

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	// lights
	lights.doResetAllLights(bHard);

	// shadows
	lights.doResetShadow();
#endif

	// shader displacer
	floor.doResetAll();

	// material
	material.doResetMaterial(bHard);

	// test scene
	doResetTestScene();

	// camera
	doResetCamera();

	// bg
	if (bHard) bg.doResetAll();
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetTestScene() {
	ofLogNotice("ofxSurfingPBR") << "doResetTestScene()";

	scaleTestScene = -0.6;
	positionTestScene = { 0, 0, 0 };
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetDefaultScene() {
	ofLogNotice("ofxSurfingPBR") << "doResetDefaultScene()";

#if 0
	doResetAll(true);
	//bg.doResetAll();
	//floor.doResetAll();
	//material.doResetAll();
#else

	// Background
	bg.bDrawBgColorObject.set(true);
	bg.bDrawWireframe.set(true);
	bg.setColorGlobal(ofFloatColor::darkKhaki);

	// Floor
	floor.doResetAll();
	floor.setColorGlobal(ofFloatColor::darkSeaGreen);

	// Material
	material.roughness = 0.5;
	material.reflectance = 0.5;
	material.globalColor.set(ofFloatColor::orange);

	// Scene
	doResetTestScene();
#endif
}