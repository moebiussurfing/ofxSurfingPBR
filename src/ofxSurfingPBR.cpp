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

	ofRemoveListener(drawParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDraw);
	ofRemoveListener(floorParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedFloor);
	ofRemoveListener(internalParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofRemoveListener(testSceneParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofRemoveListener(cameraParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	ofRemoveListener(displacersParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDisplacers);
#endif

#ifdef SURFING__PBR__USE_CUBE_MAP
	ofRemoveListener(cubeMapParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
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
		//sHelp += "       " + nameHelpLayout.get() + "\n";
		sHelp += "d      Debug\n";
		sHelp += "g-G    Gui-ofxGui\n";
		//sHelp += "Tab    Layout UI\n";
		//sHelp += "       " + nameGuiLayout.get() + "\n";
		sHelp += "\n";
		sHelp += "DRAW\n";
		sHelp += "p      Floor Plane\n";
		sHelp += "B      Floor Box\n";
		sHelp += "i      Floor Infinite\n";
		sHelp += "b      Background Mode\n";
		sHelp += "s      Shadows \n";
		sHelp += "c      CubeMap\n";
		sHelp += "\n";
	}
	sHelp += "WINDOW\n";
	//sHelp += "       " + ofToString(ofGetFrameRate(), 1) + " FPS\n";
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

	int i = guiLayout.get();
	i = ofClamp(i, guiLayout.getMin(), guiLayout.getMax());
	i++;
	//i = i % guiLayout.getMax();
	if (i > guiLayout.getMax()) i = 0;
	guiLayout.set(i);

	buildHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "setupParams()";

	bGui.set("PBR", true);
	bGui_ofxGui.set("PBR ofxGui", true);
	bGuiFloor.set("UI FLOOR", false);

	bDebug.set("Debug", false);
	bKeys.set("Keys", true);
	bHelp.set("Help", false);

	vMinimizeAllGui.set("Gui Minimize");
	vMaximizeAllGui.set("Gui Maximize");
	guiLayout.set("Gui Layout", 0, 0, 1);
	nameGuiLayout.set("Gui LN ", "NONE");

	int imax = (int)ofxSurfing::SURFING_LAYOUT_AMOUNT - 1;
	int idef = (int)ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;
	helpLayout.set("Help Layout", idef, 0, imax);
	nameHelpLayout.set("Help LN", "NONE");

	nameGuiLayout.setSerializable(false);
	nameHelpLayout.setSerializable(false);

	//--

	bDrawFloorPlane.set("Draw Floor Plane", true);
	bDrawFloorBox.set("Draw Floor Box", false);

	//--

	parameters.setName("SCENE");

	vLoadAll.set("Load All");
	vSaveAll.set("Save All");

//TODO: make a full project save/load
#if 0
	parameters.add(vLoadAll);
	parameters.add(vSaveAll);
#endif

	showGuiParams.setName("UI");
	showGuiParams.add(bGuiFloor);
	showGuiParams.add(bg.bGui);
	showGuiParams.add(material.bGui);

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	showGuiParams.add(lights.bGui);
#endif

	parameters.add(showGuiParams);

	drawParams.setName("DRAW");
	drawParams.add(bDrawFloorPlane);
	drawParams.add(bDrawFloorBox);

	drawParams.add(bg.bDrawBgColorObject);
	drawParams.add(bg.bDrawBgColorPlain);

#ifdef SURFING__PBR__USE_CUBE_MAP
	bDrawCubeMap.set("Draw Bg CubeMap", true);
	drawParams.add(bDrawCubeMap);
#endif

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	drawParams.add(lights.bDrawShadow);
#endif

	parameters.add(drawParams);

	//--

	floorParams.setName("FLOOR");
	floorTransformParams.setName("Transform");
	internalParams.setName("Internal");
	advancedParams.setName("Advanced");

	//--

	vResetFloor.set("Reset Floor");
	vResetFloorTransform.set("Reset Transform");
	vResetAll.set("Reset All");

	//--

	bFloorWireframe.set("Draw Wireframe", false);
	bFloorInfinite.set("Infinite", true);
	bFloorSquaredSize.set("Squared", false);
	floorSize.set("Size", glm::vec2(0.5f, 0.5f), glm::vec2(0, 0), glm::vec2(1.f, 1.f));
	floorResolution.set("Resolution", glm::vec2(0.5f, 0.5f), glm::vec2(0, 0), glm::vec2(1.f, 1.f));
	floorRotation.set("x Rotation", 0, -45, 135);
	floorPosition.set("y Position", 0, -1, 1);

	//--

	floorParams.add(materialFloor.bGui);
	floorParams.add(bDrawFloorPlane);
	floorParams.add(bDrawFloorBox);
	floorParams.add(bFloorWireframe);

	//--

	materialFloor.setup("MATERIAL FLOOR");

	floorTransformParams.add(floorPosition);
	floorTransformParams.add(floorRotation);
	floorTransformParams.add(floorSize);
	floorTransformParams.add(bFloorSquaredSize);
	floorTransformParams.add(bFloorInfinite);
	floorTransformParams.add(floorBoxDepth);
	floorTransformParams.add(floorResolution);
	floorTransformParams.add(vResetFloorTransform);
	floorParams.add(floorTransformParams);

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	setupParamsDisplace();
#endif

	floorParams.add(vResetFloor);

	//--

	//parameters.add(floorParams);

	//--

#ifdef SURFING__PBR__USE_CUBE_MAP
	setupCubeMap();
#endif

	//--

	testSceneParams.setName("Test Scene");
	bGui_DrawTestScene.set("Draw Scene", true);
	scaleTestScene.set("Scale", 0, -1.f, 1.f);
	positionTestScene.set("Pos", { 0, 0, 0 }, { -1, -1, -1 }, { 1, 1, 1 });
	vResetTestScene.set("Reset TestScene");
	testSceneParams.add(bGui_DrawTestScene);
	testSceneParams.add(scaleTestScene);
	testSceneParams.add(positionTestScene);
	testSceneParams.add(vResetTestScene);

	//--

	cameraParams.setName("Camera");
	vLoadCamera.set("Load");
	vSaveCamera.set("Save");
	bEnableCameraAutosave.set("Auto Save", true);
	vResetCamera.set("Reset Camera");
	cameraParams.add(vLoadCamera);
	cameraParams.add(vSaveCamera);
	cameraParams.add(bEnableCameraAutosave);
	cameraParams.add(vResetCamera);

	//--

	parameters.add(cameraParams);
	parameters.add(testSceneParams);

	//internalParams.add(bDebug);

	//--

	//advancedParams.add(bGui); //workflow: added to add to settings. could hide the toggle..

	advancedParams.add(material.getIndexStateParam());
	//advancedParams.add(material.bGuiHelpers);

	guiParams.setName("Gui");
	guiParams.add(guiManager.bAutoLayout);
	guiParams.add(vMinimizeAllGui);
	guiParams.add(vMaximizeAllGui);
	guiParams.add(guiLayout);
	guiParams.add(nameGuiLayout);
	guiParams.add(helpLayout);
	guiParams.add(nameHelpLayout);
	guiParams.add(bGui);
	guiParams.add(bGui_ofxGui);
	guiParams.add(material.bGui);
	advancedParams.add(guiParams);

	internalParams.add(advancedParams);

	internalParams.add(bKeys);
	parameters.add(internalParams);

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

	listenerResetAll = vResetAll.newListener([this](void) {
		doResetAll();
	});

	listenerDebug = bDebug.newListener([this](bool) {
		lights.bDebug = bDebug;
	});

	listenerSaveAll = vSaveAll.newListener([this](void) {
		saveAll();
	});

	listenerLoadAll = vLoadAll.newListener([this](void) {
		loadAll();
	});

	ofAddListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);

	ofAddListener(drawParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDraw);
	ofAddListener(floorParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedFloor);
	ofAddListener(internalParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofAddListener(testSceneParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofAddListener(cameraParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	ofAddListener(displacersParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDisplacers);
#endif

#ifdef SURFING__PBR__USE_CUBE_MAP
	ofAddListener(cubeMapParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
#endif

	//--

	//TODO: not works
	//#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	//		lights.bDebug.makeReferenceTo(bDebug);
	//#endif
}

//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void ofxSurfingPBR::refreshImgShaderPlane() {
	ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane()";

	int w = floorPlane.getWidth();
	int h = floorPlane.getHeight();

	if (bLimitImage) {
		w = ofClamp(w, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MIN);
		h = ofClamp(h, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MIN);
	} else {
		w = ofClamp(w, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MAX);
		h = ofClamp(h, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MAX);
	}

	{

	#ifdef TARGET_OPENGLES
		OPENGL ES supports GL_RGBA32F but not GL_R32F
			img.allocate(80, 60, OF_IMAGE_COLOR_ALPHA);
	#else
		//img.clear();

		img.allocate(w, h, OF_IMAGE_GRAYSCALE);

		ofLogNotice("ofxSurfingPBR") << "Allocated img: " << w << "," << h;
	#endif

		//TODO: seems breaking some plane colors/materials props..
		//apply to plane
		floorPlane.mapTexCoordsFromTexture(img.getTexture());

		ofLogNotice("ofxSurfingPBR") << "mapTexCoordsFromTexture(img.getTexture()";
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupShaderPlane() {
	ofLogNotice("ofxSurfingPBR") << "setupShaderPlane()";

	ofDisableArbTex();

	bLoadedShaderPlane = shaderPlane.load("shadersGL3/shader");

	refreshImgShaderPlane();

	doResetNoise();
	doResetDisplace();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupParamsDisplace() {
	ofLogNotice("ofxSurfingPBR") << "setupParamsDisplace()";

	vResetDisplace.set("Reset Displace");
	vResetNoise.set("Reset Noise");

	bShaderToPlane.set("ShaderToPlane", false);
	bDisplaceToMaterial.set("DispToMaterial", false);
	bLimitImage.set("Limit IMG", true);

	noiseParams.setName("Noise Generator");
	noiseParams.add(noiseAmplitude.set("Noise Amp D", 0.0f, 0.0f, 2.f));
	noiseParams.add(noiseScale.set("Noise Scale", 0.0f, 0.0f, 0.1f));
	noiseParams.add(noiseSpeed.set("Noise Speed", 0.0f, 0.0f, 1.0f));
	noiseParams.add(vResetNoise);

	displaceMaterialParams.setName("Material Disp");
	displaceMaterialParams.add(displacementStrength.set("D Strength", 0.0, 0.0, 500.0));
	displaceMaterialParams.add(displacementNormalsStrength.set("D Normals Strength", 0.0, 0.0, 1.0));
	displaceMaterialParams.add(normalGeomToNormalMapMix.set("NGeom NMapMix", 0.0, 0.0, 1.0));
	displaceMaterialParams.add(vResetDisplace);

	displacersParams.setName("Displacers");
	displacersParams.add(bLimitImage);
	displacersParams.add(bDisplaceToMaterial);
	displacersParams.add(bShaderToPlane);
	displacersParams.add(noiseParams);
	displacersParams.add(displaceMaterialParams);

	floorParams.add(displacersParams);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetNoise() {
	ofLogNotice("ofxSurfingPBR") << "doResetNoise()";

	noiseAmplitude.set(1.f);
	noiseScale.set(0.05f);
	noiseSpeed.set(0.5f);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetDisplace() {
	ofLogNotice("ofxSurfingPBR") << "doResetDisplace()";

	// reduce callbacks amount a bit not updating params if not required..

	float displacementStrength_ = 100;
	//float displacementStrength_ = displacementStrength.getMax() * 0.75;
	float displacementNormalsStrength_ = displacementNormalsStrength.getMax() / 2;
	float normalGeomToNormalMapMix_ = normalGeomToNormalMapMix.getMax() / 2;

	if (displacementStrength.get() != displacementStrength_)
		displacementStrength.set(displacementStrength_);

	if (displacementNormalsStrength.get() != displacementNormalsStrength_)
		displacementNormalsStrength.set(displacementNormalsStrength_);

	if (normalGeomToNormalMapMix.get() != normalGeomToNormalMapMix_)
		normalGeomToNormalMapMix.set(normalGeomToNormalMapMix_);

	//if (!bLimitImage) bLimitImage.set(true);
	//if (bShaderToPlane) bShaderToPlane.set(false);
	//if (bDisplaceToMaterial) bDisplaceToMaterial.set(false);
}

//--------------------------------------------------------------
void ofxSurfingPBR::updateDisplace() {

	// compute noise
	// note that this makes drop the FPS a lot!
	float n = noiseSpeed * ofGetElapsedTimef();

	auto & pixels = img.getPixels();
	int numChannels = pixels.getNumChannels();
	int w = img.getWidth();
	int h = img.getHeight();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int i = y * w + x;
			float value = noiseAmplitude * ofNoise(x * noiseScale, y * noiseScale, n);
			for (int k = 0; k < numChannels; k++) {
				pixels[i * numChannels + k] = value;
			}
		}
	}

	img.update();

	// apply displace to material plane
	if (bDisplaceToMaterial) {
		materialFloor.setDisplacementTexture(img.getTexture());

		materialFloor.setDisplacementStrength(displacementStrength);
		materialFloor.setDisplacementNormalsStrength(displacementNormalsStrength);
		materialFloor.setNormalGeomToNormalMapMix(normalGeomToNormalMapMix);

		//materialFloor.setTexCoordScale(scaleX, scaleY);
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::beginShaderPlane() {
	shaderPlane.begin();
	shaderPlane.setUniformTexture("displacement", img.getTexture(), 1);
}
//--------------------------------------------------------------
void ofxSurfingPBR::endShaderPlane() {
	shaderPlane.end();
}
#endif

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

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	setupShaderPlane();
#endif

	//--

	// reset all except material
	// to avoid do it twice and overwrite loaded settings!
	doResetAll(true);
	//doResetAll();

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
	return guiLayout.get();
}
//--------------------------------------------------------------
const ofxSurfing::SURFING_LAYOUT ofxSurfingPBR::getLayoutHelp() {
	return ofxSurfing::SURFING_LAYOUT(helpLayout.get());
}
//--------------------------------------------------------------
bool ofxSurfingPBR::isVisibleDebugShader() {
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDebug && (bShaderToPlane || bDisplaceToMaterial))
		return true;
	else
		return false;
#else
	return false;
#endif
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
		bGui_ofxGui = true;
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
	;
}

//// Will be called on the first update frame.
///	Use this pattern carefully, as could be a bad practice in some scenarios!
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
	guiLayout = guiLayout.get(); //trig crashes..
#else
	if (guiLayout.get() == 0)
		nameGuiLayout.set(namesGuiLayouts[0]);
	else if (guiLayout.get() == 1)
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
	guiFloor.setup(floorParams);

	refreshGui();

	//--

	// all ofxPanels positions and/or drawing are handled by guiManager
	guiManager.setup(&gui);
	guiManager.add(&gui, bGui);
	guiManager.add(&guiFloor, bGuiFloor);
	guiManager.add(&material.gui, material.bGui, ofxSurfing::SURFING__OFXGUI__MODE_ONLY_POSITION);
	guiManager.add(&bg.gui, bg.bGui);
#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	guiManager.add(&lights.gui, lights.bGui);
#endif
	guiManager.startup();

	//--

	// assign to ofxGui icons
	listenerSaveGui = gui.savePressedE.newListener([this] {
		save();
	});
	listenerLoadGui = gui.loadPressedE.newListener([this] {
		load();
	});
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshGui() {
	if (!guiManager.bAutoLayout) return;

	ofLogNotice("ofxSurfingPBR") << "refreshGui()";

	// top-left
	gui.setPosition(SURFING__PAD_TO_WINDOW_BORDERS, SURFING__PAD_TO_WINDOW_BORDERS);

	// minimize sub panels

	guiFloor.getGroup(floorTransformParams.getName())
		.minimize();

	guiFloor.getGroup(floorTransformParams.getName())
		.getGroup(floorSize.getName())
		.minimize();

	guiFloor.getGroup(floorTransformParams.getName())
		.getGroup(floorResolution.getName())
		.minimize();

	//gui.getGroup(floorParams.getName()).minimize();

	materialFloor.refreshGui();

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	//gui.getGroup(floorParams.getName())
	//	.getGroup(displacersParams.getName())
	//	.minimize();

	//gui.getGroup(floorParams.getName())
	//	.getGroup(displacersParams.getName())
	//	.getGroup(displaceMaterialParams.getName())
	//	.minimize();

	//gui.getGroup(floorParams.getName())
	//	.getGroup(displacersParams.getName())
	//	.getGroup(noiseParams.getName())
	//	.minimize();
#endif

	gui.getGroup(testSceneParams.getName()).minimize();
	gui.getGroup(cameraParams.getName()).minimize();
	gui.getGroup(internalParams.getName()).minimize();
	//gui.getGroup(drawParams.getName()).minimize();

	gui.getGroup(internalParams.getName())
		.getGroup(advancedParams.getName())
		.minimize();

#ifdef SURFING__PBR__USE_CUBE_MAP
	gui.getGroup(cubeMapParams.getName()).minimize();
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

	//--

	if (bFlagRefreshFloorPlane) {
		bFlagRefreshFloorPlane = false;

		refreshFloorPlane();
	}

	if (bFlagRefreshFloorBox) {
		bFlagRefreshFloorBox = false;

		refreshFloorBox();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::updatePBRScene() {

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDisplaceToMaterial || bShaderToPlane) {
		updateDisplace();
	}
#endif

	//--

	drawPBRScene();
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
void ofxSurfingPBR::beginMaterialFloor() {
	materialFloor.begin();
}
//--------------------------------------------------------------
void ofxSurfingPBR::endMaterialFloor() {
	materialFloor.end();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawOfxGui() {

	if (bGui_ofxGui) {
		gui.draw();

		guiManager.draw();

		// all ofxPanels positions and/or drawing are handled by guiManager
		material.drawGui();

		if (bGuiFloor) 
		{
		ofxSurfing::setGuiPositionBelowTo(materialFloor.gui, guiFloor);
			materialFloor.drawGui();
		}
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
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (!bShaderToPlane && !bDisplaceToMaterial)
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
		if (guiLayout.get() == 0)
			l = ofxSurfing::SURFING_LAYOUT_TOP_CENTER;
		else if (guiLayout.get() == 1)
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

	//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	// Shader and displacer info
	if (bShaderToPlane || bDisplaceToMaterial) {
		// layout is responsive

		int x, y, w, h;

		float r = img.getHeight() / (float)img.getWidth();

		// info
		string s = "";
		s += " \n";
		s += "DEBUG\nSHADER\n\n";
		s += "FLOOR PLANE\n";
		s += "Size: ";
		s += ofToString(floorPlane.getWidth(), 0) + "x" + ofToString(floorPlane.getHeight(), 0) + "\n";
		s += "Resolution: ";
		s += ofToString(floorPlane.getResolution().x) + "x" + ofToString(floorPlane.getResolution().y) + "\n\n";
		s += "IMAGE\n";
		s += "Size: ";
		s += ofToString(img.getWidth()) + "x" + ofToString(img.getHeight()) + "\n\n";
		s += "WINDOW\n";
		s += ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
		s += ofToString(ofGetFrameRate(), 1) + " FPS";

		ofxSurfing::SURFING_LAYOUT l;
		l = ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;

		//TODO
		// Set the preview box position related to help box layout.

		if (helpLayout == ofxSurfing::SURFING_LAYOUT_TOP_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_TOP_CENTER)
			l = ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_TOP_RIGHT)
			l = ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;

		if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_TOP_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_CENTER_RIGHT)
			l = ofxSurfing::SURFING_LAYOUT_CENTER_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_CENTER_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_CENTER_RIGHT;

		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_TOP_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER)
			l = ofxSurfing::SURFING_LAYOUT_TOP_CENTER;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT)
			l = ofxSurfing::SURFING_LAYOUT_TOP_RIGHT;

		ofxSurfing::ofDrawBitmapStringBox(s, l);

		auto bb = ofxSurfing::getBBBitmapStringBoxToLayout(s, l);

		//image preview
		w = bb.getWidth();
		h = w * r;
		x = bb.getTopLeft().x;

		//fix to avoid empty pixels on the union..
		float round = (float)SURFING__STRING_BOX__DEFAULT_ROUND;
		bool isRound = round > 0;

		// set image pos above text box
		y = bb.getTopLeft().y - h;

		//move image below the text box
		if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT)
			y = bb.getBottomLeft().y; //TOP_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER)
			y = bb.getBottomLeft().y; //TOP_CENTER;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT)
			y = bb.getBottomLeft().y; //TOP_RIGHT;

		img.draw(x, y, w, h);

		// border rect line
		x += 1;
		w -= 2;
		ofRectangle rbb = ofRectangle(x, y, w, h);
		ofPushStyle();
		ofNoFill();
		ofSetColor(0, 255);
		ofDrawRectangle(rbb);
		ofPopStyle();
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawBg() {
	bg.draw();
}

//--------------------------------------------------------------
void ofxSurfingPBR::draw() {
	if (f_RenderScene == nullptr) return;

	ofEnableDepthTest();

	//--

	updatePBRScene();

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

		drawPBRSceneDebug();
	}
	camera->end();
}

#ifdef SURFING__PBR__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::updateCubeMap() {

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
void ofxSurfingPBR::drawPBRScene() {

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

#ifdef SURFING__PBR__USE_CUBE_MAP
	updateCubeMap();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPBRSceneDebug() {
	if (!bGui) return;

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	// Debug lights and shadows
	lights.drawDebugLights();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshFloorPlane() {

	if (bFloorSquaredSize) {
		float x = floorSize.get().x;
		float y = floorSize.get().y;
		if (y != x) {
			floorSize.set(glm::vec2(x, x));
		}
	}

	int w, h;

	if (bFloorInfinite) {
		// size hardcoded to a safety max!
		w = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
		h = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
	} else {
		// size from normalized param multiplied by a unit
		int planeSizeUnit = SURFING__PBR__SCENE_SIZE_UNIT * SURFING__PBR__PLANE_SIZE_MULTIPLIER;
		w = floorSize.get().x * planeSizeUnit;
		h = floorSize.get().y * planeSizeUnit;
	}

	//--

	int xResolution;
	int yResolution;

	xResolution = (int)ofMap(floorResolution.get().x, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	yResolution = (int)ofMap(floorResolution.get().y, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	floorPlane.set(w, h, xResolution, yResolution);

	//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDisplaceToMaterial || bShaderToPlane)
		refreshImgShaderPlane();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshFloorBox() {

	if (bFloorSquaredSize) {
		float x = floorSize.get().x;
		float y = floorSize.get().y;
		if (y != x) {
			floorSize.set(glm::vec2(x, x));
		}
	}

	int w, h;

	if (bFloorInfinite) {
		// size hardcoded to a safety max!
		w = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
		h = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
	} else {
		// size from normalized param multiplied by a unit
		int planeSizeUnit = SURFING__PBR__SCENE_SIZE_UNIT * SURFING__PBR__PLANE_SIZE_MULTIPLIER;
		w = floorSize.get().x * planeSizeUnit;
		h = floorSize.get().y * planeSizeUnit;
	}

	//--

	int xResolution;
	int yResolution;

	xResolution = (int)ofMap(floorResolution.get().x, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	yResolution = (int)ofMap(floorResolution.get().y, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	floorBox.set(w, h, floorBoxDepth, xResolution, yResolution, 1);
}

//--------------------------------------------------------------
void ofxSurfingPBR::Changed(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogVerbose("ofxSurfingPBR") << "Changed " << name << ": " << e;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedFloor(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedFloor " << name << ": " << e;

	//--

	if (0) {
	}

	else if (name == bDrawFloorPlane.getName()) {
		if (bDrawFloorPlane)
			if (bDrawFloorBox) bDrawFloorBox = false;
	}

	else if (name == bDrawFloorBox.getName()) {
		if (bDrawFloorBox)
			if (bDrawFloorPlane) bDrawFloorPlane = false;
	}

	else if (name == bFloorSquaredSize.getName()) {
		if (bFloorSquaredSize) {
			bFlagRefreshFloorPlane = true;
			bFlagRefreshFloorBox = true;
		}
	}

	else if (name == floorSize.getName()) {
		static glm::vec2 floorSize_ = glm::vec2(-1, -1);
		if (floorSize.get() != floorSize_) { // if changed

			floorSize_ = floorSize.get();

			bFlagRefreshFloorPlane = true;
			bFlagRefreshFloorBox = true;
		} else {
			ofLogVerbose("ofxSurfingPBR") << "Floor Plane size not Changed. Skipped refresh!";
		}
	}

	else if (name == floorResolution.getName()) {
		bFlagRefreshFloorPlane = true;
		bFlagRefreshFloorBox = true;
	}

	else if (name == bFloorInfinite.getName()) {
		bFlagRefreshFloorPlane = true;
		bFlagRefreshFloorBox = true;
	}

	else if (name == floorBoxDepth.getName()) {
		bFlagRefreshFloorBox = true;
	}

	else if (name == floorRotation.getName()) {
		glm::vec3 axis(1.0f, 0.0f, 0.0f);
		float angle = floorRotation.get() - 90;
		glm::quat q = glm::angleAxis(glm::radians(angle), axis);

		floorPlane.setOrientation(q);
		floorBox.setOrientation(q);
	}

	else if (name == floorPosition.getName()) {
		floorPlane.setPosition(0, floorPosition.get() * SURFING__PBR__SCENE_SIZE_UNIT * 5.f, 0);
		floorBox.setPosition(0, floorPosition.get() * SURFING__PBR__SCENE_SIZE_UNIT * 5.f, 0);
	}

	else if (name == vResetFloor.getName()) {
		doResetFloor();
	}

	else if (name == vResetFloorTransform.getName()) {
		doResetFloorTransform();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedInternal(ofAbstractParameter & e) {
	if (bDisableCallbacks) return;

	std::string name = e.getName();

	//TODO fix crash callbacks
	//Exception thrown : read access violation.std::shared_ptr<of::priv::BaseEvent<of::priv::Function<ofAbstractParameter, std::recursive_mutex>, std::recursive_mutex>::Data>::operator->**<of::priv::BaseEvent<of::priv::Function<ofAbstractParameter, std::recursive_mutex>, std::recursive_mutex>::Data, 0> **(...) returned nullptr.

	//// Exclude to avoid crashes..
	//if (name == nameGuiLayout.getName()) return;

	ofLogNotice("ofxSurfingPBR") << "ChangedInternal " << name << ": " << e;

	//--

	if (name == bKeys.getName()) {
		buildHelp();
	}

	else if (name == helpLayout.getName()) {
		string s = ofxSurfing::getLayoutName(helpLayout.get());

		if (nameHelpLayout.get() != s) nameHelpLayout.set(s);
		//if (nameHelpLayout.get() != s) nameHelpLayout.setWithoutEventNotifications(s);
	}

	else if (name == guiLayout.getName()) {
		//do not updates the gui!
		if (guiLayout.get() == 0) {
			string n = namesGuiLayouts[0];
			if (nameGuiLayout.get() != n)
				nameGuiLayout.setWithoutEventNotifications(n);
		}

		else if (guiLayout.get() == 1) {
			string n = namesGuiLayouts[1];
			if (nameGuiLayout.get() != n)
				nameGuiLayout.setWithoutEventNotifications(n);
		}

		//--

		//TODO fix crash callbacks
		if (!bDoneStartup) return;

		//--

		if (guiLayout.get() == 0) {
			string n = namesGuiLayouts[0];
			if (nameGuiLayout.get() != n)
				nameGuiLayout.set(n);
		}

		else if (guiLayout.get() == 1) {
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

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedDraw " << name << ": " << e;

	//--

	//workflow
	if (name == bg.bDrawBgColorPlain.getName() && bg.bDrawBgColorPlain.get()) {
		if (bDrawCubeMap) bDrawCubeMap.set(false);
	}

	else if (name == bg.bDrawBgColorObject.getName() && bg.bDrawBgColorObject.get()) {
		if (bDrawCubeMap) bDrawCubeMap.set(false);
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedTestScene(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedTestScene " << name << ": " << e;

	//--

	if (name == vResetTestScene.getName()) {
		doResetTestScene();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedCamera(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedCamera " << name << ": " << e;

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

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void ofxSurfingPBR::ChangedDisplacers(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedDisplacers " << name << ": " << e;

	//--

	if (name == vResetDisplace.getName()) {
		doResetDisplace();
	} else if (name == vResetNoise.getName()) {
		doResetNoise();
	}

	else if (name == bShaderToPlane.getName()) {
		if (bShaderToPlane) { //workflow
			if (bDisplaceToMaterial) bDisplaceToMaterial = false;
		} else {
		}
	}

	else if (name == bDisplaceToMaterial.getName()) {
		if (bDisplaceToMaterial) { //workflow
			if (bShaderToPlane) bShaderToPlane = false;
		} else { //release mods
			materialFloor.setDisplacementStrength(0);
			materialFloor.setDisplacementNormalsStrength(0);
			materialFloor.setNormalGeomToNormalMapMix(0);
		}
	}

	else if (name == bLimitImage.getName()) {
		//refreshFloorPlane();
		bFlagRefreshFloorPlane = true;
	}
}
#endif

#ifdef SURFING__PBR__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::ChangedCubeMaps(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedCubeMaps " << name << ": " << e;

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

	else if (name == bDrawCubeMap.getName()) {
		if (!bLoadedCubeMap) return; //skip

	//workflow
	#if 1
		if (bDrawCubeMap) {
			if (bg.bDrawBgColorPlain) bg.bDrawBgColorPlain.set(false);
			if (bg.bDrawBgColorObject) bg.bDrawBgColorObject.set(false);
		}
	#endif
	}
}
#endif

//--

//--------------------------------------------------------------
void ofxSurfingPBR::drawTestScene() {
	if (!bGui_DrawTestScene) return;

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
void ofxSurfingPBR::drawFloor() {
	drawPlane();
	drawBoxFloor();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPlane() {
	if (!bDrawFloorPlane) return;

	// plane
	if (bFloorWireframe) {
		floorPlane.drawWireframe();
	}

	//else
	{
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
		if (bShaderToPlane)
			beginShaderPlane();
		else
			beginMaterialFloor();

		{
			floorPlane.draw();
		}

		if (bShaderToPlane)
			endShaderPlane();
		else
			endMaterialFloor();
#else
		beginMaterialFloor();
		{
			floorPlane.draw();
		}
		endMaterialFloor();
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawBoxFloor() {
	if (!bDrawFloorBox) return;

	if (bFloorWireframe) {
		floorBox.drawWireframe();
	}

	//else
	{
		beginMaterialFloor();
		{
			floorBox.draw();
		}
		endMaterialFloor();
	}
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
	ofLogNotice("ofxSurfingPBR") << "Scene save -> " << path;
	ofLogNotice("ofxSurfingPBR") << "Floor save -> " << pathFloor;

	//save scene
	ofxSurfing::saveSettings(parameters, path);
	ofxSurfing::saveSettings(floorParams, pathFloor);
}

//--------------------------------------------------------------
bool ofxSurfingPBR::load() {
	ofLogNotice("ofxSurfingPBR") << "Scene load -> " << path;
	ofLogNotice("ofxSurfingPBR") << "Floor load -> " << pathFloor;

	bool b;

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	//--

	b = ofxSurfing::loadSettings(parameters, path);
	ofxSurfing::loadSettings(floorParams, pathFloor);

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

	//if (bEnableCameraAutosave) doSaveCamera();
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

	//load all: material, bg, lights and camera.
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

	cubeMapParams.setName("CubeMap");

	cubeMapParams.add(bDrawCubeMap);
	cubeMapParams.add(cubeMapMode); //index
	cubeMapParams.add(cubeMapModeName); //display
	cubeMapParams.add(cubeMapExposure);
	cubeMapParams.add(cubeMapprefilterRoughness);
	cubeMapParams.add(cubeMapName);
	cubeMapParams.add(path_CubemapFileAbsPath);
	cubeMapParams.add(vOpenCubeMap);
	cubeMapParams.add(vResetCubeMap);

	parameters.add(cubeMapParams);
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
	if (key == 'G') bGui_ofxGui = !bGui_ofxGui;

	if (key == 'p') bDrawFloorPlane = !bDrawFloorPlane;
	if (key == 'B') bDrawFloorBox = !bDrawFloorBox;

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

	if (key == 'w') bFloorWireframe = !bFloorWireframe;

#ifdef SURFING__PBR__USE_CUBE_MAP
	if (key == 'c') bDrawCubeMap = !bDrawCubeMap;
#endif

	if (key == 'h') bHelp = !bHelp;
	if (key == 'd') bDebug = !bDebug;

	if (key == 'i') bFloorInfinite = !bFloorInfinite;

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
	if (key == 'q') sWindowDimensions = ofxSurfing::setWindowShapeSquared(); // 800
	if (key == 'Q') sWindowDimensions = ofxSurfing::setWindowShapeSquared(ofGetWidth());
	if (key == '1') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(0); // IGTV Cover Photo
	if (key == '2') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(1); // IG Landscape Photo
	if (key == '3') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(2); // IG Portrait
	if (key == '4') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(3); // IG Story
	if (key == '5') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(4); // IG Square

	material.keyPressed(key);
	materialFloor.keyPressed(key);

	buildHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setLogLevel(ofLogLevel logLevel) {
	ofLogNotice("ofxSurfingPBR") << "setLogLevel(" << logLevel << ")";

	ofSetLogLevel("ofxSurfingPBR", logLevel);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetFloor() {
	ofLogNotice("ofxSurfingPBR") << "doResetFloor()";

	doResetFloorTransform();

	materialFloor.doResetMaterial();

	bFloorWireframe = false;

	bDrawFloorBox = true;

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	// displacers
	if (!bLimitImage) bLimitImage.set(true);
	if (bShaderToPlane) bShaderToPlane.set(false);
	if (bDisplaceToMaterial) bDisplaceToMaterial.set(false);

#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetFloorTransform() {
	ofLogNotice("ofxSurfingPBR") << "doResetFloorTransform()";

	floorSize.set(glm::vec2(0.5, 0.2));
	floorResolution.set(glm::vec2(0.01f, 0.01f));
	floorPosition.set(0.f);
	floorRotation.set(0.f);

	floorBoxDepth = 5;

	bFloorSquaredSize = false;
	bFloorInfinite = false;
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
void ofxSurfingPBR::doResetAll(bool bExcludeExtras) {
	ofLogNotice("ofxSurfingPBR") << "doResetAll()";

	bDebug = false;

	// plane
	doResetFloor();

	// cubemap
#ifdef SURFING__PBR__USE_CUBE_MAP
	doResetCubeMap();
#endif

#ifdef SURFING__PBR__USE_LIGHTS_CLASS
	// lights
	if (!bExcludeExtras) lights.doResetAllLights();
	// shadows
	if (!bExcludeExtras) lights.doResetShadow();
#endif

		// shader displacer
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	doResetNoise();
	doResetDisplace();
#endif

	// material
	if (!bExcludeExtras) material.doResetMaterial();

	// bg
	if (!bExcludeExtras) bg.doResetAll();

	// test scene
	doResetTestScene();

	// camera
	doResetCamera();
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

	// Background
	bg.bDrawBgColorPlain.set(true);
	bg.bgColorPlain.set(ofFloatColor(0, 0.03, 0.3, 1));

	materialFloor.doResetMaterial();

	// Material
	material.roughness = 0.5;
	material.reflectance = 0.5;
	material.globalColor.set(ofFloatColor::orange);
}