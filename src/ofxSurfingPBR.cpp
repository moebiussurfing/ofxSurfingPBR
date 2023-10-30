#include "ofxSurfingPBR.h"
//--------------------------------------------------------------
ofxSurfingPBR::ofxSurfingPBR() {
	ofAddListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofAddListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);
}

//--------------------------------------------------------------
ofxSurfingPBR::~ofxSurfingPBR() {
	ofRemoveListener(ofEvents().update, this, &ofxSurfingPBR::update);
	ofRemoveListener(parameters.parameterChangedE(), this, &ofxSurfingPBR::Changed);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetLight() {
	lightX = 0;
	lightY = SURFING__SZ_UNIT / 2.f;
	lightZ = 0;
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlane() {
	doResetPlaneTransform();
	planeShiness.set(0.85);
	globalColor.set(ofFloatColor(1.f));
	planeDiffuseColor.set(ofFloatColor(0.6));
	planeSpecularColor.set(ofFloatColor(1));
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlaneTransform() {
	planeSz.set(glm::vec2(0.5f, 0.5f));
	planeRot.set(10.f);
	planePos.set(-0.1f);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetShadow() {
	bDrawShadow.set(true);
	shadowBias.set(0.07);
	shadowNormalBias.set(-4.f);
}

#ifdef SURFING__USE_CUBE_MAP
//--------------------------------------------------------------
void ofxSurfingPBR::doResetcubeMap() {
	cubeMapMode = 2;
	bDrawCubeMap = true;
	cubeMapprefilterRoughness = 0.25f;
}
#endif

//--------------------------------------------------------------
void ofxSurfingPBR::doResetAll() {
	bDebug = false;

	doResetPlane();
	doResetLight();
	doResetShadow();

#ifdef SURFING__USE_CUBE_MAP
	doResetcubeMap();
#endif

	//material.doResetMaterial();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupParams() {
	parameters.setName("PBR_Scene");

	resetPlane.set("Reset Plane");
	resetPlaneTransform.set("Reset Transform");
	resetLight.set("Reset Light");
	resetShadow.set("Reset Shadow");
	resetAll.set("Reset All");

	planeParams.setName("Plane");
	planeTransform.setName("Transform");
	bDrawPlane.set("Draw Plane", true);
	planeSz.set("Size", glm::vec2(0.8f, 0.8f), glm::vec2(0, 0), glm::vec2(1, 1));
	planeRot.set("x Rotation", 0, -180, 180);
	planePos.set("y Position", 0, -1, 1);
	planeShiness.set("Shiness", 0.85, 0, 1);
	globalColor.set("Global Color", ofFloatColor(1.f), ofFloatColor(0), ofFloatColor(1));
	globalColor.setSerializable(false);
	planeDiffuseColor.set("Diffuse Color", ofFloatColor(0.6), ofFloatColor(0), ofFloatColor(1));
	planeSpecularColor.set("Specular Color", ofFloatColor(1), ofFloatColor(0), ofFloatColor(1));

	planeParams.add(bDrawPlane);
	planeParams.add(wireframe.set("Wireframe", false));
	planeTransform.add(planeSz);
	planeTransform.add(planeRot);
	planeTransform.add(planePos);
	planeTransform.add(resetPlaneTransform);
	planeParams.add(planeTransform);

	planeSettingsParams.setName("Settings");
	planeSettingsParams.add(globalColor);
	planeSettingsParams.add(planeShiness);
	planeSettingsParams.add(planeDiffuseColor);
	planeSettingsParams.add(planeSpecularColor);
	planeParams.add(planeSettingsParams);
	planeParams.add(resetPlane);

#ifdef SURFING__USE_DISPLACE
	//planeParams.add(useMaterial.set("Use Material", true));
	planeParams.add(noiseAmplitude.set("Displacement", 0.0f, 0.0f, 255.0f));
	planeParams.add(noiseScale.set("Noise Scale", 0.0f, 0.0f, 0.1f));
	planeParams.add(noiseSpeed.set("Noise Speed", 0.0f, 0.0f, 1.0f));
#endif

	parameters.add(planeParams);

	lightParams.setName("Light");
	float u = SURFING__SZ_UNIT;
	lightParams.add(lightX.set("X", 0.0f, -1 * u, 1 * u));
	lightParams.add(lightY.set("Y", 0.0f, -1 * u, 1 * u));
	lightParams.add(lightZ.set("Z", 0.0f, -1 * u, 1 * u));
	lightParams.add(resetLight);
	parameters.add(lightParams);

	shadowParams.setName("Shadow");
	shadowParams.add(bDrawShadow.set("Draw Shadow", true));
	shadowParams.add(shadowBias.set("Bias", 0.07, 0.0, 1.0));
	shadowParams.add(shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0));
	shadowParams.add(resetShadow);
	parameters.add(shadowParams);
}

//--------------------------------------------------------------
void ofxSurfingPBR::setup() {
	setupParams();

	// object
	material.setup();

	//--

#ifdef SURFING__USE_CUBE_MAP
	setupCubeMap();
#endif

	bDebug.set("Debug", false);
	parameters.add(bDebug);

	parameters.add(resetAll);

	//--

	// shader
#ifdef SURFING__USE_DISPLACE
	ofDisableArbTex();

	shader.load("shadersGL3/shader");
	#ifdef TARGET_OPENGLES
	OPENGL ES supports GL_RGBA32F but not GL_R32F
		img.allocate(80, 60, OF_IMAGE_COLOR_ALPHA);
	#else
	img.allocate(800, 600, OF_IMAGE_GRAYSCALE);
	#endif

	plane.mapTexCoordsFromTexture(img.getTexture());
#endif

	//--

	// lights and shadows

	for (int i = 0; i < 1; i++) {
		auto light = make_shared<ofLight>();
		light->enable();
		if (i == 0) {
			light->setPointLight();
			//light->getShadow().setNearClip(20);
			//light->getShadow().setFarClip(500);
			light->getShadow().setNearClip(20);
			light->getShadow().setFarClip(SURFING__SZ_UNIT);
		}

		//else {
		//	light->setSpotlight(60, 20);
		//	light->getShadow().setNearClip(200);
		//	light->getShadow().setFarClip(2000);
		//	light->setPosition(210, 330.0, 750);
		//	light->setAmbientColor(ofFloatColor(0.4));
		//}

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
	doResetAll();

	setupGui();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupGui() {
	gui.setup(parameters);
	gui.loadFromFile(path);

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
	for (auto & light : lights) {
		light->getShadow().setEnabled(bDrawShadow);
	}

	if (lights.size() > 0) {
		if (lights[0]->getType() == OF_LIGHT_POINT) {
			float tangle = ofGetElapsedTimef() * 1.05;
			lights[0]->setPosition(lightX, lightY, lightZ);
			//lights[0]->setAmbientColor(ofColor::red);
		}
	}

	ofShadow::setAllShadowBias(shadowBias.get());
	ofShadow::setAllShadowNormalBias(shadowNormalBias.get());
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
void ofxSurfingPBR::Changed(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "Changed " << name << " : " << e;

	if (name == planeSz.getName()) {
		float u = SURFING__SZ_UNIT * 20.f;
		int reso = 4;
		plane.set(u * planeSz.get().x, u * planeSz.get().y, reso, reso);
	}

	else if (name == planeRot.getName()) {
		glm::vec3 axis(1.0f, 0.0f, 0.0f);
		float angle = planeRot.get() - 90;
		glm::quat q = glm::angleAxis(glm::radians(angle), axis);
		plane.setOrientation(q);
	}

	else if (name == planePos.getName()) {
		plane.setPosition(0, planePos.get() * SURFING__SZ_UNIT * 5.f, 0);
	}

	else if (name == planeShiness.getName()) {
		materialPlane.setShininess(planeShiness.get() * 100);
	}

	else if (name == globalColor.getName()) {
		planeDiffuseColor = globalColor.get();
		planeSpecularColor = globalColor.get();
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
		//Open the Open File Dialog
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a exr or EXR file."); 
	
		//Check if the user opened a file
		if (openFileResult.bSuccess) {

			ofLogVerbose("ofxSurfingPBR") << ("User selected a file");

			//We have a file, check it and process it
			processOpenFileSelection(openFileResult);

		} else {
			ofLogVerbose("ofxSurfingPBR") << ("User hit cancel");
		}
	} 
	else if (name == resetCubeMap.getName()) {
		doResetcubeMap();
	} else if (name == cubeMapMode.getName()) {
		if (cubeMapMode == 1)
			cubeMapModeName = "Cube Map";
		else if (cubeMapMode == 2)
			cubeMapModeName = "Prefilter Roughness";
		else if (cubeMapMode == 3)
			cubeMapModeName = "Irradiance";
	}
#endif
}

//--------------------------------------------------------------
void ofxSurfingPBR::drawPlane() {
	if (!bDrawPlane) return;

	// plane
	if (wireframe) {
		plane.drawWireframe();
	} else {
		beginMaterialPlane();
		{
			plane.draw();
		}
		endMaterialPlane();
	}
}

//--------------------------------------------------------------
void ofxSurfingPBR::exit() {
	gui.saveToFile(path);
	material.exit();
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
	// params
	cubeMapMode.set("Mode", 2, 1, 3);
	cubeMapModeName.set("Type", "");
	cubeMapModeName.setSerializable(false);
	bDrawCubeMap.set("Draw CubeMap", true);
	cubeMapprefilterRoughness.set("Roughness", 0.25f, 0, 1.f);
	openCubeMap.set("Open File");
	resetCubeMap.set("Reset Cubemap");

	cubeMapParams.setName("CubeMap");
	cubeMapParams.add(bDrawCubeMap);
	cubeMapParams.add(cubeMapMode);
	cubeMapParams.add(cubeMapModeName);
	cubeMapParams.add(cubeMapprefilterRoughness);
	cubeMapParams.add(openCubeMap);
	cubeMapParams.add(resetCubeMap);

	parameters.add(cubeMapParams);

	//doResetcubeMap();

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