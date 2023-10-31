#include "ofxSurfingPBR.h"
//--------------------------------------------------------------
ofxSurfingPBR::ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "constructor()";

	ofAddListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofAddListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);
}

//--------------------------------------------------------------
ofxSurfingPBR::~ofxSurfingPBR() {
	ofLogNotice("ofxSurfingPBR") << "destructor()";

	ofRemoveListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofRemoveListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);
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

	bPlaneInfinite = true;
	bPlaneWireframe = false;
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlaneTransform() {
	planeSize.set(glm::vec2(0.5f, 0.5f));
	planeRotation.set(10.f);
	planePosition.set(-0.1f);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetShadow() {
	bDrawShadow.set(true);
	shadowBias.set(0.07);
	shadowNormalBias.set(-4.f);
}

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::doResetCubeMap() {
	cubeMapMode = 2;
	bDrawCubeMap = true;
	cubeMapprefilterRoughness = 0.25f;
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

	if (!bExcludeMaterial) material.doResetMaterial();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "setupParams()";

	parameters.setName("PBR_Scene");

	resetPlane.set("Reset Plane");
	resetPlaneTransform.set("Reset Transform");
	resetLight.set("Reset Light");
	resetShadow.set("Reset Shadow");
	resetAll.set("Reset All");

	planeParams.setName("Plane");
	planeTransform.setName("Transform");
	bDrawPlane.set("Draw Plane", true);
	planeSize.set("Size", glm::vec2(0.8f, 0.8f), glm::vec2(0, 0), glm::vec2(1, 1));
	planeRotation.set("x Rotation", 0, -45, 135);
	planePosition.set("y Position", 0, -1, 1);
	planeShiness.set("Shiness", 0.85, 0, 1);
	planeGlobalColor.set("Global Color", ofFloatColor(1.f), ofFloatColor(0), ofFloatColor(1));
	planeGlobalColor.setSerializable(false);
	planeDiffuseColor.set("Diffuse Color", ofFloatColor(0.6f), ofFloatColor(0), ofFloatColor(1));
	planeSpecularColor.set("Specular Color", ofFloatColor(1), ofFloatColor(0), ofFloatColor(1));

	planeParams.add(bDrawPlane);
	planeParams.add(bPlaneWireframe.set("Wireframe", false));
	planeTransform.add(planeSize);
	planeTransform.add(bPlaneInfinite.set("Infinite", true));
	planeTransform.add(planeRotation);
	planeTransform.add(planePosition);
	planeTransform.add(resetPlaneTransform);
	planeParams.add(planeTransform);

	planeSettingsParams.setName("Settings");
	planeSettingsParams.add(planeGlobalColor);
	planeSettingsParams.add(planeDiffuseColor);
	planeSettingsParams.add(planeSpecularColor);
	planeSettingsParams.add(planeShiness);
	planeParams.add(planeSettingsParams);
	planeParams.add(resetPlane);

#ifdef SURFING__USE_SHADER_AND_DISPLACE
	resetDisplace.set("Reset Displace");
	bShaderToPlane.set("ShaderToPlane", false);
	bDisplaceToMaterial.set("DisplaceToMaterial", false);
	displaceParams.setName("Displace");
	displaceParams.add(bShaderToPlane);
	displaceParams.add(bDisplaceToMaterial);
	noiseParams.setName("Noise");
	noiseParams.add(noiseAmplitude.set("Displace Amp", 0.0f, 0.0f, 255.0f));
	noiseParams.add(noiseScale.set("Noise Scale", 0.0f, 0.0f, 0.1f));
	noiseParams.add(noiseSpeed.set("Noise Speed", 0.0f, 0.0f, 1.0f));
	displaceParams.add(noiseParams);
	displaceParams.add(displacementStrength.set("Displace Strength", 0.0, 0.0, 500.0));
	displaceParams.add(displacementNormalsStrength.set("Displace Normals Strength", 0.0, 0.0, 1.0));
	displaceParams.add(normalGeomToNormalMapMix.set("Normal GeomToNormMapMix", 0.0, 0.0, 1.0));
	displaceParams.add(resetDisplace);
	planeParams.add(displaceParams);
#endif

	parameters.add(planeParams);

	lightParams.setName("Light");
	lightAmbientColor.set("Light Ambient Color", ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
	float u = 2 * SURFING__SZ_UNIT;
	lightParams.add(lightX.set("X", 0.0f, -u, u));
	lightParams.add(lightY.set("Y", 0.0f, -u, u));
	lightParams.add(lightZ.set("Z", 0.0f, -u, u));
	lightParams.add(lightAmbientColor);
	lightParams.add(resetLight);
	parameters.add(lightParams);

	shadowParams.setName("Shadow");
	shadowParams.add(bDrawShadow.set("Draw Shadow", true));
	shadowParams.add(shadowBias.set("Bias", 0.07, 0.0, 1.0));
	shadowParams.add(shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0));
	shadowParams.add(resetShadow);
	parameters.add(shadowParams);

	//--

#ifdef SURFING__USE_CUBE_MAP
	setupCubeMap();
#endif

	//--

	bDebug.set("Debug", false);
	bAutoSave.set("Autosave", true);
	bKeys.set("Keys", true);

	//link
	//TODO: not working?
	material.bAutoSave.makeReferenceTo(bAutoSave);
	//bAutoSave.makeReferenceTo(material.bAutoSave);

	parameters.add(bDebug);
	parameters.add(bKeys);
	parameters.add(bAutoSave);
	parameters.add(resetAll);
}

//--

#ifdef SURFING__USE_SHADER_AND_DISPLACE
//--------------------------------------------------------------
void ofxSurfingPBR::refreshImg() {
	ofLogNotice("ofxSurfingPBR") << "refreshImg()";

	float w, h;
	//w = 800;
	//h = 600;

	w = plane.getWidth();
	h = plane.getHeight();

	////clamp
	//w = ofClamp(w, 0, 1024);
	//h = ofClamp(h, 0, 1024);

	#ifdef TARGET_OPENGLES
	OPENGL ES supports GL_RGBA32F but not GL_R32F
		img.allocate(80, 60, OF_IMAGE_COLOR_ALPHA);
	#else
	//img.clear();
	img.allocate(w, h, OF_IMAGE_GRAYSCALE);
	#endif

	// apply to plane
	//plane.set(800, 600, 80, 60);
	plane.mapTexCoordsFromTexture(img.getTexture());

	ofLogNotice("ofxSurfingPBR") << "w,h: " << w << "," << h;
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupShader() {
	ofLogNotice("ofxSurfingPBR") << "setupShader()";

	// shader
	ofDisableArbTex();

	bShaderReady = shader.load("shadersGL3/shader");

	refreshImg();

	doResetDisplace();
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetDisplace() {
	ofLogNotice("ofxSurfingPBR") << "doResetDisplace()";

	noiseAmplitude.set(0.0f);
	noiseScale.set(0.0f);
	noiseSpeed.set(0.0f);

	displacementStrength.set(0.0f);
	displacementNormalsStrength.set(0.0f);
	normalGeomToNormalMapMix.set(0.0f);

	bShaderToPlane.set(false);
	bDisplaceToMaterial.set(false);
}

//--------------------------------------------------------------
void ofxSurfingPBR::updateDisplace() {

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

	if (bDisplaceToMaterial) {
		material.material.setDisplacementTexture(img.getTexture());
		material.material.setDisplacementStrength(displacementStrength);
		material.material.setDisplacementNormalsStrength(displacementNormalsStrength);

		material.material.setNormalGeomToNormalMapMix(normalGeomToNormalMapMix);

		//material.material.setTexCoordScale(scaleX, scaleY);
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::beginShader() {
	shader.begin();
	shader.setUniformTexture("displacement", img.getTexture(), 1);
}
//--------------------------------------------------------------
void ofxSurfingPBR::endShader() {
	shader.end();
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::setup() {
	ofLogNotice("ofxSurfingPBR") << "setup()";

	// for using on any objects
	material.setup();

	setupParams();

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACE
	setupShader();
#endif

	//--

	// lights and shadows

	int amountLights = 1; //currently limited!

	for (int i = 0; i < amountLights; i++) {
		auto light = make_shared<ofLight>();
		light->enable();
		if (i == 0) {
			light->setPointLight();
			light->getShadow().setNearClip(20);
			light->getShadow().setFarClip(SURFING__SZ_UNIT);
		}

		else {
			light->setSpotlight(60, 20);
			light->getShadow().setNearClip(200);
			light->getShadow().setFarClip(2000);
			light->setPosition(210, 330.0, 750);
			light->setAmbientColor(ofFloatColor(0.4));
		}

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
	gui.getGroup(planeParams.getName()).getGroup(planeTransform.getName()).minimize();
	//gui.getGroup(lightParams.getName()).minimize();
	gui.getGroup(shadowParams.getName()).minimize();

#ifdef SURFING__USE_CUBE_MAP
	gui.getGroup(cubeMapParams.getName()).minimize();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void ofxSurfingPBR::update() {

	// autosave workflow
	auto t = ofGetElapsedTimeMillis() - timeLastChange;
	if (bAutoSave && bFlagSave && t > timeSaveGap) {
		bFlagSave = false;
		save();
	}

	//--

	for (auto & light : lights) {
		light->getShadow().setEnabled(bDrawShadow);
	}

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

#ifdef SURFING__USE_SHADER_AND_DISPLACE
	updateDisplace();
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

	ofDisableDepthTest();

	//pos
	auto p = gui.getShape().getTopRight() + glm::vec2(2, 0);
	material.gui.setPosition(p);

	gui.draw();
	material.drawGui();

	//--

#ifdef SURFING__USE_SHADER_AND_DISPLACE
	if (bDebug) {
		int pad = 10;
		int x, y, w, h;
		w = img.getWidth();
		h = img.getHeight();
		x = ofGetWidth() - pad;
		y = ofGetHeight() - pad;
		//auto r = ofRectangle(x, y, w, h);
		img.draw(x, y);
	}
#endif

	if (bDebug) {
		float fps = ofGetFrameRate();
		string s = ofToString(fps, 1);
		s += " FPS";
		//TODO: something fails. requires offset to fix
		ofBitmapFont bf;
		auto bb = bf.getBoundingBox(s, 0, 0);
		ofDrawBitmapStringHighlight(s, 4, 8 + ofGetWindowHeight() - bb.getHeight());
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
					//renderScene();
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
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		// Should fix bc makes some models "transparent"...
		// sets the orientation for front-facing
		// polygons1GL_CW means that polygons with vertices
		// in clockwise order on the screen are considered front-facing1.
		glCullFace(GL_BACK);
		{
			//renderScene();
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
				if (bDebug) light->getShadow().drawFrustum();
			}
		}

		//--

#ifdef SURFING__USE_CUBE_MAP
		if (bDrawCubeMap) {
			// drawing of the cube map renders at max depth, so it can be drawn last
			// this will allow for the benefit of depth clipping
			if (cubeMapMode == 2) {
				if (cubeMap.hasPrefilteredMap()) {
					cubeMap.drawPrefilteredCube(cubeMapprefilterRoughness.get());
				} else {
					ofLogNotice("ofxSurfingPBR") << "DOES NOT HAVE PRE FILTERED CUBE MAP " << ofGetFrameNum();
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
	float szU = SURFING__SZ_UNIT * (bPlaneInfinite ? (float)SURFING__PLANE_INFINITE_MAGNITUDE : 20.f);

	float w = szU * planeSize.get().x;
	float h = szU * planeSize.get().y;

//TODO
#ifdef SURFING__USE_SHADER_AND_DISPLACE
	//hardcode
	w = 800;
	h = 600;
#endif

	//TODO: make it simple..
#ifndef SURFING__USE_SHADER_AND_DISPLACE
	int resX = (int)SURFING__PLANE_RESOLUTION;
	int resY = (int)SURFING__PLANE_RESOLUTION;
#else
	int resX = w / 10.f;
	int resY = h / 10.f;
#endif

	plane.set(w, h, resX, resY);

#ifdef SURFING__USE_SHADER_AND_DISPLACE
	refreshImg();
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::Changed(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "Changed " << name << ": " << e;

	//for every modified param, flag to save on the next frame but after a time gap.
	if (bAutoSave) {
		bFlagSave = true;
		timeLastChange = ofGetElapsedTimeMillis();
	}

	if (name == planeSize.getName()) {
		refreshPlane();
	} else if (name == bPlaneInfinite.getName()) {
		planeSize = planeSize.get(); //refresh
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

#ifdef SURFING__USE_CUBE_MAP
	else if (name == openCubeMap.getName()) {
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a exr or EXR file.");
		if (openFileResult.bSuccess) {
			ofLogVerbose("ofxSurfingPBR") << ("User selected a file");
			processOpenFileSelection(openFileResult);

		} else {
			ofLogVerbose("ofxSurfingPBR") << ("User hit cancel");
		}
	} else if (name == resetCubeMap.getName()) {
		doResetCubeMap();
	} else if (name == cubeMapMode.getName()) {
		//TODO:
		//return;//fix crash
		if (cubeMapMode.get() == 1)
			cubeMapModeName.set(ofToString("Cube Map"));
		else if (cubeMapMode.get() == 2)
			cubeMapModeName.set(ofToString("Prefilter Roughness"));
		else if (cubeMapMode.get() == 3)
			cubeMapModeName.set(ofToString("Irradiance"));
	}
#endif

#ifdef SURFING__USE_SHADER_AND_DISPLACE
	else if (name == resetDisplace.getName()) {
		doResetDisplace();
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawTestScene() {
	// do once
	static bool b = false;
	if (!b) {
		b = true;
		ofSetConeResolution(30, 10, 2);
	}

	ofPushMatrix();
	ofTranslate(-200, 100, 0);
	ofRotateXDeg(180);
	ofDrawCone(0, 0, 0, 65, 100);
	ofPopMatrix();

	ofPushMatrix();
	float spd = 240;
	ofRotateYDeg(360.f * (ofGetFrameNum() % (int)spd) / spd);
	ofDrawBox(0, 100, 0, 100);
	ofPopMatrix();

	ofDrawSphere(200, 100, 0, 50);
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPlane() {
	if (!bDrawPlane) return;

	// plane
	if (bPlaneWireframe) {
		plane.drawWireframe();
	}

	else {
#ifdef SURFING__USE_SHADER_AND_DISPLACE
		if (bShaderToPlane)
			beginShader();
		else
			beginMaterialPlane();
		{
			plane.draw();
		}
		if (bShaderToPlane)
			endShader();
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
	gui.saveToFile(path);
}
//--------------------------------------------------------------
void ofxSurfingPBR::load() {
	ofLogNotice("ofxSurfingPBR") << "Load: " << path;
	gui.loadFromFile(path);
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

	if (path == "") //use default path
		csettings.filePath = path_Cubemaps + "/" + path_CubemapFilename;
	else
		csettings.filePath = path;

	// uncomment to load from a cache or make one if it doesn't exist
	//	csettings.useCache = true;

	// uncomment to use the maximum precision available. OpenGL ES is float16 and OpenGL is float32;
	//	csettings.useMaximumPrecision = true;

	// make sure the defaults are the same for making and loading the cache
	// ie opengl es defaults are smaller and the file names to load are based on the resolution
	csettings.irradianceRes = 32;
	csettings.preFilterRes = 128;

	bool b = cubeMap.load(csettings);
	// below is another method for loading a cube map without passing settings class
	//	cubeMap.load("modern_buildings_2_1k.exr", 512, true );

	if (!b)
		ofLogError("ofxSurfingPBR") << "Error loading cubemap: " << csettings.filePath;
	else
		ofLogNotice("ofxSurfingPBR") << "Successfully loaded cubemap: " << csettings.filePath;

	return b;
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupCubeMap() {
	ofLogNotice("ofxSurfingPBR") << "setupCubeMap()";

	// params
	cubeMapMode.set("Mode", 2, 1, 3);
	cubeMapModeName.set("Type", "NONE");
	cubeMapModeName.setSerializable(false);
	bDrawCubeMap.set("Draw CubeMap", true);
	cubeMapprefilterRoughness.set("Roughness", 0.25f, 0, 1.f);
	openCubeMap.set("Open File");
	resetCubeMap.set("Reset CubeMap");

	cubeMapParams.setName("CubeMap");
	cubeMapParams.add(bDrawCubeMap);
	cubeMapParams.add(cubeMapMode);
	cubeMapParams.add(cubeMapModeName);
	cubeMapParams.add(cubeMapprefilterRoughness);
	cubeMapParams.add(openCubeMap);
	cubeMapParams.add(resetCubeMap);

	parameters.add(cubeMapParams);

	//doResetCubeMap();

	//--

	loadCubeMap();

	ofEnableArbTex();
}

void ofxSurfingPBR::processOpenFileSelection(ofFileDialogResult openFileResult) {

	ofLogVerbose("ofxSurfingPBR") << ("getName(): " + openFileResult.getName());
	ofLogVerbose("ofxSurfingPBR") << ("getPath(): " + openFileResult.getPath());

	ofFile file(openFileResult.getPath());

	if (file.exists()) {
		ofLogVerbose("ofxSurfingPBR") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		if (fileExtension == "exr" || fileExtension == "EXR") {

			loadCubeMap(openFileResult.getPath());
		}
	}
}
#endif

void ofxSurfingPBR::keyPressed(int key) {
	if (!bKeys) return;

	if (key == 'd') bDebug = !bDebug;
	if (key == 'p') bDrawPlane = !bDrawPlane;
	if (key == 'c') bDrawCubeMap = !bDrawCubeMap;

	if (key == OF_KEY_F1) doRandomMaterialColors();
	if (key == OF_KEY_F2) doRandomMaterialSettings();
	if (key == OF_KEY_F3) doRandomMaterial();
	if (key == OF_KEY_F4) doResetMaterial();
}