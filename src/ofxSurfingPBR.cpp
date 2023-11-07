#include "ofxSurfingPBR.h"
//--------------------------------------------------------------
ofxSurfingPBR::ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "constructor()";

	ofAddListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofAddListener(ofEvents().windowResized, this, &ofxSurfingPBR::windowResized);
}

//--------------------------------------------------------------
ofxSurfingPBR::~ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "destructor()";

	ofRemoveListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofRemoveListener(ofEvents().windowResized, this, &ofxSurfingPBR::windowResized);

	//--

	ofRemoveListener(planeParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedPlane);
	ofRemoveListener(lightParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedLight);
	ofRemoveListener(shadowParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedShadow);
	ofRemoveListener(internalParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofRemoveListener(testSceneParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofRemoveListener(cameraParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);
	ofRemoveListener(backgroundParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedBg);

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	ofRemoveListener(displacersParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDisplacers);
#endif

#ifdef SURFING__USE_CUBE_MAP
	ofRemoveListener(cubeMapParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::windowResized(ofResizeEventArgs & resize) {
	glm::vec2 sz { resize.width, resize.height };
	static glm::vec2 sz_ { -1, -1 };
	if (sz != sz_) {
		sz_ = sz;
		ofLogNotice("ofxSurfingPBR") << "windowResized() Size: " << sz_.x << "," << sz_.y;

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
		sHelp += "h   Help\n";
		sHelp += "d   Debug\n";
		sHelp += "i   Infinite Plane\n";
		sHelp += "g   Gui\n";
		sHelp += "G   ofxGui\n";
		sHelp += "TAB Layout Gui\n";
		sHelp += "l   Layout Help\n";
		sHelp += "\n";
		sHelp += "    DRAW\n";
		sHelp += "p   Plane\n";
		sHelp += "s   Shadow \n";
		sHelp += "c   CubeMap\n";
		sHelp += "b   BgAlt\n";
		sHelp += "\n";
	}
	sHelp += "    WINDOW\n";
	if (bKeys) {
		sHelp += "f   FullScreen\n";
		sHelp += "q   Squared\n";
	}
	sHelp += "    " + ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
	sHelp += "    " + ofToString(ofGetFrameRate(), 1) + " FPS\n";
	if (bKeys) {
		sHelp += "\n";
		sHelp += "HELPERS\n";
		sHelp += "\n";
		sHelp += "    MATERIAL\n    RESET\n";
		sHelp += "F1  Full\n";
		sHelp += "\n";
		sHelp += "    MATERIAL\n    RANDOM\n";
		sHelp += "F2  Full\n";
		sHelp += "F3  Settings\n";
		sHelp += "    COLORS\n";
		sHelp += "F4  GlobalNoAlpha\n";
		sHelp += "F5  NoAlpha\n";
		sHelp += "F6  WithAlpha\n";
		sHelp += "F7  OnlyAlphas\n";
		sHelp += "\n";
		sHelp += "    HISTORY\n";
		sHelp += "z   Prev\n";
		sHelp += "x   Next\n";
		sHelp += "r   Recall\n";
		sHelp += "s   Store\n";
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::doNextLayoutHelp() {
	ofLogNotice("ofxSurfingPBR") << "doNextLayoutHelp()";

	int i = (int)helpLayout.get();
	i++;
	i = i % (int)(helpLayout.getMax()+1);
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
	nameGuiLayout.setSerializable(false);

	int imax =(int)ofxSurfing::SURFING_LAYOUT_AMOUNT-1;
	int idef =(int)ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;
	helpLayout.set("Help Layout", idef, 0, imax);
	nameHelpLayout.set("Help LN", "NONE");
	nameHelpLayout.setSerializable(false);

	//--

	parameters.setName("PBR_Scene");

	//--

	bDrawPlane.set("Draw Plane", true);
	bDrawShadow.set("Draw Shadow", true);
	bDrawBg.set("Draw Bg", false);

	parameters.add(bDrawPlane);
	parameters.add(bDrawShadow);

#ifdef SURFING__USE_CUBE_MAP
	bDrawCubeMap.set("Draw CubeMap", true);
	parameters.add(bDrawCubeMap);
#endif
	parameters.add(bDrawBg);

	parameters.add(material.bGui);

	//--

	planeParams.setName("Plane");
	planeMaterialParams.setName("Material");
	planeSettingsParams.setName("Settings");
	planeColorsParams.setName("Colors");
	planeTransformParams.setName("Transform");
	lightParams.setName("Light");
	shadowParams.setName("Shadow");
	internalParams.setName("Internal");
	advancedParams.setName("Advanced");

	//--

	vResetPlane.set("Reset Plane");
	vResetPlaneTransform.set("Reset Transform");
	vResetLight.set("Reset Light");
	vResetShadow.set("Reset Shadow");
	vResetAll.set("Reset All");

	//--

	bPlaneWireframe.set("Draw Wireframe", false);
	bPlaneInfinite.set("Infinite", true);
	planeSize.set("Size", glm::vec2(0.8f, 0.8f), glm::vec2(0, 0), glm::vec2(1.f, 1.f));
	planeResolution.set("Resolution",
		glm::ivec2(SURFING__PLANE_RESOLUTION, SURFING__PLANE_RESOLUTION),
		glm::ivec2(1, 1),
		glm::ivec2(SURFING__PLANE_RESOLUTION_MAX, SURFING__PLANE_RESOLUTION_MAX));
	planeRotation.set("x Rotation", 0, -45, 135);
	planePosition.set("y Position", 0, -1, 1);
	planeShiness.set("Shiness", 0.85, 0, 1);

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

	planeSettingsParams.add(planeShiness);
	planeMaterialParams.add(planeSettingsParams);

	//TODO: add more..
	planeColorsParams.add(planeDiffuseColor);
	planeColorsParams.add(planeSpecularColor);
	planeMaterialParams.add(planeColorsParams);

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

	lightAmbientColor.set("Light Ambient Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	float u = 1.5 * SURFING__SCENE_SIZE_UNIT;
	lightParams.add(lightX.set("X", 0.0f, -u, u));
	lightParams.add(lightY.set("Y", 0.0f, -u, u));
	lightParams.add(lightZ.set("Z", 0.0f, -u, u));
	lightParams.add(lightAmbientColor);
	lightParams.add(vResetLight);

	//--

	shadowParams.add(bDrawShadow);
	shadowParams.add(shadowBias.set("Bias", 0.07, 0.0, 1.0));
	shadowParams.add(shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0));

	//TODO
	//shadowParams.add(shadowStrength.set("Strength", 0.6f, 0.f, 1.f));
	//shadowParams.add(shadowSize.set("Shadow Size", glm::vec2(0.25f, 0.25f), glm::vec2(0, 0), glm::vec2(1.f, 1.f)));

	shadowParams.add(bDebugShadow.set("Debug Shadow", false));
	shadowParams.add(vResetShadow);

	//--

	parameters.add(planeParams);
	parameters.add(lightParams);
	parameters.add(shadowParams);

	//--

#ifdef SURFING__USE_CUBE_MAP
	setupCubeMap();
#endif

	backgroundParams.setName("Background");
	bgColor.set("Bg Color", ofFloatColor::darkGrey, ofFloatColor(0.f), ofFloatColor(1.f));
	backgroundParams.add(bDrawBg);
	backgroundParams.add(bgColor);
	parameters.add(backgroundParams);

	//--

	testSceneParams.setName("Test Scene");
	scaleTestScene.set("Scale", 0, -1.f, 1.f);
	positionTestScene.set("yPosition", 0, -1.f, 1.f);
	vResetTestScene.set("Reset TestScene");
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

	internalParams.add(bDebug);

	//--

	//advancedParams.add(bGui); //workflow: added to add to settings. could hide the toggle..

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	// auto saver
	callback_t f = std::bind(&ofxSurfingPBR::save, this);
	//register the local save function to be called when saving is required.
	autoSaver.setFunctionSaver(f);
	advancedParams.add(autoSaver.bEnable);
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
	advancedParams.add(guiParams);

	internalParams.add(advancedParams);

	internalParams.add(bKeys);
	parameters.add(internalParams);

	parameters.add(vResetAll);
	parameters.add(bHelp);

	//--

	listenerResetAll = vResetAll.newListener([this](void) {
		doResetAll();
	});

	ofAddListener(planeParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedPlane);
	ofAddListener(lightParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedLight);
	ofAddListener(shadowParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedShadow);
	ofAddListener(internalParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedInternal);
	ofAddListener(testSceneParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedTestScene);
	ofAddListener(cameraParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCamera);
	ofAddListener(backgroundParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedBg);

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	ofAddListener(displacersParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedDisplacers);
#endif

#ifdef SURFING__USE_CUBE_MAP
	ofAddListener(cubeMapParams.parameterChangedE(), this, &ofxSurfingPBR::ChangedCubeMaps);
#endif

	//--

	bDoneSetupParams = true;
}

//--

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void ofxSurfingPBR::refreshImgShaderPlane() {
	ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane()";

	int w = plane.getWidth();
	int h = plane.getHeight();

	if (bLimitImage) {
		w = ofClamp(w, 0, SURFING__SHADER_LIMIT_IMAGE);
		h = ofClamp(h, 0, SURFING__SHADER_LIMIT_IMAGE);
	} else { //ma
		w = ofClamp(w, 0, SURFING__SHADER_LIMIT_IMAGE_MAX);
		h = ofClamp(h, 0, SURFING__SHADER_LIMIT_IMAGE_MAX);
	}

	//reduce a bit image re allocations calls when not required
	//bc sizes not changed..
	static int w_ = -1;
	static int h_ = -1;
	if (w != w_ || h != h_) { //if changed
		w_ = w;
		h_ = h;

	#ifdef TARGET_OPENGLES
		OPENGL ES supports GL_RGBA32F but not GL_R32F
			img.allocate(80, 60, OF_IMAGE_COLOR_ALPHA);
	#else
		//img.clear();
		img.allocate(w, h, OF_IMAGE_GRAYSCALE);
		ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane() Allocated img: " << w << "," << h;
	#endif

		//apply to plane
		plane.mapTexCoordsFromTexture(img.getTexture());

		ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane() w,h: " << w << "," << h;
	}

	else {
		ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane() Skipped img.allocate (" << w << "," << h << ")";
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

	noiseAmplitude.set(1.5f);
	noiseScale.set(0.05f);
	noiseSpeed.set(0.5f);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetDisplace() {
	ofLogNotice("ofxSurfingPBR") << "doResetDisplace()";

	displacementStrength.set(displacementStrength.getMax() * 0.75);
	displacementNormalsStrength.set(displacementNormalsStrength.getMax() / 2);
	normalGeomToNormalMapMix.set(normalGeomToNormalMapMix.getMax() / 2);

	bShaderToPlane.set(false);
	bDisplaceToMaterial.set(false);
	bLimitImage.set(false);
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
void ofxSurfingPBR::setup() {
	ofLogNotice("ofxSurfingPBR") << "setup() Start";

	//ofSetLogLevel("ofxSurfing", OF_LOG_WARNING);

	// for using on any objects
	material.setup();

	setupParams();

	//--

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	setupShaderPlane();
#endif

	//--

	setupPBRScene();

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
const ofxSurfing::SURFING_LAYOUT ofxSurfingPBR::getLayoutHelp()
{
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
	f_RenderScene = f;
};

//--------------------------------------------------------------
ofParameterGroup & ofxSurfingPBR::getMaterialParameters() { //mainly to expose to external gui's like ImGui
	return material.parameters;
};

//--------------------------------------------------------------
void ofxSurfingPBR::startup() {
	ofLogNotice("ofxSurfingPBR") << "startup() Start";

	if (!this->getSettingsFileFound()) {
		ofLogWarning("ofxSurfingPBR") << "Initialize settings for the first time!";
		ofLogWarning("ofxSurfingPBR") << "Potential Newbie User Found!";
		ofLogWarning("ofxSurfingPBR") << "Forcing states and some default stuff visible:";
		ofLogWarning("ofxSurfingPBR") << "Enabled help, debug, reset camera and settings, etc...";

		//--
#if 0
		// Initialize a Default Startup Scene!
		{
			// Background
			bDrawBg.set(true);
			bgColor.set(ofFloatColor(0, 0.03, 0.3, 1));

			// Plane
			planeGlobalColor.set(ofFloatColor(0.25, 0, 0.5, 1));

			// Material
			material.roughness = 0.5;
			material.reflectance = 0.5;
			material.globalColor.set(ofFloatColor::orange);
		}
#endif
		//--

		// Force enable Help and Debug states!
		// Ready to a newbie user!
		// All important/learning controls will be visible.
		{
			bGui = true;
			bGui_ofxGui = true;
			bHelp = true;
			bDebug = true;
			bDebugShadow = true;
		}
	}

	//--

	if (!this->getSettingsFileFoundForCamera()) {
		doResetCamera();
	} else if (camera != nullptr)
		ofxLoadCamera(*camera, pathCamera);

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

	bDoneDelayed = true;
	ofLogNotice("ofxSurfingPBR") << "startupDelayed() Done! at frame number: " << ofGetFrameNum();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupPBRScene() {
	ofLogNotice("ofxSurfingPBR") << "setupPBRScene()";

	//--

	// lights and shadows

	int amountLights = 1; //currently limited to 1 light!

	for (int i = 0; i < amountLights; i++) {
		auto light = make_shared<ofLight>();
		light->enable();
		if (i == 0) {
			light->setPointLight();
			light->getShadow().setNearClip(20);
			light->getShadow().setFarClip(SURFING__SCENE_SIZE_UNIT);

			//TODO: not working.. trying to set "bounding box"
			//float w = shadowSize.get().x * 5 * SURFING__SCENE_SIZE_UNIT;
			//float h = shadowSize.get().y * 5 * SURFING__SCENE_SIZE_UNIT;
			//light->getShadow().setDirectionalBounds(w,h);
		}

		// currently not used ?
		else {
			light->setSpotlight(60, 20);
			light->getShadow().setNearClip(200);
			light->getShadow().setFarClip(2000);
			light->setPosition(210, 330.0, 750);
			light->setAmbientColor(ofFloatColor(0.4));
		}

		//TODO: not working..
		//light->getShadow().setStrength(shadowStrength);
		light->getShadow().setStrength(0.6);

		if (light->getType() != OF_LIGHT_POINT) {
			glm::quat xq = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1, 0, 0));
			glm::quat yq = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
			light->setOrientation(yq * xq);
		}
		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	ofShadow::enableAllShadows();
	ofShadow::setAllShadowTypes(OF_SHADOW_TYPE_PCF_LOW);
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupGui() {
	gui.setup(parameters);

	//TOOO: remove the button to include only in settings..?
	//gui.getGroup(internalParams.getName()).getButton(bGui.getName()).setSize(5, 6);

	refreshGui();
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "refreshGui()";

	gui.setPosition(SURFING__PAD_TO_WINDOW_BORDERS, SURFING__PAD_TO_WINDOW_BORDERS);

	// minimize sub panels

	gui.getGroup(planeParams.getName())
		.getGroup(planeTransformParams.getName())
		.minimize();

	gui.getGroup(planeParams.getName())
		.getGroup(planeMaterialParams.getName())
		.getGroup(planeSettingsParams.getName())
		.minimize();
	gui.getGroup(planeParams.getName())
		.getGroup(planeMaterialParams.getName())
		.getGroup(planeColorsParams.getName())
		.minimize();

	gui.getGroup(lightParams.getName()).minimize();
	gui.getGroup(shadowParams.getName()).minimize();
	gui.getGroup(backgroundParams.getName()).minimize();
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
		if (f == 0) {
			if (!bAppRunning) {
				bAppRunning = true;
				ofLogNotice("ofxSurfingPBR") << "Starting app at frame number: " << ofGetFrameNum();
			}
		}

		// After the 1st frame
		if (f > 0 && bAppRunning && !bDoneDelayed) {
			//TODO fix crash callbacks
			startupDelayed();
		}
	}

	//--

	updatePBRScene();
}

//--------------------------------------------------------------
void ofxSurfingPBR::updatePBRScene() {

	// shadow
	for (auto & light : lights) {
		light->getShadow().setEnabled(bDrawShadow);
	}

	// lights
	//TODO: not used bc limited to 1 single light!
	if (lights.size() > 0) {
		if (lights[0]->getType() == OF_LIGHT_POINT) {
			float tangle = ofGetElapsedTimef() * 1.05;
			lights[0]->setPosition(lightX, lightY, lightZ);
			lights[0]->setAmbientColor(lightAmbientColor);
		}
	}

	ofShadow::setAllShadowBias(shadowBias.get());
	ofShadow::setAllShadowNormalBias(shadowNormalBias.get());

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
void ofxSurfingPBR::drawGui() {
	if (!bGui) return;

	ofDisableDepthTest();

	if (bGui_ofxGui) {
		gui.draw();

		if (material.bGui) {
			// Force position for material gui
			glm::vec3 p;
			if (isWindowPortrait()) {
				p = gui.getShape().getBottomLeft() + glm::vec2(0, SURFING__PAD_OFXGUI_BETWEEN_PANELS);
			} else {
				p = gui.getShape().getTopRight() + glm::vec2(SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0);
			}
			material.gui.setPosition(p);

			material.drawGui();
		}
	}

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
void ofxSurfingPBR::drawDebug() {

	// Fps and window size
#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	if (!bShaderToPlane && !bDisplaceToMaterial)
#endif
	{
		string s = "";
		s += ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
		float fps = ofGetFrameRate();
		s += ofToString(fps, 1);
		s += " FPS";
		//s += "\n";

		ofxSurfing::SURFING_LAYOUT layout;
		if (guiLayout.get() == 0)
			layout = ofxSurfing::SURFING_LAYOUT_TOP_CENTER;
		else if (guiLayout.get() == 1)
			layout = ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER;
		else
			layout = ofxSurfing::SURFING_LAYOUT_CENTER;

		ofxSurfing::ofDrawBitmapStringBox(s, layout);
	}

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
		//bool bflip = helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;
		//if (!bflip)
		//	l= ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;
		//else //flip to avoid overlap
		//	l= ofxSurfing::SURFING_LAYOUT_TOP_RIGHT;
		ofxSurfing::ofDrawBitmapStringBox(s, l);

		auto bb = ofxSurfing::getBBBitmapStringBoxToLayout(s, layout);

		// image preview
		w = bb.getWidth();
		h = w * r;
		x = bb.getTopLeft().x;

		//to avoid empty pixels on union
		float round = (float)SURFING__STRING_BOX__DEFAULT_ROUND;
		bool isRound = round > 0;

		if (!bflip) {
			y = bb.getTopLeft().y - h;
			if (isRound) y += round;
		} else {
			y = bb.getBottomLeft().y;
			if (isRound) y -= round;
		}
		img.draw(x, y, w, h);

		// border rect
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
void ofxSurfingPBR::draw() {
	if (f_RenderScene == nullptr) return;

	drawPBRScene();

	//--

	//ofEnableDepthTest();

	// camera
	camera->begin();
	{
		// Alternative Bg
		if (bDrawBg) {
			ofClear(bgColor);
		}

		//----

		glEnable(GL_CULL_FACE);

		glFrontFace(GL_CW);
		// Maybe should fix bc makes some models "transparent"...
		// sets the orientation for front-facing
		// polygons1GL_CW means that polygons with vertices
		// in clockwise order on the screen are considered front-facing1.

		glCullFace(GL_BACK);
		{
			if (f_RenderScene != nullptr) f_RenderScene();
		}
		glDisable(GL_CULL_FACE);

		//----

		drawPBRSceneDebug();
	}
	camera->end();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPBRScene() {
	if (f_RenderScene == nullptr) return;

	ofSetColor(ofColor::white);
	ofEnableDepthTest();

	// compute shadows
	for (int i = 0; i < lights.size(); i++) {
		auto & light = lights[i];

		if (light->shouldRenderShadowDepthPass()) {

			int numShadowPasses = light->getNumShadowDepthPasses();
			for (int j = 0; j < numShadowPasses; j++) {
				light->beginShadowDepthPass(j);
				{
					if (f_RenderScene != nullptr) f_RenderScene();
				}
				light->endShadowDepthPass(j);
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPBRSceneDebug() {

	// debug lights
	if (bGui)
		for (int i = 0; i < lights.size(); i++) {
			auto & light = lights[i];

			ofSetColor(light->getDiffuseColor());
			if (light->getType() == OF_LIGHT_POINT) {
				if (bDebug) ofDrawSphere(light->getPosition(), 12);
			} else {
				light->draw();
			}
			if (light->getShadow().getIsEnabled()) {
				if (bDebug && bDebugShadow) light->getShadow().drawFrustum();
			}
		}

		//--

		// debug shadows
#ifdef SURFING__USE_CUBE_MAP
	if (bDrawCubeMap && bLoadedCubeMap) {
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

	//TODO
	//use params to allow customize
	int resX, resY;
#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	resX = MIN(1024, w / 10.f);
	resY = MIN(1024, h / 10.f);
#else
	resX = (int)SURFING__PLANE_RESOLUTION;
	resY = (int)SURFING__PLANE_RESOLUTION;
#endif

	plane.set(w, h, resX, resY);

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
	autoSaver.saveSoon();
#endif

	//--

	if (name == planeSize.getName()) {
		static glm::vec2 planeSize_ = glm::vec2(-1, -1);
		if (planeSize.get() != planeSize_) { // if changed
			planeSize_ = planeSize.get();

			refreshPlane();
		} else {
			ofLogVerbose("ofxSurfingPBR") << "Plane size not Changed. Skipped refresh!";
		}
	}

	else if (name == bPlaneInfinite.getName()) {
		refreshPlane();
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
		materialPlane.setShininess(planeShiness.get() * SURFING__PBR__MAX_SHININESS);
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
void ofxSurfingPBR::ChangedLight(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedLight " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (name == vResetLight.getName()) {
		doResetLight();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedShadow(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedShadow " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (name == vResetShadow.getName()) {
		doResetShadow();
	}

	else if (name == bDebugShadow.getName()) {
		if (bDebugShadow) {
			if (!bDebug) bDebug = true; //workflow
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedInternal(ofAbstractParameter & e) {
	if (bDisableCallbacks) return;

	std::string name = e.getName();

	//TODO fix crash callbacks
	//// Exclude to avoid crashes..
	//if (name == nameGuiLayout.getName()) return;

	ofLogNotice("ofxSurfingPBR") << "ChangedInternal " << name << ": " << e;

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (name == bKeys.getName()) {
		buildHelp();
	}

	else if (name == helpLayout.getName()) {
		string s = ofxSurfing::getLayoutName(helpLayout.get());
		nameHelpLayout.set(s);
		//nameHelpLayout.setWithoutEventNotifications(s);
	}

	else if (name == guiLayout.getName()) {
		//bDisableCallbacks = true;

		//do not updates the gui!
		if (guiLayout.get() == 0)
			nameGuiLayout.setWithoutEventNotifications(namesGuiLayouts[0]);
		else if (guiLayout.get() == 1)
			nameGuiLayout.setWithoutEventNotifications(namesGuiLayouts[1]);

		//--

		//TODO fix crash callbacks
		//Exception thrown : read access violation.std::shared_ptr<of::priv::BaseEvent<of::priv::Function<ofAbstractParameter, std::recursive_mutex>, std::recursive_mutex>::Data>::operator->**<of::priv::BaseEvent<of::priv::Function<ofAbstractParameter, std::recursive_mutex>, std::recursive_mutex>::Data, 0> **(...) returned nullptr.

		//if (!bDoneSetup) return;
		if (!bDoneStartup) return;

		//--

		if (guiLayout.get() == 0)
			nameGuiLayout.set(namesGuiLayouts[0]);
		else if (guiLayout.get() == 1)
			nameGuiLayout.set(namesGuiLayouts[1]);

		//bDisableCallbacks = false;
	}

	else if (name == vMinimizeAllGui.getName()) {
		gui.minimizeAll();

		material.gui.minimizeAll();
		material.guiHelpers.minimizeAll();
	} else if (name == vMaximizeAllGui.getName()) {
		gui.maximizeAll();
		material.gui.maximizeAll();
		material.guiHelpers.maximizeAll();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedTestScene(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedTestScene " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
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

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (name == vSaveCamera.getName()) {
		ofxSaveCamera(*camera, pathCamera);
	} else if (name == vLoadCamera.getName()) {
		ofxLoadCamera(*camera, pathCamera);
	} else if (name == vResetCamera.getName()) {
		doResetCamera();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::ChangedBg(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedBg " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

#ifdef SURFING__USE_CUBE_MAP
	if (name == bDrawBg.getName()) {
		if (!bLoadedCubeMap) return; //skip
		//workflow
		if (bDrawBg)
			if (bDrawCubeMap) bDrawCubeMap = false;
	}
#endif
}

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void ofxSurfingPBR::ChangedDisplacers(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedDisplacers " << name << ": " << e;

	#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
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
		refreshPlane();
	}
}
#endif

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::ChangedCubeMaps(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "ChangedCubeMaps " << name << ": " << e;

	#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
	#endif

	//--

	if (name == vOpenCubeMap.getName()) {
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a exr or EXR file.");
		if (openFileResult.bSuccess) {
			ofLogNotice("ofxSurfingPBR") << ("User selected a file");
			processOpenFileSelection(openFileResult);

		} else {
			ofLogNotice("ofxSurfingPBR") << ("User hit cancel");
		}
	} else if (name == vResetCubeMap.getName()) {
		if (!bLoadedCubeMap) return; //skip
		doResetCubeMap();
	} else if (name == cubeMapMode.getName()) {
		//TODO:
		//return;//fix crash
		if (!bLoadedCubeMap) return; //skip
		if (cubeMapMode.get() == 1)
			cubeMapModeName.set(ofToString("Cube Map"));
		else if (cubeMapMode.get() == 2)
			cubeMapModeName.set(ofToString("PreFilter Roughness"));
		else if (cubeMapMode.get() == 3)
			cubeMapModeName.set(ofToString("Irradiance"));
	}

	else if (name == bDrawCubeMap.getName()) {
		if (!bLoadedCubeMap) return; //skip
		//workflow
		if (bDrawCubeMap)
			if (bDrawBg) bDrawBg = false;
	}
}
#endif

//----

//--------------------------------------------------------------
void ofxSurfingPBR::drawTestScene() {
	// do once
	{
		static bool b = false;
		if (!b) {
			b = true;
			ofSetConeResolution(50, 10, 2);
		}
	}

	//--

#define DO_SCENE_TEST_TRANSFORMS 1

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
	//save();
	//material.exit();

	if (bEnableCameraAutosave) ofxSaveCamera(*camera, pathCamera);
}

//--------------------------------------------------------------
void ofxSurfingPBR::save() {
	ofLogNotice("ofxSurfingPBR") << "Save: " << path;

	// Save
	{
		//gui.saveToFile(path);
		ofxSurfing::saveSettings(parameters, path);
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::load() {
	ofLogNotice("ofxSurfingPBR") << "Load: " << path;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	// Load
	{
		//gui.loadFromFile(path);
		ofxSurfing::loadSettings(parameters, path);
	}

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFoundForScene() {

	// search for the mandatory settings file to consider if the app is opened for the first time.
	ofFile f;
	bool b = f.doesFileExist(path);
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
	ofFile f2;
	bool b2 = f2.doesFileExist(material.path);
	if (b2) {
		ofLogNotice("ofxSurfingPBR") << "Found MATERIAL settings file: " << material.path;
	} else {
		ofLogWarning("ofxSurfingPBR") << "MATERIAL settings file: " << material.path << " not found!";
	}
	return b2;
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFoundForCamera() {
	ofFile f3;
	bool b3 = f3.doesFileExist(pathCamera);
	if (b3) {
		ofLogNotice("ofxSurfingPBR") << "Found CAMERA settings file: " << pathCamera;
	} else {
		ofLogWarning("ofxSurfingPBR") << "CAMERA settings file: " << pathCamera << " not found!";
	}
	return b3;
}

//--

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
bool ofxSurfingPBR::loadCubeMap(string path) {

	// loading a cube map will enable image based lighting on PBR materials.
	// cube map will create a prefiltered light cube map and an irradiance cube map
	// cube map texture from https://polyhaven.com
	// comment out the loading of the cube map image to see added cube map lighting without image
	// fake environment lighting is added in the pbr shader

	ofCubeMap::ofCubeMapSettings csettings;

	if (path == "") //uses default path
		csettings.filePath = path_Cubemaps + "/" + path_CubemapFilename;
	else //uses passed path
		csettings.filePath = path;

	// uncomment to load from a cache or make one if it doesn't exist
	// 	csettings.useCache = true;

	// uncomment to use the maximum precision available. OpenGL ES is float16 and OpenGL is float32;
	//	csettings.useMaximumPrecision = true;

	// make sure the defaults are the same for making and loading the cache
	// ie opengl es defaults are smaller and the file names to load are based on the resolution
	csettings.irradianceRes = 32;
	csettings.preFilterRes = 128;

	bool b = cubeMap.load(csettings);
	// below is another method for loading a cube map without passing settings class
	// cubeMap.load("modern_buildings_2_1k.exr", 512, true );

	if (!b)
		ofLogError("ofxSurfingPBR") << "Error loading cubemap: " << csettings.filePath;
	else
		ofLogNotice("ofxSurfingPBR") << "Successfully loaded cubemap: " << csettings.filePath;

	//--

	bLoadedCubeMap = b;

	if (b) {
		cubeMapMode = cubeMapMode; //refresh
		cubeMapName = path_CubemapFilename;
	} else {
		cubeMapModeName = "NONE";
		cubeMapName = "NONE";
	}

	return b;
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupCubeMap() {
	ofLogNotice("ofxSurfingPBR") << "setupCubeMap()";

	cubeMapName.setSerializable(false);
	cubeMapModeName.setSerializable(false);

	cubeMapModeName.set("Type CubeMap", "NONE");
	cubeMapMode.set("Mode CubeMap", 2, 1, 3);
	cubeMapName.set("Name CubeMap", "NONE");
	cubeMapprefilterRoughness.set("Roughness CubeMap", 0.25f, 0, 1.f);
	vOpenCubeMap.set("Open File");
	vResetCubeMap.set("Reset CubeMap");

	//--

	cubeMapParams.setName("CubeMap");

	cubeMapParams.add(bDrawCubeMap);
	cubeMapParams.add(cubeMapName);
	cubeMapParams.add(vOpenCubeMap);
	cubeMapParams.add(cubeMapMode);
	cubeMapParams.add(cubeMapModeName);
	cubeMapParams.add(cubeMapprefilterRoughness);
	cubeMapParams.add(vResetCubeMap);

	parameters.add(cubeMapParams);

	//--

	loadCubeMap();
}

void ofxSurfingPBR::processOpenFileSelection(ofFileDialogResult openFileResult) {

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

void ofxSurfingPBR::keyPressed(int key) {
	if (!bKeys) return;

	if (key == 'g') bGui = !bGui;
	if (key == 'G') bGui_ofxGui = !bGui_ofxGui;

	if (key == 'p') bDrawPlane = !bDrawPlane;
	if (key == 's') bDrawShadow = !bDrawShadow;
	if (key == 'b') bDrawBg = !bDrawBg;

#ifdef SURFING__USE_CUBE_MAP
	if (key == 'c') bDrawCubeMap = !bDrawCubeMap;
#endif

	if (key == 'h') bHelp = !bHelp;
	if (key == 'd') bDebug = !bDebug;
	if (key == 'i') bPlaneInfinite = !bPlaneInfinite;

	if (key == 'l') doNextLayoutHelp(); //next layout help
	if (key == OF_KEY_TAB) doNextLayouGui(); //next layout gui

	if (key == 'f') ofToggleFullscreen();
	if (key == 'q') ofxSurfing::setWindowSquared(800);

	// Rajndomizers
	if (key == OF_KEY_F1) doResetMaterial();
	if (key == OF_KEY_F2) doRandomMaterial();
	if (key == OF_KEY_F3) doRandomMaterialSettings();
	if (key == OF_KEY_F4) doRandomMaterialColorGlobal();
	if (key == OF_KEY_F5) doRandomMaterialColors();
	if (key == OF_KEY_F6) doRandomMaterialColorsAlpha();
	if (key == OF_KEY_F7) doRandomMaterialAlphas();

	// history
	if (key == 'z') material.doPrevHistory();
	if (key == 'x') material.doNextHistory();
	if (key == 'r') material.doRecallState();
	if (key == 's') material.doStoreNewState();
	if (key == 'u') material.doSaveState();
}

void ofxSurfingPBR::setLogLevel(ofLogLevel logLevel) {
	ofSetLogLevel("ofxSurfingPBR", logLevel);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetLight() {
	lightX = 0;
	lightY = SURFING__SCENE_SIZE_UNIT * 0.5;
	lightZ = 0;
	lightAmbientColor.set(ofFloatColor(1.f, 1.f));
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlane() {
	doResetPlaneTransform();

	planeShiness.set(0.85);

	ofFloatColor c = ofFloatColor(0.5f, 1.f);
	planeGlobalColor.set(c);

	//planeDiffuseColor.set(ofFloatColor(0.6));
	//planeSpecularColor.set(ofFloatColor(1));

	bPlaneWireframe = false;
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlaneTransform() {
	planeSize.set(glm::vec2(0.12, 0.05));
	bPlaneInfinite = false;
	planePosition.set(0.f);
	planeRotation.set(10.f);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetShadow() {
	bDrawShadow.set(true);
	shadowBias.set(0.1);
	shadowNormalBias.set(-4.f);
	//shadowStrength.set(0.6f);//TODO
	//shadowSize.set(glm::vec2(0.25f,0.25f));//TODO
	//bDebugShadow.set(false);
}

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::doResetCubeMap() {
	cubeMapMode = 2;
	//bDrawCubeMap = true;
	cubeMapprefilterRoughness = 0.25f;
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::doResetCamera() {
	ofEasyCam * easyCam = dynamic_cast<ofEasyCam *>(camera);
	if (easyCam != nullptr) {
		easyCam->reset();

		camera->setFarClip(SURFING__SCENE_CAMERA_FAR);

		easyCam->setPosition({ 8.35512, 581.536, 696.76 });
		easyCam->setOrientation({ 0.940131, -0.340762, 0.00563653, 0.00204303 });

		//save
		vSaveCamera.trigger();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetAll(bool bExcludeMaterial) {
	bDebug = false;

	doResetPlane();
	doResetLight();
	doResetShadow();

#ifdef SURFING__USE_CUBE_MAP
	doResetCubeMap();
#endif

#ifdef SURFING__USE__PLANE_SHADER_AND_DISPLACERS
	doResetNoise();
	doResetDisplace();
#endif

	if (!bExcludeMaterial) material.doResetMaterial();

	bgColor.set(ofFloatColor(0.2f));
#ifdef SURFING__USE_CUBE_MAP
	if (bDrawBg) bDrawBg = false;
#else
	bDrawBg = true;
#endif

	vResetTestScene.trigger();

	vResetCamera.trigger();
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetTestScene() {
	scaleTestScene = -0.6;
	positionTestScene = 0;
}

//--

//--------------------------------------------------------------
void ofxSurfingPBR::doResetMaterial() {
	material.doResetMaterial();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterial() {
	material.doRandomMaterial();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialSettings() {
	material.doRandomSettings();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColorGlobal() {
	material.doRandomColorGlobal();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColors() {
	material.doRandomColors();
	material.doStoreNewState();
};
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColorsAlpha() {
	material.doRandomColorsAlpha();
	material.doStoreNewState();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialAlphas() {
	material.doRandomAlphas();
	material.doStoreNewState();
}