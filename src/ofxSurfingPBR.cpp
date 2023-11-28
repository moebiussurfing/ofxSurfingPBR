#include "ofxSurfingPBR.h"
//--------------------------------------------------------------
ofxSurfingPBR::ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "ofxSurfingPBR()";

	ofAddListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofAddListener(ofEvents().windowResized, this, &ofxSurfingPBR::windowResized);
}

//--------------------------------------------------------------
ofxSurfingPBR::~ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "~ofxSurfingPBR()";

	ofRemoveListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofRemoveListener(ofEvents().windowResized, this, &ofxSurfingPBR::windowResized);

	//--

	ofRemoveListener(drawParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDraw);
	ofRemoveListener(planeParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedPlane);
	ofRemoveListener(internalParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofRemoveListener(testSceneParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofRemoveListener(cameraParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	ofRemoveListener(displacersParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDisplacers);
#endif

#ifdef SURFING__USE_CUBE_MAP
	ofRemoveListener(cubeMapParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
#endif

	if (!bDoneExit) {
		ofLogWarning("ofxSurfingPBR") << "Force calling exit() bc has not been called until now!";
		exit();
	}
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
		sHelp += "h     Help\n";
		sHelp += "L-l   Layout Help\n";
		//sHelp += "      " + nameHelpLayout.get() + "\n";
		sHelp += "d     Debug\n";
		sHelp += "i     Infinite Plane\n";
		sHelp += "g-G   Gui-ofxGui\n";
		sHelp += "Tab   Layout UI\n";
		//sHelp += "      " + nameGuiLayout.get() + "\n";
		sHelp += "\n";
		sHelp += " DRAW\n";
		sHelp += "p     Plane\n";
		sHelp += "s     Shadow \n";
		sHelp += "c     CubeMap\n";
		sHelp += "b     BgAlt\n";
		sHelp += "\n";
	}
	sHelp += "   WINDOW\n";
	sHelp += "      " + ofToString(ofGetFrameRate(), 1) + " FPS\n";
	sHelp += "      " + ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
	if (bKeys) {
		sHelp += "f     FullScreen\n";
		sHelp += "q-Q   Squared\n";
		sHelp += "1-5   Instagram Sizes\n";
		sHelp += sWindowDimensions + "\n";
	}
	if (bKeys) {
		sHelp += "\n";
		sHelp += "  HELPERS\n";
		sHelp += "  MATERIAL RESET\n";
		sHelp += "F1    Full\n";
		sHelp += "\n";
		sHelp += "  MATERIAL RANDOM\n";
		sHelp += "F2    Full\n";
		sHelp += "F3    Settings\n";
		sHelp += "\n";
		sHelp += "  COLORS\n";
		sHelp += "F4    GlobalNoAlpha\n";
		sHelp += "F5    NoAlpha\n";
		sHelp += "F6    WithAlpha\n";
		sHelp += "F7    OnlyAlphas\n";
		sHelp += "\n";
		sHelp += "   HISTORY BROWSER\n";
		sHelp += "z-x   Prev-Next\n";
		sHelp += "r     Recall\n";
		sHelp += "s     Store\n";
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

	bDrawPlane.set("Draw Plane", true);

	//--

	parameters.setName("PBR_SCENE");

	vLoad.set("Load");
	vSave.set("Save");
	parameters.add(vLoad);
	parameters.add(vSave);

	showGuiParams.setName("UI");
	showGuiParams.add(material.bGui);
	showGuiParams.add(surfingBg.bGui);

#ifdef SURFING__USE_LIGHTS_CLASS
	showGuiParams.add(lights.bGui);
	//showGuiParams.add(lights.bGui_Shadows);
#endif

	parameters.add(showGuiParams);

	drawParams.setName("DRAW");
	drawParams.add(bDrawPlane);

#ifdef SURFING__USE_LIGHTS_CLASS
	drawParams.add(lights.bDrawShadow);
#endif

#ifdef SURFING__USE_CUBE_MAP
	bDrawCubeMap.set("Draw Bg CubeMap", true);
	drawParams.add(bDrawCubeMap);
#endif

	drawParams.add(surfingBg.bDrawObject);
	drawParams.add(surfingBg.bDrawBgColorPlain);
	parameters.add(drawParams);

	//--

	planeParams.setName("Plane");
	planeMaterialParams.setName("Material");
	planeSettingsParams.setName("Settings");
	planeColorsParams.setName("Colors");
	planeTransformParams.setName("Transform");
	internalParams.setName("Internal");
	advancedParams.setName("Advanced");

	//--

	vResetPlane.set("Reset Plane");
	vResetPlaneTransform.set("Reset Transform");
	vResetAll.set("Reset All");

	//--

	bPlaneWireframe.set("Draw Wireframe", false);
	bPlaneInfinite.set("Infinite", true);
	planeSize.set("Size", glm::vec2(0.5f, 0.5f), glm::vec2(0, 0), glm::vec2(1.f, 1.f));
	planeResolution.set("Resolution", glm::vec2(0.5f, 0.5f), glm::vec2(0, 0), glm::vec2(1.f, 1.f));
	planeRotation.set("x Rotation", 0, -45, 135);
	planePosition.set("y Position", 0, -1, 1);
	planeShiness.set("Shiness", 0.85 * SURFING__PBR__MAX_SHININESS, 0, SURFING__PBR__MAX_SHININESS);

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	planeGlobalColor.set("Global Color",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
	planeDiffuseColor.set("Diffuse Color",
		ofFloatColor(0.f), ofFloatColor(0.f), ofFloatColor(1.f));
	planeSpecularColor.set("Specular Color",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	planeGlobalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	planeDiffuseColor.set("Diffuse Color",
		ofFloatColor(0.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	planeSpecularColor.set("Specular Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	planeGlobalColor.setSerializable(false);

	//--

	planeParams.add(bDrawPlane);
	planeParams.add(bPlaneWireframe);

	//--

	planeMaterialParams.add(planeGlobalColor);

	//TODO: add more..
	planeColorsParams.add(planeDiffuseColor);
	planeColorsParams.add(planeSpecularColor);
	planeMaterialParams.add(planeColorsParams);

	planeSettingsParams.add(planeShiness);
	planeMaterialParams.add(planeSettingsParams);

	planeParams.add(planeMaterialParams);

	//--

	planeTransformParams.add(planePosition);
	planeTransformParams.add(planeRotation);
	planeTransformParams.add(planeSize);
	planeTransformParams.add(bPlaneInfinite);
	planeTransformParams.add(planeResolution);
	planeTransformParams.add(vResetPlaneTransform);
	planeParams.add(planeTransformParams);

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	setupParamsDisplace();
#endif

	planeParams.add(vResetPlane);

	//--

	parameters.add(planeParams);

	//--

#ifdef SURFING__USE_CUBE_MAP
	setupCubeMap();
#endif

	//--

	testSceneParams.setName("Test Scene");
	bGui_DrawTestScene.set("Draw Scene", true);
	scaleTestScene.set("Scale", 0, -1.f, 1.f);
	positionTestScene.set("yPos", 0, -1.f, 1.f);
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

	parameters.add(testSceneParams);
	parameters.add(cameraParams);

	//internalParams.add(bDebug);

	//--

	//advancedParams.add(bGui); //workflow: added to add to settings. could hide the toggle..

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	callback_t f = std::bind(&ofxSurfingPBR::save, this);
	autoSaver.setFunctionSaver(f);
#endif

	advancedParams.add(material.getIndexStateParam());
	//advancedParams.add(material.bGuiHelpers);

	guiParams.setName("Gui");
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

	listenerSave = vSave.newListener([this](void) {
		save();
	});

	listenerLoad = vLoad.newListener([this](void) {
		load();
	});

	ofAddListener(drawParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDraw);
	ofAddListener(planeParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedPlane);
	ofAddListener(internalParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofAddListener(testSceneParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofAddListener(cameraParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	ofAddListener(displacersParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDisplacers);
#endif

#ifdef SURFING__USE_CUBE_MAP
	ofAddListener(cubeMapParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
#endif
}

//--

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void ofxSurfingPBR::refreshImgShaderPlane() {
	ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane()";

	int w = plane.getWidth();
	int h = plane.getHeight();

	if (bLimitImage) {
		w = ofClamp(w, 0, SURFING__SHADER_LIMIT_IMAGE_MIN);
		h = ofClamp(h, 0, SURFING__SHADER_LIMIT_IMAGE_MIN);
	} else {
		w = ofClamp(w, 0, SURFING__SHADER_LIMIT_IMAGE_MAX);
		h = ofClamp(h, 0, SURFING__SHADER_LIMIT_IMAGE_MAX);
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
		plane.mapTexCoordsFromTexture(img.getTexture());

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

	planeParams.add(displacersParams);
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
		materialPlane.setDisplacementTexture(img.getTexture());

		materialPlane.setDisplacementStrength(displacementStrength);
		materialPlane.setDisplacementNormalsStrength(displacementNormalsStrength);
		materialPlane.setNormalGeomToNormalMapMix(normalGeomToNormalMapMix);

		//materialPlane.setTexCoordScale(scaleX, scaleY);
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

	surfingBg.setup();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setup() {
	ofLogNotice("ofxSurfingPBR") << "setup() Start";

	// for using on any objects
	material.setup();

	//--

#ifdef SURFING__USE_LIGHTS_CLASS
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

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	setupShaderPlane();
#endif

	//--

	// reset all except material to avoid do it twice and overwrite loaded settings!
	doResetAll(true);

	setupGui();

	buildHelp();

	bDoneSetup = true;
	ofLogNotice("ofxSurfingPBR") << "setup() Done!";

	//--

	startup();
}

// Camera
//--------------------------------------------------------------
void ofxSurfingPBR::setup(ofCamera & camera_) {
	this->setup();

	setCameraPtr(dynamic_cast<ofCamera *>(&camera_));
}
//--------------------------------------------------------------
void ofxSurfingPBR::setCameraPtr(ofCamera & camera_) {
	setCameraPtr(dynamic_cast<ofCamera *>(&camera_));
}
//--------------------------------------------------------------
void ofxSurfingPBR::setCameraPtr(ofCamera * camera_) {
	camera = camera_;

	//FORCED
	camera->setFarClip(SURFING__SCENE_CAMERA_FAR);
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
#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
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
		ofLogError("ofxSurfingPBR") << "setFunctionRenderScene(). Wrong callback_t";
	}
	f_RenderScene = f;

#ifdef SURFING__USE_LIGHTS_CLASS
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

	if (!this->getSettingsFileFound()) {
		ofLogWarning("ofxSurfingPBR") << "Initialize settings for the first time!";
		ofLogWarning("ofxSurfingPBR") << "Potential Newbie User Found!";
		ofLogWarning("ofxSurfingPBR") << "Forcing states and some default stuff visible:";
		ofLogWarning("ofxSurfingPBR") << "Enabled help, debug, reset camera and settings, etc...";

		//--

		//#if 0
		//		// Initialize a Default Startup Scene!
		//		{
		//			//// Background
		//			//bDrawBgColorPlain.set(true);
		//			//bgColorPlain.set(ofFloatColor(0, 0.03, 0.3, 1));
		//
		//			// Plane
		//			planeGlobalColor.set(ofFloatColor(0.25, 0, 0.5, 1));
		//
		//			// Material
		//			material.roughness = 0.5;
		//			material.reflectance = 0.5;
		//			material.globalColor.set(ofFloatColor::orange);
		//		}
		//#endif

		//--

		// Force enable Help and Debug states!
		// Ready to a newbie user!
		// All important/learning controls will be visible.
		{
			bGui = true;
			bGui_ofxGui = true;
			bHelp = true;
			bDebug = true;
		}

		//--

		//TODO: not works
		//#ifdef SURFING__USE_LIGHTS_CLASS
		//		lights.bDebug.makeReferenceTo(bDebug);
		//#endif
	}

	//--

	//if (!this->getSettingsFileFoundForCamera()) {
	//	doResetCamera();
	//}

	//--

	load();

	bDoneStartup = true;
	ofLogNotice("ofxSurfingPBR") << "startup() Done! at frame number: " << ofGetFrameNum();
}

//// Will be called on the first update frame.
///	Use this pattern carefully, as could be a bad practice in some scenarios!
//--------------------------------------------------------------
void ofxSurfingPBR::startupDelayed() {
	ofLogNotice("ofxSurfingPBR") << "startupDelayed() Start";

	if (!this->getSettingsFileFoundForCamera()) {
		doResetCamera();
	} else if (camera != nullptr)
		doLoadCamera();

//TODO fix crash callbacks
// some bypassed callbacks that makes the app crash...
#if 0
	guiLayout = guiLayout.get(); //trig crashes..
#else
	if (guiLayout.get() == 0)
		nameGuiLayout.set(namesGuiLayouts[0]);
	else if (guiLayout.get() == 1)
		nameGuiLayout.set(namesGuiLayouts[1]);
#endif

	string s = ofxSurfing::getLayoutName(helpLayout.get());
	nameHelpLayout.set(s);

	bDoneStartupDelayed = true;
	ofLogNotice("ofxSurfingPBR") << "startupDelayed() Done! at frame number: " << ofGetFrameNum();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupGui() {
	gui.setup(parameters);

	static ofEventListener listenerSave;
	static ofEventListener listenerLoad;
	listenerSave = gui.savePressedE.newListener([this] {
		save();
	});
	listenerLoad = gui.loadPressedE.newListener([this] {
		load();
	});

	//TOOO: remove the button to include only in settings..?
	//gui.getGroup(internalParams.getName()).getButton(bGui.getName()).setSize(5, 6);

	refreshGui();
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "refreshGui()";

	// top-left
	gui.setPosition(SURFING__PAD_TO_WINDOW_BORDERS, SURFING__PAD_TO_WINDOW_BORDERS);

	// minimize sub panels

	gui.getGroup(planeParams.getName())
		.getGroup(planeTransformParams.getName())
		.minimize();

	gui.getGroup(planeParams.getName())
		.getGroup(planeTransformParams.getName())
		.getGroup(planeSize.getName())
		.minimize();

	gui.getGroup(planeParams.getName())
		.getGroup(planeTransformParams.getName())
		.getGroup(planeResolution.getName())
		.minimize();

	gui.getGroup(planeParams.getName())
		.getGroup(planeMaterialParams.getName())
		.getGroup(planeSettingsParams.getName())
		.minimize();

	gui.getGroup(planeParams.getName())
		.getGroup(planeMaterialParams.getName())
		.getGroup(planeColorsParams.getName())
		.minimize();

	//gui.getGroup(lightParams.getName()).minimize();
	//gui.getGroup(shadowParams.getName()).minimize();
	//gui.getGroup(bgColorPlainParams.getName()).minimize();
	gui.getGroup(testSceneParams.getName()).minimize();
	gui.getGroup(cameraParams.getName()).minimize();
	gui.getGroup(internalParams.getName()).minimize();

	gui.getGroup(internalParams.getName())
		.getGroup(advancedParams.getName())
		.minimize();

#ifdef SURFING__USE_CUBE_MAP
	gui.getGroup(cubeMapParams.getName()).minimize();
#endif

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	gui.getGroup(planeParams.getName())
		.getGroup(displacersParams.getName())
		.minimize();

	gui.getGroup(planeParams.getName())
		.getGroup(displacersParams.getName())
		.getGroup(displaceMaterialParams.getName())
		.minimize();

	gui.getGroup(planeParams.getName())
		.getGroup(displacersParams.getName())
		.getGroup(noiseParams.getName())
		.minimize();
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

	if (bFlagRefreshPlane) {
		bFlagRefreshPlane = false;

		refreshPlane();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::updatePBRScene() {

	//--

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDisplaceToMaterial || bShaderToPlane) {
		updateDisplace();
	}
#endif
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
void ofxSurfingPBR::beginMaterialPlane() {
	materialPlane.begin();
}
//--------------------------------------------------------------
void ofxSurfingPBR::endMaterialPlane() {
	materialPlane.end();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawOfxGui() {

	if (bGui_ofxGui) {
		gui.draw();

		//--

		if (material.bGui) {
			// Force position for material gui
			glm::vec3 p;
#if 1
			p = gui.getShape().getTopRight() + glm::vec2(SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0);
#else
			if (isWindowPortrait()) {
				p = gui.getShape().getBottomLeft() + glm::vec2(0, SURFING__PAD_OFXGUI_BETWEEN_PANELS);
			} else {
				p = gui.getShape().getTopRight() + glm::vec2(SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0);
			}
#endif
			material.setGuiPosition(p);

			material.drawGui();
		}

		//--

		if (surfingBg.bGui) {
			glm::vec2 p;
			if (material.bGui)
				p = material.gui.getShape().getTopRight();
			else
				p = gui.getShape().getTopRight();
			p += glm::vec2 { (float)SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0.f };
			surfingBg.setGuiPosition(p);

			surfingBg.drawGui();
		}

		//--

#ifdef SURFING__USE_LIGHTS_CLASS
		if (lights.bGui) {
			glm::vec2 p;
			if (surfingBg.bGui)
				p = surfingBg.gui.getShape().getTopRight();
			else if (material.bGui)
				p = material.gui.getShape().getTopRight();
			else
				p = gui.getShape().getTopRight();
			p += glm::vec2 { (float)SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0.f };
			lights.setGuiPosition(p);

			lights.drawGui();
		}
#endif
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
#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	if (!bShaderToPlane && !bDisplaceToMaterial)
#endif
	{
		string s = "";

		float fps = ofGetFrameRate();
		s += ofToString(fps, 1);
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

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	// Shader and displacer info
	if (bShaderToPlane || bDisplaceToMaterial) {
		// layout is responsive

		int x, y, w, h;

		float r = img.getHeight() / (float)img.getWidth();

		// info
		string s = "";
		s += " \n";
		s += "DEBUG\nSHADER\n\n";
		s += "PLANE\n";
		s += "Size: ";
		s += ofToString(plane.getWidth(), 0) + "," + ofToString(plane.getHeight(), 0) + "\n";
		s += "Reso: ";
		s += ofToString(plane.getResolution().x) + "," + ofToString(plane.getResolution().y) + "\n\n";
		s += "IMAGE\n";
		s += "Size: ";
		s += ofToString(img.getWidth()) + "," + ofToString(img.getHeight()) + "\n\n";
		s += ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
		s += ofToString(ofGetFrameRate(), 1) + " FPS";

		ofxSurfing::SURFING_LAYOUT l;
		l = ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;

		//TODO
		// Set the preview box position related to help box layouot.

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
	surfingBg.draw();
}

//--------------------------------------------------------------
void ofxSurfingPBR::draw() {
	if (f_RenderScene == nullptr) return;

	ofEnableDepthTest();

	//--

	updatePBRScene();

	drawPBRScene();

	//--

	// camera
	camera->begin();
	{

#ifdef SURFING__USE_LIGHTS_CLASS
		lights.begin();
#endif

		drawBg();

		//----

		glEnable(GL_CULL_FACE);

		glFrontFace(GL_CW);
		// Maybe should fix bc makes some models "transparent"...
		// sets the orientation for front-facing
		// polygons1GL_CW means that polygons with vertices
		// in clockwise order on the screen are considered front-facing1.

		glCullFace(GL_BACK);
		{
			f_RenderScene();
		}
		glDisable(GL_CULL_FACE);

		//----

#ifdef SURFING__USE_LIGHTS_CLASS
		lights.end();
#endif

		//--

		//ofDisableDepthTest();

		drawPBRSceneDebug();
	}
	camera->end();
}

#ifdef SURFING__USE_CUBE_MAP
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

#ifdef SURFING__USE_LIGHTS_CLASS
	lights.updateLights();
#endif

	//--

#ifdef SURFING__USE_CUBE_MAP
	updateCubeMap();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPBRSceneDebug() {
	if (!bGui) return;

#ifdef SURFING__USE_LIGHTS_CLASS
	// Debug lights and shadows
	lights.drawDebugLights();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshPlane() {

	int w, h;

	if (bPlaneInfinite) {
		// size hardcoded to a safety max!
		w = SURFING__PLANE_SIZE_INFINITE_MODE;
		h = SURFING__PLANE_SIZE_INFINITE_MODE;
	} else {
		// size from normalized param multiplied by a unit
		int planeSizeUnit = SURFING__SCENE_SIZE_UNIT * SURFING__PLANE_SIZE_MULTIPLIER;
		w = planeSize.get().x * planeSizeUnit;
		h = planeSize.get().y * planeSizeUnit;
	}

	//--

	int xResolution;
	int yResolution;

	xResolution = (int)ofMap(planeResolution.get().x, 0.f, 1.f,
		(int)SURFING__PLANE_RESOLUTION_MIN, (int)SURFING__PLANE_RESOLUTION_MAX, true);

	yResolution = (int)ofMap(planeResolution.get().y, 0.f, 1.f,
		(int)SURFING__PLANE_RESOLUTION_MIN, (int)SURFING__PLANE_RESOLUTION_MAX, true);

	plane.set(w, h, xResolution, yResolution);

	//--

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDisplaceToMaterial || bShaderToPlane)
		refreshImgShaderPlane();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedPlane(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedPlane " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	if (name == planeSize.getName()) {
		static glm::vec2 planeSize_ = glm::vec2(-1, -1);
		if (planeSize.get() != planeSize_) { // if changed
			planeSize_ = planeSize.get();

			bFlagRefreshPlane = true;
		} else {
			ofLogVerbose("ofxSurfingPBR") << "Plane size not Changed. Skipped refresh!";
		}
	}

	else if (name == planeResolution.getName()) {
		bFlagRefreshPlane = true;
	}

	else if (name == bPlaneInfinite.getName()) {
		bFlagRefreshPlane = true;
	}

	else if (name == planeRotation.getName()) {
		glm::vec3 axis(1.0f, 0.0f, 0.0f);
		float angle = planeRotation.get() - 90;
		glm::quat q = glm::angleAxis(glm::radians(angle), axis);
		plane.setOrientation(q);
	}

	else if (name == planePosition.getName()) {
		plane.setPosition(0, planePosition.get() * SURFING__SCENE_SIZE_UNIT * 5.f, 0);
	}

	else if (name == planeShiness.getName()) {
		materialPlane.setShininess(planeShiness.get());
	}

	else if (name == planeGlobalColor.getName()) {
		//fix crash
		if (!bDoneStartup) return;

		planeDiffuseColor.set(planeGlobalColor.get());
		planeSpecularColor.set(planeGlobalColor.get());
	}

	else if (name == planeDiffuseColor.getName()) {
		materialPlane.setDiffuseColor(planeDiffuseColor.get());
	}

	else if (name == planeSpecularColor.getName()) {
		materialPlane.setSpecularColor(planeSpecularColor.get());
	}

	else if (name == vResetPlane.getName()) {
		doResetPlane();
	}

	else if (name == vResetPlaneTransform.getName()) {
		doResetPlaneTransform();
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

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

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

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	//workflow
	if (name == surfingBg.bDrawBgColorPlain.getName() && surfingBg.bDrawBgColorPlain.get()) {
		if (bDrawCubeMap) bDrawCubeMap.set(false);
	}

	else if (name == surfingBg.bDrawObject.getName() && surfingBg.bDrawObject.get()) {
		if (bDrawCubeMap) bDrawCubeMap.set(false);
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedTestScene(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedTestScene " << name << ": " << e;

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

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

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	if (name == vSaveCamera.getName()) {
		doSaveCamera();
	} else if (name == vLoadCamera.getName()) {
		doLoadCamera();
	} else if (name == vResetCamera.getName()) {
		doResetCamera();
	}
}

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void ofxSurfingPBR::ChangedDisplacers(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedDisplacers " << name << ": " << e;

	//--

	#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
	#endif

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
			materialPlane.setDisplacementStrength(0);
			materialPlane.setDisplacementNormalsStrength(0);
			materialPlane.setNormalGeomToNormalMapMix(0);
		}
	}

	else if (name == bLimitImage.getName()) {
		//refreshPlane();
		bFlagRefreshPlane = true;
	}
}
#endif

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::ChangedCubeMaps(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedCubeMaps " << name << ": " << e;

	//--

	#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
	#endif

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
			if (surfingBg.bDrawBgColorPlain) surfingBg.bDrawBgColorPlain.set(false);
			if (surfingBg.bDrawObject) surfingBg.bDrawObject.set(false);
		}
	#endif
	}

	////#ifdef SURFING__USE_CUBE_MAP
	////	if (name == surfingBg.bDrawBgColorPlain.getName()) {
	////		if (!bLoadedCubeMap) return; //skip
	////		//workflow
	////		if (surfingBg.bDrawBgColorPlain)
	////			if (bDrawCubeMap) bDrawCubeMap = false;
	////	}
	////#endif
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::drawTestScene() {
	if (!bGui_DrawTestScene) return;

	//----

	ofPushStyle();
	ofSetConeResolution(50, 10, 2);
	ofSetSphereResolution(50);

	//--

	pushTestSceneTRansform();

	{
		float u = SURFING__SCENE_SIZE_UNIT / 10.f;

		// Cone
		ofPushMatrix();
		ofTranslate(-2 * u, u, 0);
		ofRotateXDeg(180);
		ofDrawCone(0, 0, 0, u * 0.65f, u);
		ofPopMatrix();

		// Box
		ofPushMatrix();
		float spd = 240;
		ofRotateYDeg(360.f * (ofGetFrameNum() % (int)spd) / spd);
		ofDrawBox(0, u, 0, u);
		ofPopMatrix();

		// Sphere
		ofDrawSphere(2 * u, u, 0, u / 2.f);
	}

	popTestSceneTRansform();

	ofPopStyle();
}

//--------------------------------------------------------------
void ofxSurfingPBR::pushTestSceneTRansform() {
#if (DO_SCENE_TEST_TRANSFORMS)
	// Scene transforms
	ofPushMatrix();

	// Position
	float yUnit = SURFING__SCENE_SIZE_UNIT / 2.f;
	float y = ofMap(positionTestScene, -1.f, 1.f,
		-yUnit, yUnit, true);

	// Scale
	float s = ofMap(scaleTestScene, -1.f, 1.f,
		1.f / SURFING__SCENE_TEST_UNIT_SCALE, SURFING__SCENE_TEST_UNIT_SCALE, true);

	ofTranslate(0, y, 0);
	ofScale(s);
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::popTestSceneTRansform() {
#if DO_SCENE_TEST_TRANSFORMS
	ofPopMatrix();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPlane() {
	if (!bDrawPlane) return;

	// plane
	if (bPlaneWireframe) {
		plane.drawWireframe();
	}

	else {
#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
		if (bShaderToPlane)
			beginShaderPlane();
		else
			beginMaterialPlane();
		{
			plane.draw();
		}
		if (bShaderToPlane)
			endShaderPlane();
		else
			endMaterialPlane();
#else
		beginMaterialPlane();
		{
			plane.draw();
		}
		endMaterialPlane();
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::exit() {
	ofLogNotice("ofxSurfingPBR") << "exit()";

// Not required to be called bc it's using the auto saver!
#if defined(SURFING__USE_AUTOSAVE_FORCE_ON_EXIT) || !defined(SURFING__USE_AUTOSAVE_SETTINGS_ENGINE)
	save();

	material.exit();
	surfingBg.exit();

	#ifdef SURFING__USE_LIGHTS_CLASS
	lights.exit();
	#endif
#endif

	if (bEnableCameraAutosave) doSaveCamera();

	bDoneExit = true;
}

//--------------------------------------------------------------
void ofxSurfingPBR::save() {
	ofLogNotice("ofxSurfingPBR") << "save -> " << path;

	ofxSurfing::saveSettings(parameters, path);
}

//--------------------------------------------------------------
void ofxSurfingPBR::load() {
	ofLogNotice("ofxSurfingPBR") << "load -> " << path;

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	//--

	ofxSurfing::loadSettings(parameters, path);

	//CubeMap
	loadCubeMap(path_CubemapFileAbsPath.get());

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFoundForScene() {

	// search for the mandatory settings file to consider if the app is opened for the first time.
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

	//not required, but we log if it's located or not.
	getSettingsFileFoundForMaterial();

	//not required, but we log if it's located or not.
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

#ifdef SURFING__USE_CUBE_MAP
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

		cubeMapMode = cubeMapMode; //refresh

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

	cubeMapModeName.set("TypeC", "NONE");
	cubeMapMode.set("ModeC", 2, 1, 3);
	cubeMapName.set("NameC", "NONE");
	path_CubemapFileAbsPath.set("Path CubeMap", "NONE");
	cubeMapprefilterRoughness.set("Roughness CubeMap", 0.25f, 0, 1.f);
	vOpenCubeMap.set("Open File");
	vResetCubeMap.set("Reset CubeMap");

	cubeMapName.setSerializable(false);
	cubeMapModeName.setSerializable(false);

	//--

	cubeMapParams.setName("CubeMap");

	cubeMapParams.add(bDrawCubeMap);
	cubeMapParams.add(cubeMapMode); //index
	cubeMapParams.add(cubeMapModeName); //display
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
void ofxSurfingPBR::keyPressed(int key) {
	if (!bKeys) return;
	ofLogNotice("ofxSurfingPBR") << "keyPressed(" << key << ")";

	if (key == 'g') bGui = !bGui;
	if (key == 'G') bGui_ofxGui = !bGui_ofxGui;

	if (key == 'p') bDrawPlane = !bDrawPlane;

#ifdef SURFING__USE_LIGHTS_CLASS
	if (key == 's') lights.bDrawShadow = !lights.bDrawShadow;
#endif

	if (key == 'b') surfingBg.bDrawBgColorPlain = !surfingBg.bDrawBgColorPlain;
	if (key == 'w') bPlaneWireframe = !bPlaneWireframe;

#ifdef SURFING__USE_CUBE_MAP
	if (key == 'c') bDrawCubeMap = !bDrawCubeMap;
#endif

	if (key == 'h') bHelp = !bHelp;
	if (key == 'd') bDebug = !bDebug;

	if (key == 'i') bPlaneInfinite = !bPlaneInfinite;

	if (key == OF_KEY_TAB) doNextLayouGui(); //next layout gui

	if (key == 'l') doNextLayoutHelp(); //next layout help
	if (key == 'L') doPrevLayoutHelp(); //prev layout help

	if (key == 'f') ofToggleFullscreen();
	if (key == 'q') sWindowDimensions = ofxSurfing::setWindowShapeSquared(); // 800
	if (key == 'Q') sWindowDimensions = ofxSurfing::setWindowShapeSquared(ofGetWidth());
	if (key == '1') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(0); // IGTV Cover Photo
	if (key == '2') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(1); // IG Landscape Photo
	if (key == '3') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(2); // IG Portrait
	if (key == '4') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(3); // IG Story
	if (key == '5') sWindowDimensions = ofxSurfing::setWindowShapeForInstagram(4); // IG Square

	// Randomizers
	if (key == OF_KEY_F1) doResetMaterial();
	if (key == OF_KEY_F2) doRandomMaterial();
	if (key == OF_KEY_F3) doRandomMaterialSettings();
	if (key == OF_KEY_F4) doRandomMaterialColorGlobal();
	if (key == OF_KEY_F5) doRandomMaterialColors();
	if (key == OF_KEY_F6) doRandomMaterialColorsAlpha();
	if (key == OF_KEY_F7) doRandomMaterialAlphas();

	// History
	if (key == 'z') material.doPrevHistory();
	if (key == 'x') material.doNextHistory();
	if (key == 'r') material.doRecallState();
	if (key == 's') material.doStoreNewState();
	if (key == 'u') material.doSaveState();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setLogLevel(ofLogLevel logLevel) {
	ofLogNotice("ofxSurfingPBR") << "setLogLevel(" << logLevel << ")";

	ofSetLogLevel("ofxSurfingPBR", logLevel);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlane() {
	ofLogNotice("ofxSurfingPBR") << "doResetPlane()";

	doResetPlaneTransform();

	planeShiness.set(0.85 * SURFING__PBR__MAX_SHININESS);

	ofFloatColor c = ofFloatColor(0.5f, 1.f);
	planeGlobalColor.set(c);

	bPlaneWireframe = false;

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	// displacers
	if (!bLimitImage) bLimitImage.set(true);
	if (bShaderToPlane) bShaderToPlane.set(false);
	if (bDisplaceToMaterial) bDisplaceToMaterial.set(false);
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlaneTransform() {
	ofLogNotice("ofxSurfingPBR") << "doResetPlaneTransform()";

	planeSize.set(glm::vec2(0.12, 0.05));
	planeResolution.set(glm::vec2(0.f, 0.f));
	planePosition.set(0.f);
	planeRotation.set(0.f);
	//planeRotation.set(10.f);
	bPlaneInfinite = false;
}

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::doResetCubeMap() {
	ofLogNotice("ofxSurfingPBR") << "doResetCubeMap()";

	cubeMapMode = 2;
	cubeMapprefilterRoughness = 0.25f;
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::doLoadCamera() {
	ofLogNotice("ofxSurfingPBR") << "doLoadCamera()";

	ofxLoadCamera(*camera, pathCamera);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doSaveCamera() {
	ofLogNotice("ofxSurfingPBR") << "doSaveCamera()";

	ofxSaveCamera(*camera, pathCamera);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetCamera() {
	ofLogNotice("ofxSurfingPBR") << "doResetCamera()";

	ofEasyCam * easyCam = dynamic_cast<ofEasyCam *>(camera);
	if (easyCam != nullptr) {
		easyCam->reset();

		camera->setFarClip(SURFING__SCENE_CAMERA_FAR);

		easyCam->setPosition({ 8.35512, 581.536, 696.76 });
		easyCam->setOrientation({ 0.940131, -0.340762, 0.00563653, 0.00204303 });

		// save
		//vSaveCamera.trigger();
		doSaveCamera();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetAll(bool bExcludeExtras) {
	ofLogNotice("ofxSurfingPBR") << "doResetAll()";

	bDebug = false;

	// plane
	doResetPlane();

	// cubemap
#ifdef SURFING__USE_CUBE_MAP
	doResetCubeMap();
#endif

#ifdef SURFING__USE_LIGHTS_CLASS
	// lights
	if (!bExcludeExtras) lights.doResetAllLights();
	// shadows
	if (!bExcludeExtras) lights.doResetShadow();
#endif

		// shader displacer
#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	doResetNoise();
	doResetDisplace();
#endif

	// material
	if (!bExcludeExtras) material.doResetMaterial();

	// bg
	if (!bExcludeExtras) surfingBg.doResetAll();

	// test scene
	vResetTestScene.trigger();

	// camera
	vResetCamera.trigger();
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetTestScene() {
	ofLogNotice("ofxSurfingPBR") << "doResetTestScene()";

	scaleTestScene = -0.6;
	positionTestScene = 0;
}

//--

//--------------------------------------------------------------
void ofxSurfingPBR::doResetMaterial() {
	ofLogNotice("ofxSurfingPBR") << "doResetMaterial()";

	material.doResetMaterial();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterial() {
	ofLogNotice("ofxSurfingPBR") << "doRandomMaterial()";

	material.doRandomMaterial();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialSettings() {
	ofLogNotice("ofxSurfingPBR") << "doRandomMaterialSettings()";

	material.doRandomSettings();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColorGlobal() {
	ofLogNotice("ofxSurfingPBR") << "doRandomMaterialColorGlobal()";

	material.doRandomColorGlobal();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColors() {
	ofLogNotice("ofxSurfingPBR") << "doRandomMaterialColors()";

	material.doRandomColors();
	material.doStoreNewState();
};
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColorsAlpha() {
	ofLogNotice("ofxSurfingPBR") << "doRandomMaterialColorsAlpha()";

	material.doRandomColorsAlpha();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialAlphas() {
	ofLogNotice("ofxSurfingPBR") << "doRandomMaterialAlphas()";

	material.doRandomAlphas();
	material.doStoreNewState();
}