#include "ofxSurfingPBR.h"
//--------------------------------------------------------------
ofxSurfingPBR::ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "constructor()";

	ofAddListener(ofEvents().update, this, &ofxSurfingPBR::update);

	sHelp = "";
	//sHelp += " \n";
	sHelp += " \n";
	sHelp += "HELP\n";
	sHelp += "ofxSurfingPBR\n";
	sHelp += "\n";
	sHelp += "KEYS\n";
	sHelp += "\n";
	sHelp += "h Help\n";
	sHelp += "d Debug\n";
	sHelp += "g/G Gui\n";
	sHelp += "p Draw Plane\n";
	sHelp += "c Draw CubeMap\n";
	sHelp += "s Draw Shadow \n";
	sHelp += "b Draw BgAlt\n";
	sHelp += "f FullScreen\n";
	sHelp += "\n";
	sHelp += "MATERIAL\n";
	sHelp += "HELPERS\n";
	sHelp += "\n";
	sHelp += "F1 Reset Material\n";
	sHelp += "F2 Random Material\n";
	sHelp += "F3 Random Settings\n";
	sHelp += "F4 Random ColorGlobal\n";
	sHelp += "F5 Random Colors\n";
	sHelp += "F6 Random ColorsAlpha\n";
	sHelp += "F7 Random Alphas";
}

//--------------------------------------------------------------
ofxSurfingPBR::~ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "destructor()";

	ofRemoveListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofRemoveListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "setupParams()";

	bGui.set("PBR", true);
	bGui_ofxGui.set("PBR", true);

	bDebug.set("Debug", false);
	bKeys.set("Keys", true);
	bHelp.set("Help", false);

	scaleTestScene.set("Scale", 0, 0, 1);
	positionTestScene.set("yPosition", 0, -1, 1);
	resetTestScene.set("Reset SceneTest");

	testSceneParams.setName("TestScene");
	testSceneParams.add(scaleTestScene);
	testSceneParams.add(positionTestScene);
	testSceneParams.add(resetTestScene);

#ifdef SURFING__USE_AUTOSAVE_ENGINE
	bAutoSave.set("Autosave", true);
	////link
	////TODO: not working?
	//material.bAutoSave.makeReferenceTo(bAutoSave);
	////bAutoSave.makeReferenceTo(material.bAutoSave);
#endif

	//--

	parameters.setName("PBR_Scene");

	planeParams.setName("Plane");
	planeTransformParams.setName("Transform");
	planeSettingsParams.setName("Settings");
	lightParams.setName("Light");
	shadowParams.setName("Shadow");
	internalParams.setName("Internal");

	//--

	resetPlane.set("Reset Plane");
	resetPlaneTransform.set("Reset Transform");
	resetLight.set("Reset Light");
	resetShadow.set("Reset Shadow");
	resetAll.set("Reset All");

	//--

	bDrawPlane.set("Draw Plane", true);
	bPlaneWireframe.set("Draw Wireframe", false);
	bPlaneInfinite.set("Infinite", true);
	planeSize.set("Size", glm::vec2(0.8f, 0.8f), glm::vec2(0, 0), glm::vec2(1, 1));
	planeRotation.set("x Rotation", 0, -45, 135);
	planePosition.set("y Position", 0, -1, 1);
	planeShiness.set("Shiness", 0.85, 0, 1);
	planeGlobalColor.set("Global Color", ofFloatColor(1.f), ofFloatColor(0), ofFloatColor(1));
	planeGlobalColor.setSerializable(false);
	planeDiffuseColor.set("Diffuse Color", ofFloatColor(0.6f), ofFloatColor(0), ofFloatColor(1));
	planeSpecularColor.set("Specular Color", ofFloatColor(1), ofFloatColor(0), ofFloatColor(1));

	planeParams.add(bDrawPlane);
	planeParams.add(bPlaneWireframe);

	planeTransformParams.add(planeSize);
	planeTransformParams.add(bPlaneInfinite);
	planeTransformParams.add(planeRotation);
	planeTransformParams.add(planePosition);
	planeTransformParams.add(resetPlaneTransform);
	planeParams.add(planeTransformParams);

	planeSettingsParams.add(planeGlobalColor);
	planeSettingsParams.add(planeDiffuseColor);
	planeSettingsParams.add(planeSpecularColor);
	planeSettingsParams.add(planeShiness);

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	setupParamsDisplace();
#endif

	planeParams.add(planeSettingsParams);
	planeParams.add(resetPlane);

	//--

	lightAmbientColor.set("Light Ambient Color", ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
	float u = 2 * SURFING__SZ_UNIT;
	lightParams.add(lightX.set("X", 0.0f, -u, u));
	lightParams.add(lightY.set("Y", 0.0f, -u, u));
	lightParams.add(lightZ.set("Z", 0.0f, -u, u));
	lightParams.add(lightAmbientColor);
	lightParams.add(resetLight);

	//--

	shadowParams.add(bDrawShadow.set("Draw Shadow", true));
	shadowParams.add(shadowBias.set("Bias", 0.07, 0.0, 1.0));
	shadowParams.add(shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0));
	//TODO
	//shadowParams.add(shadowStrength.set("Strength", 0.6f, 0.f, 1.f));
	//shadowParams.add(shadowSize.set("Shadow Size", glm::vec2(0.25f, 0.25f), glm::vec2(0, 0), glm::vec2(1.f, 1.f)));
	shadowParams.add(bDebugShadow.set("Debug Shadow", false));
	shadowParams.add(resetShadow);

	//--

	parameters.add(planeParams);
	parameters.add(lightParams);
	parameters.add(shadowParams);

	//--

#ifdef SURFING__USE_CUBE_MAP
	setupCubeMap();
#endif

	//--

	internalParams.add(testSceneParams);
	internalParams.add(bDebug);
	internalParams.add(bHelp);
	internalParams.add(bKeys);
#ifdef SURFING__USE_AUTOSAVE_ENGINE
	internalParams.add(bAutoSave);
#endif
	parameters.add(internalParams);

	parameters.add(resetAll);

	ofAddListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);
}

//--

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void ofxSurfingPBR::refreshImgShaderPlane() {
	ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane()";

	float w, h;
	w = plane.getWidth();
	h = plane.getHeight();

	w = ofClamp(w, 0, 800);
	h = ofClamp(h, 0, 600);

	//reduce a bit image re allocations when not required
	//bc sizes not changed..
	static float w_ = -1;
	static float h_ = -1;
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
		//plane.set(800, 600, 80, 60);
		plane.mapTexCoordsFromTexture(img.getTexture());

		ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane() w,h: " << w << "," << h;
	}

	else {
		ofLogNotice("ofxSurfingPBR") << "refreshImgShaderPlane() Skipped img.allocate";
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

	resetDisplace.set("Reset Displace");
	resetNoise.set("Reset Noise");

	bShaderToPlane.set("ShaderToPlane", false);
	bDisplaceToMaterial.set("DispToMaterial", false);

	noiseParams.setName("Noise Generator");
	noiseParams.add(noiseAmplitude.set("Noise Amp D", 0.0f, 0.0f, 2.f));
	noiseParams.add(noiseScale.set("Noise Scale", 0.0f, 0.0f, 0.1f));
	noiseParams.add(noiseSpeed.set("Noise Speed", 0.0f, 0.0f, 1.0f));
	noiseParams.add(resetNoise);

	displaceMaterialParams.setName("Material Disp");
	displaceMaterialParams.add(displacementStrength.set("D Strength", 0.0, 0.0, 500.0));
	displaceMaterialParams.add(displacementNormalsStrength.set("D Normals Strength", 0.0, 0.0, 1.0));
	displaceMaterialParams.add(normalGeomToNormalMapMix.set("NGeom NMapMix", 0.0, 0.0, 1.0));
	displaceMaterialParams.add(resetDisplace);

	displacersParams.setName("Displacers");
	displacersParams.add(bShaderToPlane);
	displacersParams.add(bDisplaceToMaterial);
	displacersParams.add(noiseParams);
	displacersParams.add(displaceMaterialParams);

	planeSettingsParams.add(displacersParams);
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
	ofLogNotice("ofxSurfingPBR") << "setup()";

	// for using on any objects
	material.setup();

	setupParams();

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	setupShaderPlane();
#endif

	//--

	// lights and shadows

	int amountLights = 1; //currently limited to 1 light!

	for (int i = 0; i < amountLights; i++) {
		auto light = make_shared<ofLight>();
		light->enable();
		if (i == 0) {
			light->setPointLight();
			light->getShadow().setNearClip(20);
			light->getShadow().setFarClip(SURFING__SZ_UNIT);

			//TODO: not working.. trying to set "bounding box"
			//float w = shadowSize.get().x * 5 * SURFING__SZ_UNIT;
			//float h = shadowSize.get().y * 5 * SURFING__SZ_UNIT;
			//light->getShadow().setDirectionalBounds(w,h);
		}

		//// currently not used
		//else {
		//	light->setSpotlight(60, 20);
		//	light->getShadow().setNearClip(200);
		//	light->getShadow().setFarClip(2000);
		//	light->setPosition(210, 330.0, 750);
		//	light->setAmbientColor(ofFloatColor(0.4));
		//}

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

	//--

	// reset
	doResetAll(true); //except material

	setupGui();

	load();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupGui() {
	gui.setup(parameters);

	//minimize
	//gui.getGroup(planeParams.getName()).minimize();
	gui.getGroup(planeParams.getName())
		.getGroup(planeTransformParams.getName())
		.minimize();
	gui.getGroup(lightParams.getName()).minimize();
	gui.getGroup(shadowParams.getName()).minimize();
	gui.getGroup(internalParams.getName()).minimize();

#ifdef SURFING__USE_CUBE_MAP
	gui.getGroup(cubeMapParams.getName()).minimize();
#endif

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	gui.getGroup(planeParams.getName())
		.getGroup(planeSettingsParams.getName())
		.getGroup(displacersParams.getName())
		.minimize();
	gui.getGroup(planeParams.getName())
		.getGroup(planeSettingsParams.getName())
		.getGroup(displacersParams.getName())
		.getGroup(displaceMaterialParams.getName())
		.minimize();
	gui.getGroup(planeParams.getName())
		.getGroup(planeSettingsParams.getName())
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
	if (ofGetFrameNum() == 0) {
		if (!bDoneStartup) bDoneStartup = true;
	}

#ifdef SURFING__USE_AUTOSAVE_ENGINE
	// autosave workflow
	if (bAutoSave) {
		auto t = ofGetElapsedTimeMillis() - timeLastChange;
		if (bFlagSave && t > timeSaveDelay) {
			bFlagSave = false;
			save();
		}
	}
#endif

	//--

	// shadow
	for (auto & light : lights) {
		light->getShadow().setEnabled(bDrawShadow);
	}

	// lights
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

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
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
		//force position
		auto p = gui.getShape().getTopRight() + glm::vec2(2, 0);
		material.gui.setPosition(p);

		gui.draw();
		material.drawGui();
	}

	if (bDebug) drawDebug();
	if (bHelp) drawHelp();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawHelp() {
	int x, y, w, h;
	int pad = 0;
	int ox;
	int oy;

	ofBitmapFont bf;
	auto bb = bf.getBoundingBox(sHelp, 0, 0);

	if (0) {
		// left-bottom
		x = pad;
		y = ofGetHeight() - bb.getHeight() - pad;
		ox = 4;
		oy = -6;
	} else {
		// top-right
		x = ofGetWidth() - bb.getWidth() - pad;
		y = pad;
		ox = -4;
		oy = 14;
	}

	//fix offsets
	x += ox;
	y += oy;

	ofDrawBitmapStringHighlight(sHelp, x, y);
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawDebug() {

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	if (bShaderToPlane || bDisplaceToMaterial) {
		int x, y, w, h;
		int pad = 0;

		//fix offsets
		int ox = 4;
		int oy = 3;

		float r = img.getHeight() / img.getWidth();

		// info
		string s = "";
		s += " \n";
		s += "DEBUG\nSHADER\n\n";
		s += "PLANE\n";
		s += "Size: " + ofToString(plane.getWidth(), 0) + "," + ofToString(plane.getHeight(), 0) + "\n";
		s += "Reso: " + ofToString(plane.getResolution().x) + "," + ofToString(plane.getResolution().y) + "\n\n";
		s += "IMAGE\n";
		s += "Size: " + ofToString(img.getWidth()) + "," + ofToString(img.getHeight());
		s += "\n\n" + ofToString(ofGetFrameRate(), 1);
		s += " FPS";

		ofBitmapFont bf;
		auto bb = bf.getBoundingBox(s, 0, 0);

		// bottom-right
		x = ofGetWidth() - bb.getWidth() - pad;
		y = ofGetHeight() - bb.getHeight() - pad;

		//fix offsets
		x -= 4;
		y += 7;

		ofDrawBitmapStringHighlight(s, x, y);

		// image preview
		w = bb.getWidth() + 2 * ox;
		h = w * r;
		x = x + bb.getTopLeft().x - ox;
		y = y + bb.getTopLeft().y - h - oy;
		img.draw(x, y, w, h);

		int lw = 2;
		ofRectangle rbb = ofRectangle(x + lw / 2, y + lw / 2, w - lw / 2, h - lw / 2);
		ofPushStyle();
		ofNoFill();
		ofSetColor(0, 255);
		ofSetLineWidth(lw);
		ofDrawRectangle(rbb);
		ofPopStyle();
	}
#endif

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	if (!bShaderToPlane && !bDisplaceToMaterial)
#endif
	{
		float fps = ofGetFrameRate();
		string s = ofToString(fps, 1);
		s += " FPS";
		ofBitmapFont bf;
		auto bb = bf.getBoundingBox(s, 0, 0);
		//fix offsets
		int ox = -4;
		int oy = 8;
		int x = ofGetWindowWidth() - bb.getWidth() + ox;
		int y = ofGetWindowHeight() - bb.getHeight() + oy;
		ofDrawBitmapStringHighlight(s, x, y);
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::draw() {
	if (function_RenderScene == nullptr) return;

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
					if (function_RenderScene != nullptr) function_RenderScene();
				}
				light->endShadowDepthPass(j);
			}
		}
	}

	//--

	// camera
	camera->begin();
	{
#ifdef SURFING__USE_CUBE_MAP
		// Alternative Bg
		if (bDrawBgAlt) {
			ofClear(bgAltColor);
		}
#endif

		//--

		glEnable(GL_CULL_FACE);

		glFrontFace(GL_CW);
		// Maybe should fix bc makes some models "transparent"...
		// sets the orientation for front-facing
		// polygons1GL_CW means that polygons with vertices
		// in clockwise order on the screen are considered front-facing1.

		glCullFace(GL_BACK);
		{
			if (function_RenderScene != nullptr) function_RenderScene();
		}
		glDisable(GL_CULL_FACE);

		//--

		// lights
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
	camera->end();
}

//--------------------------------------------------------------
void ofxSurfingPBR::refreshPlane() {

	float szUnit;
	float w;
	float h;

	if (bPlaneInfinite) {
		szUnit = SURFING__SZ_UNIT * (float)SURFING__PLANE_INFINITE_MAGNITUDE;
		w = szUnit;
		h = szUnit;
	} else {
		szUnit = SURFING__SZ_UNIT * 20.f;
		w = szUnit * planeSize.get().x;
		h = szUnit * planeSize.get().y;
	}

	//--

	//TODO: make it simple..
	// clamp plane size
#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	#ifdef SURFING__CLAMP_PLANE_SIZE_BC_PERFORMANCE

	//hardcoded and clamped
	//will be applied to both plane and img!
	//w = 800;
	//h = 600;
	w = ofClamp(w, 0, 800);
	h = ofClamp(h, 0, 600);

	#endif
#endif

	int resX, resY;
#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	resX = MIN(1024, w / 10.f);
	resY = MIN(1024, h / 10.f);
#else
	resX = (int)SURFING__PLANE_RESOLUTION;
	resY = (int)SURFING__PLANE_RESOLUTION;
#endif

	plane.set(w, h, resX, resY);

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	if (bDisplaceToMaterial || bShaderToPlane)
		refreshImgShaderPlane();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::Changed(ofAbstractParameter & e) {
	if (!bEnableCallbacks) return;

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "Changed " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_ENGINE
	if (bDoneStartup) {
		// flag to save delayed
		if (bAutoSave) {
			bFlagSave = true;
			timeLastChange = ofGetElapsedTimeMillis();
		}
	}
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
	} else if (name == bPlaneInfinite.getName()) {
		refreshPlane();
	}

	else if (name == planeRotation.getName()) {
		glm::vec3 axis(1.0f, 0.0f, 0.0f);
		float angle = planeRotation.get() - 90;
		glm::quat q = glm::angleAxis(glm::radians(angle), axis);
		plane.setOrientation(q);
	}

	else if (name == planePosition.getName()) {
		plane.setPosition(0, planePosition.get() * SURFING__SZ_UNIT * 5.f, 0);
	}

	else if (name == planeShiness.getName()) {
		materialPlane.setShininess(planeShiness.get() * 100);
	}

	else if (name == planeGlobalColor.getName()) {
		planeDiffuseColor = planeGlobalColor.get();
		planeSpecularColor = planeGlobalColor.get();
	} else if (name == planeDiffuseColor.getName()) {
		materialPlane.setDiffuseColor(planeDiffuseColor.get());
	} else if (name == planeSpecularColor.getName()) {
		materialPlane.setSpecularColor(planeSpecularColor.get());
	}

	else if (name == resetPlane.getName()) {
		doResetPlane();
	} else if (name == resetPlaneTransform.getName()) {
		doResetPlaneTransform();
	} else if (name == resetLight.getName()) {
		doResetLight();
	} else if (name == resetShadow.getName()) {
		doResetShadow();
	} else if (name == resetAll.getName()) {
		doResetAll();
	}

	//--

#ifdef SURFING__USE_CUBE_MAP
	else if (name == openCubeMap.getName()) {
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a exr or EXR file.");
		if (openFileResult.bSuccess) {
			ofLogNotice("ofxSurfingPBR") << ("User selected a file");
			processOpenFileSelection(openFileResult);

		} else {
			ofLogNotice("ofxSurfingPBR") << ("User hit cancel");
		}
	} else if (name == resetCubeMap.getName()) {
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

	//TODO:
	//fixing crash
	#if 1
	else if (name == bDrawBgAlt.getName()) {
		if (!bLoadedCubeMap) return; //skip
		//workflow
		if (bDrawBgAlt)
			if (bDrawCubeMap) bDrawCubeMap = false;
	}

	else if (name == bDrawCubeMap.getName()) {
		if (!bLoadedCubeMap) return; //skip
		//workflow
		if (bDrawCubeMap)
			if (bDrawBgAlt) bDrawBgAlt = false;
	}
	#endif
#endif

	else if (name == resetTestScene.getName()) {
		scaleTestScene = 0;
		positionTestScene = 0;
	}

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	else if (name == resetDisplace.getName()) {
		doResetDisplace();
	} else if (name == resetNoise.getName()) {
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
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawTestScene() {
	// do once
	static bool b = false;
	if (!b) {
		b = true;
		ofSetConeResolution(50, 10, 2);
	}

	ofPushMatrix();

	float s = ofMap(scaleTestScene, 0, 1, 1.8, 8.0, true);
	ofScale(s);

	float u = SURFING__SZ_UNIT / 4.f;
	float y = ofMap(positionTestScene, -1, 1, -u, u, true);
	ofTranslate(0, y, 0);

	{
		// Cone
		ofPushMatrix();
		ofTranslate(-200, 100, 0);
		ofRotateXDeg(180);
		ofDrawCone(0, 0, 0, 65, 100);
		ofPopMatrix();

		// Box
		ofPushMatrix();
		float spd = 240;
		ofRotateYDeg(360.f * (ofGetFrameNum() % (int)spd) / spd);
		ofDrawBox(0, 100, 0, 100);
		ofPopMatrix();

		// Sphere
		ofDrawSphere(200, 100, 0, 50);
	}

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPlane() {
	if (!bDrawPlane) return;

	// plane
	if (bPlaneWireframe) {
		plane.drawWireframe();
	}

	else {
#ifdef SURFING__USE_SHADER_AND_DISPLACERS
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

	save();
	material.exit();
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

#ifdef SURFING__USE_AUTOSAVE_ENGINE
	// disables autosave
	// to avoid save after loading the settings,
	// as the params will change and would trig the autosave!
	bool bAutoSave_ = bAutoSave; //store state
	if (bAutoSave) {
		bAutoSave.setWithoutEventNotifications(false);
	}
#endif

	// Load
	{
		//gui.loadFromFile(path);
		ofxSurfing::loadSettings(parameters, path);
	}

#ifdef SURFING__USE_AUTOSAVE_ENGINE
	if (bAutoSave_) {
		bAutoSave.setWithoutEventNotifications(true); //restore state
	}
#endif
}

//--------------------------------------------------------------
bool ofxSurfingPBR::getSettingsFileFound() {
	/*
		will search for the settings files:
		ofxSurfingPBR_Material.json
		ofxSurfingPBR_Scene.json
	*/

	ofFile f;
	bool b = f.doesFileExist(path);
	if (b) {
		ofLogNotice("ofxSurfingPBR") << "Found SCENE settings file: " << path;
	} else {
		ofLogWarning("ofxSurfingPBR") << "SCENE settings file: " << path << " not found!";
	}

	//not required but will log if located or not
	ofFile f2;
	bool b2 = f2.doesFileExist(material.path);
	if (b2) {
		ofLogNotice("ofxSurfingPBR") << "Found MATERIAL settings file: " << material.path;
	} else {
		ofLogWarning("ofxSurfingPBR") << "MATERIAL settings file: " << material.path << " not found!";
	}

	if (!b) ofLogWarning("ofxSurfingPBR") << "Probably opening the App for the first time!";

	return b;
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
	if (1)
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

	cubeMapModeName.set("Type", "NONE");
	cubeMapMode.set("Mode", 2, 1, 3);
	cubeMapName.set("Filename", "NONE");
	bDrawCubeMap.set("Draw CubeMap", true);
	cubeMapprefilterRoughness.set("Roughness", 0.25f, 0, 1.f);
	openCubeMap.set("Open File");
	resetCubeMap.set("Reset CubeMap");
	bDrawBgAlt.set("Draw BgAlt", false);
	bgAltColor.set("BgAlt Color", ofFloatColor::darkGrey, ofFloatColor(0.f), ofFloatColor(1.f));

	//--

	cubeMapParams.setName("CubeMap");

	cubeMapParams.add(bDrawCubeMap);
	cubeMapParams.add(bDrawBgAlt);
	cubeMapParams.add(cubeMapName);
	cubeMapParams.add(openCubeMap);
	cubeMapParams.add(cubeMapMode);
	cubeMapParams.add(cubeMapModeName);
	cubeMapParams.add(cubeMapprefilterRoughness);
	cubeMapParams.add(bgAltColor);
	cubeMapParams.add(resetCubeMap);

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

		if (fileExtension == "exr" || fileExtension == "EXR" || fileExtension == "hdr" || fileExtension == "HDR") {

			bLoadedCubeMap = loadCubeMap(openFileResult.getPath());
		}
	}
}
#endif

void ofxSurfingPBR::keyPressed(int key) {
	if (!bKeys) return;

	if (key == 'h') bHelp = !bHelp;
	if (key == 'd') bDebug = !bDebug;
	if (key == 'g') bGui = !bGui;
	if (key == 'G') bGui_ofxGui = !bGui_ofxGui;
	if (key == 'p') bDrawPlane = !bDrawPlane;
	if (key == 'c') bDrawCubeMap = !bDrawCubeMap;
	if (key == 's') bDrawShadow = !bDrawShadow;
	if (key == 'b') bDrawBgAlt = !bDrawBgAlt;
	if (key == 'f') ofToggleFullscreen();

	if (key == OF_KEY_F1) doResetMaterial();
	if (key == OF_KEY_F2) doRandomMaterial();
	if (key == OF_KEY_F3) doRandomMaterialSettings();
	if (key == OF_KEY_F4) doRandomMaterialColorGlobal();
	if (key == OF_KEY_F5) doRandomMaterialColors();
	if (key == OF_KEY_F6) doRandomMaterialColorsAlpha();
	if (key == OF_KEY_F7) doRandomMaterialAlphas();
}

void ofxSurfingPBR::setLogLevel(ofLogLevel logLevel) {
	ofSetLogLevel("ofxSurfingPBR", logLevel);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetLight() {
	lightX = 0;
	lightY = SURFING__SZ_UNIT;
	lightZ = 0;
	lightAmbientColor.set(ofFloatColor(1));
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlane() {
	doResetPlaneTransform();

	planeShiness.set(0.85);
	planeGlobalColor.set(ofFloatColor(1.f));
	planeDiffuseColor.set(ofFloatColor(0.6));
	planeSpecularColor.set(ofFloatColor(1));

	bShaderToPlane.set(false);
	bDisplaceToMaterial.set(false);

	bPlaneWireframe = false;
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlaneTransform() {
	planeSize.set(glm::vec2(0.09f, 0.09f));
	bPlaneInfinite = false;
	planeRotation.set(10.f);
	planePosition.set(-0.1f);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetShadow() {
	bDrawShadow.set(true);
	shadowBias.set(0.07);
	shadowNormalBias.set(-4.f);
	//shadowStrength.set(0.6f);
	//shadowSize.set(glm::vec2(0.25f,0.25f));
	bDebugShadow.set(false);
}

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::doResetCubeMap() {
	cubeMapMode = 2;
	bDrawCubeMap = true;
	cubeMapprefilterRoughness = 0.25f;

	if (bDrawBgAlt) bDrawBgAlt = false;
	bgAltColor.set(ofFloatColor(0.3));
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::doResetAll(bool bExcludeMaterial) {
	bDebug = false;

	doResetPlane();
	doResetLight();
	doResetShadow();

#ifdef SURFING__USE_CUBE_MAP
	doResetCubeMap();
#endif

#ifdef SURFING__USE_SHADER_AND_DISPLACERS
	doResetNoise();
	doResetDisplace();
#endif

	if (!bExcludeMaterial) material.doResetMaterial();

	resetTestScene.trigger();
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetMaterial() {
	material.doResetMaterial();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterial() {
	material.doRandomMaterial();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialSettings() {
	material.doRandomSettings();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColorGlobal() {
	material.doRandomColorGlobal();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColors() {
	material.doRandomColors();
};
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialColorsAlpha() {
	material.doRandomColorsAlpha();
}
//--------------------------------------------------------------
void ofxSurfingPBR::doRandomMaterialAlphas() {
	material.doRandomAlphas();
}