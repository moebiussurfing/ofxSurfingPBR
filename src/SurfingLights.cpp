#include "SurfingLights.h"

//--------------------------------------------------------------
SurfingLights::SurfingLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:SurfingLights()";

	ofAddListener(ofEvents().update, this, &SurfingLights::update);
}

//--------------------------------------------------------------
SurfingLights::~SurfingLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:~SurfingLights()";

	ofRemoveListener(ofEvents().update, this, &SurfingLights::update);

	ofRemoveListener(params_Lights.parameterChangedE(), this, &SurfingLights::ChangedLights);
	//ofRemoveListener(params_Brights.parameterChangedE(), this, &SurfingLights::ChangedBrights);
	//ofRemoveListener(parameters.parameterChangedE(), this, &SurfingLights::Changed);

	ofRemoveListener(shadowParams.parameterChangedE(), this, &SurfingLights::ChangedShadow);
}

//--

//--------------------------------------------------------------
void SurfingLights::setFunctionRenderScene(callback_t f) {
	f_RenderScene = f;
}

//--------------------------------------------------------------
void SurfingLights::setupLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupLights()";

	lights.clear();

	/*
	OF_LIGHT_POINT=0,
	OF_LIGHT_DIRECTIONAL=1,
	OF_LIGHT_SPOT=2,
	OF_LIGHT_AREA=3 
	*/

	// POINT
	{
		auto light = make_shared<ofLight>();
		light->enable();

		light->setPointLight();

		light->getShadow().setNearClip(SURFING__PBR__LIGHTS_NEAR);
		light->getShadow().setFarClip(SURFING__PBR__LIGHTS_FAR);

		light->getShadow().setStrength(SURFING__PBR__SHADOW_STRENGTH);

		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	// DIRECTIONAL
	{
		auto light = make_shared<ofLight>();
		light->enable();

		light->setDirectional();

		light->getShadow().setNearClip(SURFING__PBR__LIGHTS_NEAR);
		light->getShadow().setFarClip(SURFING__PBR__LIGHTS_FAR);

		light->getShadow().setStrength(SURFING__PBR__SHADOW_STRENGTH);

		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	// SPOT
	{
		auto light = make_shared<ofLight>();
		light->enable();

		light->setSpotlight();

		light->getShadow().setNearClip(SURFING__PBR__LIGHTS_NEAR);
		light->getShadow().setFarClip(SURFING__PBR__LIGHTS_FAR);

		light->getShadow().setStrength(SURFING__PBR__SHADOW_STRENGTH);

		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	ofShadow::enableAllShadows();
	ofShadow::setAllShadowTypes(OF_SHADOW_TYPE_PCF_LOW);

	//----

	/*
	const int OFAPP_NUM_LIGHTS = 1; 

	for (int i = 0; i < OFAPP_NUM_LIGHTS; i++) {
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
		light->getShadow().setStrength(SURFING__PBR__SHADOW_STRENGTH);

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
	*/
}

//--------------------------------------------------------------
void SurfingLights::updateLights() {

	// POINT
	{
		lights[0]->getShadow().setEnabled(bDrawShadow);

		if (bPoint) {
			if (!lights[0]->getIsEnabled()) lights[0]->enable();
		} else {
			if (lights[0]->getIsEnabled()) lights[0]->disable();
		}

		lights[0]->setPosition(pointLightPosition);

		lights[0]->setAmbientColor(pointLightAmbientColor);
		lights[0]->setDiffuseColor(pointLightDiffuseColor);
		lights[0]->setSpecularColor(pointLightSpecularColor);
	}

	//--

	// DIRECTIONAL
	{
		lights[1]->getShadow().setEnabled(bDrawShadow);

		if (bDirectional) {
			if (!lights[1]->getIsEnabled()) lights[1]->enable();
		} else {
			if (lights[1]->getIsEnabled()) lights[1]->disable();
		}

		lights[1]->setPosition(directionalLightPosition);

		lights[1]->setAmbientColor(directionalLightAmbientColor);
		lights[1]->setDiffuseColor(directionalLightDiffuseColor);
		lights[1]->setSpecularColor(directionalLightSpecularColor);

		glm::vec3 rad = glm::radians(directionalLightOrientation.get());
		glm::quat q = glm::quat(rad);
		lights[1]->setOrientation(q);
	}

	//--

	// SPOT
	{
		lights[2]->getShadow().setEnabled(bDrawShadow);

		if (bSpot) {
			if (!lights[2]->getIsEnabled()) lights[2]->enable();
		} else {
			if (lights[2]->getIsEnabled()) lights[2]->disable();
		}

		lights[2]->setPosition(spotLightPosition);

		lights[2]->setAmbientColor(spotLightAmbientColor);
		lights[2]->setDiffuseColor(spotLightDiffuseColor);
		lights[2]->setSpecularColor(spotLightSpecularColor);

		glm::vec3 rad = glm::radians(spotLightOrientation.get());
		glm::quat q = glm::quat(rad);
		lights[2]->setOrientation(q);
	}

	//--

	ofShadow::setAllShadowBias(shadowBias.get());
	ofShadow::setAllShadowNormalBias(shadowNormalBias.get());
}

//--------------------------------------------------------------
void SurfingLights::drawLights() {
	//if (f_RenderScene == nullptr) return;

	ofPushStyle();
	ofSetColor(ofColor::white);

	ofEnableDepthTest();

	// Compute shadows passes

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

	ofPopStyle();
}

//--------------------------------------------------------------
void SurfingLights::drawDebugLights() {
	if (!bDebug) return;
	if (!bDebugLights) return;

	ofPushStyle();

	for (int i = 0; i < lights.size(); i++) {

		if (i == 0 && !bPoint)
			continue;
		else if (i == 1 && !bDirectional)
			continue;
		else if (i == 2 && !bSpot)
			continue;

		//--

		auto & light = lights[i];

		ofSetColor(light->getDiffuseColor());

		if (light->getType() == OF_LIGHT_POINT) {
			ofDrawSphere(light->getPosition(), 10);
		}

		else {
			light->draw();
		}

		if (light->getShadow().getIsEnabled()) {
			if (bDebugShadow) light->getShadow().drawFrustum();
		}
	}

	ofPopStyle();
}

////TODO
////--------------------------------------------------------------
//void SurfingLights::drawDebug() {
//	// draw debug
//	if (bDebugLights) {
//		ofPushStyle();
//
//		if (bPoint) {
//			ofSetColor(pointLight.getDiffuseColor());
//			pointLight.draw();
//		}
//
//		if (bSpot) {
//			ofSetColor(spotLight.getDiffuseColor());
//			spotLight.draw();
//		}
//
//		ofPopStyle();
//	}
//}

////--------------------------------------------------------------
//void SurfingLights::doResetLight() {
//	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetLight()";
//
//	lightX = 0;
//	lightY = SURFING__SCENE_SIZE_UNIT * 0.5;
//	lightZ = 0;
//	lightAmbientColor.set(ofFloatColor(1.f, 1.f));
//}

//--

//--------------------------------------------------------------
void SurfingLights::setGuiPosition(glm::vec2 pos) {
	gui.setPosition(pos.x, pos.y);
}

//--------------------------------------------------------------
void SurfingLights::drawGui() {
	if (!bGui) return;
	gui.draw();

	if (bGui_Shadows) {
		glm::vec2 p;
		p = gui.getShape().getTopRight();
		p += glm::vec2 { (float)SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0.f };
		guiShadows.setPosition(p.x, p.y);
		guiShadows.draw();
	}
}

//--------------------------------------------------------------
void SurfingLights::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshGui()";

	//--

	//gui.getGroup(params_Lights.getName())
	//	.getGroup(params_Brights.getName())
	//	.minimize();

	//--

	// Point

	gui.getGroup(params_Lights.getName())
		.getGroup(params_Extra.getName())
		.minimize();

	gui.getGroup(params_Lights.getName())
		.getGroup(pointParams.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(pointParams.getName())
		.getGroup(pointLightPosition.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(pointParams.getName())
		.getGroup(pointColorsParams.getName())
		.minimize();
	auto & gp = gui.getGroup(params_Lights.getName())
					.getGroup(pointParams.getName());
	if (bPoint)
		gp.maximize();
	else
		gp.minimize();

	//--

	// Spot

	gui.getGroup(params_Lights.getName())
		.getGroup(spotLightParams.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(spotLightParams.getName())
		.getGroup(spotLightPosition.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(spotLightParams.getName())
		.getGroup(spotLightOrientation.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(spotLightParams.getName())
		.getGroup(spotLightColorsParams.getName())
		.minimize();
	auto & gs = gui.getGroup(params_Lights.getName())
					.getGroup(spotLightParams.getName());
	if (bSpot)
		gs.maximize();
	else
		gs.minimize();

	//--

	// Directional

	gui.getGroup(params_Lights.getName())
		.getGroup(directionalLightParams.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(directionalLightParams.getName())
		.getGroup(directionalLightPosition.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(directionalLightParams.getName())
		.getGroup(directionalLightOrientation.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(directionalLightParams.getName())
		.getGroup(directionalColorsParams.getName())
		.minimize();
	auto & gd = gui.getGroup(params_Lights.getName())
					.getGroup(directionalLightParams.getName());
	if (bDirectional)
		gd.maximize();
	else
		gd.minimize();

	//--
}

//--------------------------------------------------------------
void SurfingLights::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupGui()";

	// lights
	gui.setup(parameters);

	// shadows
	guiShadows.setup(shadowParams);

	refreshGui();
}

//--------------------------------------------------------------
void SurfingLights::setup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setup()";

	pointLight.setPointLight();
	spotLight.setSpotlight();
	directionalLight.setDirectional();

	setupParameters();

	setupGui();
}

//--------------------------------------------------------------
void SurfingLights::setupParameters() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupParameters()";

	float u = 1.f * SURFING__SCENE_SIZE_UNIT;

	bGui.set("UI LIGHTS", true);

	bAnimLights.set("Anim Lights", false);
	bMouseLights.set("Mouse Lights", false);

	bDebug.set("Debug", true);
	bDebugShadow.set("Debug Shadow", true);
	bDebugLights.set("Debug Lights", true);

	//--

	vResetLights.set("Reset Lights");

	//--

	// Groups
	parameters.setName("PBR_LIGHTS");
	params_Lights.setName("Lights");
	params_Enablers.setName("Enable");
	params_Extra.setName("Extra");

	pointParams.setName("Light Point");
	spotLightParams.setName("Light Spot");
	directionalLightParams.setName("Light Direct");

	//params_Brights.setName("Power");

	pointColorsParams.setName("p Colors");
	directionalColorsParams.setName("d Colors");
	spotLightColorsParams.setName("s Colors");

	//--

	// Point

	bPoint.set("Point", false);
	vPointReset.set("Reset Point");
	//pointBright.set("Bright Point", 0.1, 0, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	pointLightPosition.set("p Position", glm::vec3(), glm::vec3(-u), glm::vec3(u));

	pointLightAmbientColor.set("p Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointLightDiffuseColor.set("p Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointLightSpecularColor.set("p Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	pointLightGlobalColor.set("p Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	pointLightGlobalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	//--

	// Directional

	bDirectional.set("Direct", false);
	vDirectionalReset.set("Reset Direct");
	//directionalBright.set("Bright Dir", 0.1, 0, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	directionalLightPosition.set("d Position", glm::vec3(0), glm::vec3(-u), glm::vec3(u));
	directionalLightOrientation.set("d Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));

	directionalLightAmbientColor.set("d Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalLightDiffuseColor.set("d Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalLightSpecularColor.set("d Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	directionalLightGlobalColor.set("d Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	directionalLightGlobalColor.set("d Global",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	//--

	// Spot

	bSpot.set("Spot", false);
	vSpotReset.set("Reset Spot");
	//spotBright.set("Bright Spot", 0.1, 0, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	spotLightPosition.set("s Position", glm::vec3(), glm::vec3(-u), glm::vec3(u));

	spotLightAmbientColor.set("s Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotLightDiffuseColor.set("s Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotLightSpecularColor.set("s Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	spotLightGlobalColor.set("s Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	spotLightGlobalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	spotLightOrientation.set("s Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));

	//TODO
	spotLightCutOff.set("s CutOff", 1, 0, 90);
	spotLightConcentration.set("s Concentration", 1, 0, 128);

	//--

	pointLightGlobalColor.set(ofFloatColor(1));
	directionalLightGlobalColor.set(ofFloatColor(1));
	spotLightGlobalColor.set(ofFloatColor(1));

	//--

	bSmoothLights.set("Smooth Lights", false); // Default Low Poly

	//--

	// Lights

	pointParams.add(pointLightGlobalColor);
	pointColorsParams.add(pointLightAmbientColor);
	pointColorsParams.add(pointLightDiffuseColor);
	pointColorsParams.add(pointLightSpecularColor);
	pointParams.add(pointColorsParams);
	pointParams.add(pointLightPosition);
	pointParams.add(vPointReset);

	directionalLightParams.add(directionalLightGlobalColor);
	directionalColorsParams.add(directionalLightAmbientColor);
	directionalColorsParams.add(directionalLightDiffuseColor);
	directionalColorsParams.add(directionalLightSpecularColor);
	directionalLightParams.add(directionalColorsParams);
	directionalLightParams.add(directionalLightPosition);
	directionalLightParams.add(directionalLightOrientation);
	directionalLightParams.add(vDirectionalReset);

	spotLightParams.add(spotLightGlobalColor);
	spotLightColorsParams.add(spotLightAmbientColor);
	spotLightColorsParams.add(spotLightDiffuseColor);
	spotLightColorsParams.add(spotLightSpecularColor);
	spotLightParams.add(spotLightColorsParams);
	spotLightParams.add(spotLightPosition);
	spotLightParams.add(spotLightOrientation);

	spotLightParams.add(spotLightCutOff);
	spotLightParams.add(spotLightConcentration);

	spotLightParams.add(vSpotReset);

	// main enablers
	params_Enablers.add(bPoint);
	params_Enablers.add(bDirectional);
	params_Enablers.add(bSpot);
	params_Lights.add(params_Enablers);

	//params_Brights.add(pointBright);
	//params_Brights.add(directionalBright);
	//params_Brights.add(spotBright);
	//params_Lights.add(params_Brights);

	params_Lights.add(pointParams);
	params_Lights.add(directionalLightParams);
	params_Lights.add(spotLightParams);

	params_Extra.add(bDebug);
	params_Extra.add(bDebugLights);
	params_Extra.add(bDebugShadow);

	params_Extra.add(bSmoothLights);
	params_Extra.add(bAnimLights);
	params_Extra.add(bMouseLights);
	params_Extra.add(vResetLights);
	params_Lights.add(params_Extra);

	//-

	parameters.add(bGui_Shadows);
	parameters.add(params_Lights);

	pointLightGlobalColor.setSerializable(false);
	spotLightGlobalColor.setSerializable(false);
	directionalLightGlobalColor.setSerializable(false);

	//----

	// Shadows

	bGui_Shadows.set("UI SHADOWS", true);
	bDrawShadow.set("Draw Shadow", true);
	shadowBias.set("Bias", 0.07, 0.0, 1.0);
	shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0);
	bDebugShadow.set("Debug Shadow", false);
	vResetShadow.set("Reset Shadow");

	shadowParams.setName("PBR_SHADOWS");
	shadowParams.add(bDrawShadow);
	shadowParams.add(shadowBias);
	shadowParams.add(shadowNormalBias);

	//TODO
	//shadowParams.add(shadowStrength.set("Strength", 0.6f, 0.f, 1.f));
	//shadowParams.add(shadowSize.set("Shadow Size", glm::vec2(0.25f, 0.25f), glm::vec2(0, 0), glm::vec2(1.f, 1.f)));

	shadowParams.add(bDebugShadow);
	shadowParams.add(vResetShadow);

	//-

	// Callbacks
	ofAddListener(params_Lights.parameterChangedE(), this, &SurfingLights::ChangedLights);
	//ofAddListener(params_Brights.parameterChangedE(), this, &SurfingLights::ChangedBrights);
	//ofAddListener(parameters.parameterChangedE(), this, &SurfingLights::Changed);

	ofAddListener(shadowParams.parameterChangedE(), this, &SurfingLights::ChangedShadow);

	//--

	// Startup
	startup();
}

//--------------------------------------------------------------
void SurfingLights::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:startup()";

	doResetLights();

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	// auto saver
	callback_t f = std::bind(&SurfingLights::save, this);
	//register the local save function to be called when saving is required.
	autoSaver.setFunctionSaver(f);
#endif

	//--

	// Settings
	//TODO
	bool bNotFound = true;
	//bNotFound |= !ofxSurfing::loadGroup(params_Lights, pathSettings);
	//bNotFound |= !ofxSurfing::loadGroup(paramsScene, pathSettings_Bg);

	load();

	//TODO:
	// Settings file not found!
	// Should trig the callbacks to update engine!
	if (bNotFound) {
		ofLogError("ofxSurfingPBR") << "SurfingLights: Settings files not found!";
		ofLogError("ofxSurfingPBR") << "SurfingLights: Initializing...";

		bPoint = true;
		bSpot = true;
		bDirectional = true;

		doResetLights();

		bDebug = true;
		bDebugLights = true;
		bDebugShadow = false;
	}

	//fix startup callbacks?
	//bFlagUpdateLights = true;
}
//--------------------------------------------------------------
void SurfingLights::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void SurfingLights::update() {
	if (bFlagUpdateLights) {
		bFlagUpdateLights = false;
	}

	//TODO:
	//updateAnims();
}
//--------------------------------------------------------------
void SurfingLights::updateAnims() {

	// Anim by time
	if (bAnimLights) {

		const float ratio = 3.f;
		float sizeScene = 1.f;

		pointLight.setPosition(
			cos(ofGetElapsedTimef() * .6f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + pointLightPosition.get().x,
			sin(ofGetElapsedTimef() * .8f) * SURFING__SCENE_SIZE_UNIT * sizeScene + pointLightPosition.get().y,
			-cos(ofGetElapsedTimef() * .8f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + pointLightPosition.get().z);

		spotLight.setPosition(
			cos(ofGetElapsedTimef() * .6f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + spotLightPosition.get().x,
			sin(ofGetElapsedTimef() * .8f) * SURFING__SCENE_SIZE_UNIT * sizeScene + spotLightPosition.get().y,
			-cos(ofGetElapsedTimef() * .8f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + spotLightPosition.get().z);

		directionalLight.setPosition(
			cos(ofGetElapsedTimef() * .6f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + directionalLightPosition.get().x,
			sin(ofGetElapsedTimef() * .8f) * SURFING__SCENE_SIZE_UNIT * sizeScene + directionalLightPosition.get().y,
			-cos(ofGetElapsedTimef() * .8f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + directionalLightPosition.get().z);
	}

	// Move by mouse
	if (bMouseLights) {
		mouseX = ofGetMouseX();
		mouseY = ofGetMouseY();

		// move by mouse
		spotLight.setPosition(spotLightPosition.get().z, mouseX, mouseY);
		spotLight.setOrientation(ofVec3f(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));
		//spotLight.setPosition(mouseX, mouseY, spotLightPosition.get().z);
	}
}

//--------------------------------------------------------------
void SurfingLights::doResetLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetLights()";

	if (!bPoint) bPoint = true;
	if (bSpot) bSpot = false;
	if (bDirectional) bDirectional = false;

	doResetPoint();
	doResetDirectional();
	doResetSpot();

	//refreshBrights();
}

// Point
//--------------------------------------------------------------
void SurfingLights::doResetPoint() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetPoint()";

	//bPoint = false;

	//pointLightGlobalColor.set(ofFloatColor(1));

	//pointLightGlobalColor.setWithoutEventNotifications(ofFloatColor(1));
	//pointLightAmbientColor.set(ofFloatColor(1));
	//pointLightDiffuseColor.set(ofFloatColor(1));
	//pointLightSpecularColor.set(ofFloatColor(1));

	pointLightPosition.set(glm::vec3(
		SURFING__SCENE_SIZE_UNIT * 0.2f,
		SURFING__SCENE_SIZE_UNIT,
		SURFING__SCENE_SIZE_UNIT * 0.1f));

	//pointBright = 0.1;
}

// Directional
//--------------------------------------------------------------
void SurfingLights::doResetDirectional() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetDirectional()";

	//bDirectional = false;

	//directionalLightGlobalColor.set(ofFloatColor(1));

	//directionalLightGlobalColor.setWithoutEventNotifications(ofFloatColor(1));
	//directionalLightAmbientColor.set(ofFloatColor(1));
	//directionalLightDiffuseColor.set(ofFloatColor(1));
	//directionalLightSpecularColor.set(ofFloatColor(1));

	directionalLightPosition.set(glm::vec3(0,
		SURFING__SCENE_SIZE_UNIT * 1.f,
		SURFING__SCENE_SIZE_UNIT * 0.5f));

	directionalLightOrientation.set(glm::vec3(-60, 0, 0));

	//directionalBright = 0.1f;
}

// Spot
//--------------------------------------------------------------
void SurfingLights::doResetSpot() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetSpot()";

	//bSpot = false;

	//spotLightGlobalColor.set(ofFloatColor(1));

	//spotLightGlobalColor.setWithoutEventNotifications(ofFloatColor(1));
	//spotLightAmbientColor.set(ofFloatColor(1));
	//spotLightDiffuseColor.set(ofFloatColor(1));
	//spotLightSpecularColor.set(ofFloatColor(1));

	spotLightPosition.set(glm::vec3(
		-SURFING__SCENE_SIZE_UNIT * 0.2f,
		SURFING__SCENE_SIZE_UNIT,
		SURFING__SCENE_SIZE_UNIT * 0.3f));

	spotLightOrientation.set(glm::vec3(-60, 0, 0));

	//TODO
	spotLightCutOff.set(90);
	spotLightConcentration.set(0);

	//spotBright = 0.1;
}

//--

////--------------------------------------------------------------
//void SurfingLights::Changed(ofAbstractParameter & e) {
//	string name = e.getName();
//
//	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Changed" << name << " : " << e;
//
//	if (0) {
//	}
//
//	//// Minimize
//	//else if (name == bMini_Scene.getName()) {
//	//	//setupImGuiStyles();
//	//}
//}

//--------------------------------------------------------------
void SurfingLights::ChangedLights(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedLights " << name << " : " << e;

	if (0) {
	}

	// Resets
	else if (name == vPointReset.getName()) {
		doResetPoint();
	} else if (name == vSpotReset.getName()) {
		doResetSpot();
	} else if (name == vDirectionalReset.getName()) {
		doResetDirectional();
	}

	// Enablers
	else if (name == bPoint.getName()) {
		bFlagUpdateLights = true;
		refreshGui();
	} else if (name == bSpot.getName()) {
		bFlagUpdateLights = true;
		refreshGui();
	} else if (name == bDirectional.getName()) {
		bFlagUpdateLights = true;
		refreshGui();
	}

	//--

	// Point

	else if (name == pointLightAmbientColor.getName()) {
		if (pointLight.getAmbientColor() != pointLightAmbientColor.get())
			pointLight.setAmbientColor(pointLightAmbientColor.get());
	} else if (name == pointLightDiffuseColor.getName()) {
		if (pointLight.getDiffuseColor() != pointLightDiffuseColor.get())
			pointLight.setDiffuseColor(pointLightDiffuseColor.get());
	} else if (name == pointLightSpecularColor.getName()) {
		if (pointLight.getSpecularColor() != pointLightSpecularColor.get())
			pointLight.setSpecularColor(pointLightSpecularColor.get());
	}

	//--

	// Directional

	else if (name == directionalLightAmbientColor.getName()) {
		if (directionalLight.getAmbientColor() != directionalLightAmbientColor.get())
			directionalLight.setAmbientColor(directionalLightAmbientColor.get());
	} else if (name == directionalLightDiffuseColor.getName()) {
		if (directionalLight.getDiffuseColor() != directionalLightDiffuseColor.get())
			directionalLight.setDiffuseColor(directionalLightDiffuseColor.get());
	} else if (name == directionalLightSpecularColor.getName()) {
		if (directionalLight.getSpecularColor() != directionalLightSpecularColor.get())
			directionalLight.setSpecularColor(directionalLightSpecularColor.get());
	}

	//--

	// Spot

	else if (name == spotLightAmbientColor.getName()) {
		if (spotLight.getAmbientColor() != spotLightAmbientColor.get())
			spotLight.setAmbientColor(spotLightAmbientColor.get());
	} else if (name == spotLightDiffuseColor.getName()) {
		if (spotLight.getDiffuseColor() != spotLightDiffuseColor.get())
			spotLight.setDiffuseColor(spotLightDiffuseColor.get());
	} else if (name == spotLightSpecularColor.getName()) {
		if (spotLight.getSpecularColor() != spotLightSpecularColor.get())
			spotLight.setSpecularColor(spotLightSpecularColor.get());
	}

	//TODO fix
	else if (name == spotLightCutOff.getName()) {
		spotLight.setSpotlightCutOff(spotLightCutOff.get());
	} else if (name == spotLightConcentration.getName()) {
		spotLight.setSpotConcentration(spotLightConcentration.get());
	}

	//--

	else if (name == bSmoothLights.getName()) {
		ofSetSmoothLighting(bSmoothLights);
	}

	//--

	// Global colors

	else if (name == pointLightGlobalColor.getName()) {
		////fix crash
		//if (!bDoneStartup) return;

		if (pointLightAmbientColor.get() != pointLightGlobalColor.get())
			pointLightAmbientColor.set(pointLightGlobalColor.get());

		if (pointLightDiffuseColor.get() != pointLightGlobalColor.get())
			pointLightDiffuseColor.set(pointLightGlobalColor.get());

		if (pointLightSpecularColor.get() != pointLightGlobalColor.get())
			pointLightSpecularColor.set(pointLightGlobalColor.get());
	}

	else if (name == directionalLightGlobalColor.getName()) {
		////fix crash
		//if (!bDoneStartup) return;

		if (directionalLightAmbientColor.get() != directionalLightGlobalColor.get())
			directionalLightAmbientColor.set(directionalLightGlobalColor.get());

		if (directionalLightDiffuseColor.get() != directionalLightGlobalColor.get())
			directionalLightDiffuseColor.set(directionalLightGlobalColor.get());

		if (directionalLightSpecularColor.get() != directionalLightGlobalColor.get())
			directionalLightSpecularColor.set(directionalLightGlobalColor.get());
	}

	else if (name == spotLightGlobalColor.getName()) {
		////fix crash
		//if (!bDoneStartup) return;

		if (spotLightAmbientColor.get() != spotLightGlobalColor.get())
			spotLightAmbientColor.set(spotLightGlobalColor.get());

		if (spotLightDiffuseColor.get() != spotLightGlobalColor.get())
			spotLightDiffuseColor.set(spotLightGlobalColor.get());

		if (spotLightSpecularColor.get() != spotLightGlobalColor.get())
			spotLightSpecularColor.set(spotLightGlobalColor.get());
	}

	//--

	else if (name == vResetLights.getName()) {
		doResetLights();
	}
}

//--------------------------------------------------------------
void SurfingLights::ChangedShadow(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedShadow " << name << ": " << e;

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

/*
//--------------------------------------------------------------
void SurfingLights::ChangedBrights(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedBrights " << name << " : " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	if (0) {
	}

	else if (name == pointBright.getName()) {
		refreshBrightPoint();
	}

	else if (name == directionalBright.getName()) {
		refreshBrightDirect();
	}

	else if (name == spotBright.getName()) {
		refreshBrightSpot();
	}
}

//--------------------------------------------------------------
void SurfingLights::refreshBrights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshBrights()";

	refreshBrightPoint();
	refreshBrightDirect();
	refreshBrightSpot();
}

//--------------------------------------------------------------
void SurfingLights::refreshBrightPoint() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshBrightPoint()";

	auto ca = pointLightAmbientColor.get();
	auto cd = pointLightDiffuseColor.get();
	auto cs = pointLightSpecularColor.get();
	ca.setBrightness(pointBright);
	cd.setBrightness(pointBright);
	cs.setBrightness(pointBright);
	pointLightAmbientColor.set(ca);
	pointLightDiffuseColor.set(cd);
	pointLightSpecularColor.set(cs);
}

//--------------------------------------------------------------
void SurfingLights::refreshBrightDirect() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshBrightDirect()";

	auto ca = directionalLightAmbientColor.get();
	auto cd = directionalLightDiffuseColor.get();
	auto cs = directionalLightSpecularColor.get();
	ca.setBrightness(directionalBright);
	cd.setBrightness(directionalBright);
	cs.setBrightness(directionalBright);
	directionalLightAmbientColor.set(ca);
	directionalLightDiffuseColor.set(cd);
	directionalLightSpecularColor.set(cs);
}

//--------------------------------------------------------------
void SurfingLights::refreshBrightSpot() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshBrightSpot()";

	auto ca = spotLightAmbientColor.get();
	auto cd = spotLightDiffuseColor.get();
	auto cs = spotLightSpecularColor.get();
	ca.setBrightness(spotBright);
	cd.setBrightness(spotBright);
	cs.setBrightness(spotBright);
	spotLightAmbientColor.set(ca);
	spotLightDiffuseColor.set(cd);
	spotLightSpecularColor.set(cs);
}
*/

//--------------------------------------------------------------
void SurfingLights::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Save: " << pathSettings;

	ofxSurfing::saveSettings(parameters, pathSettings);
	ofxSurfing::saveSettings(shadowParams, pathSettingsShadows);
}

//--------------------------------------------------------------
void SurfingLights::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Load: " << pathSettings;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	ofxSurfing::loadSettings(parameters, pathSettings);
	ofxSurfing::loadSettings(shadowParams, pathSettingsShadows);

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif
}

//--------------------------------------------------------------
void SurfingLights::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:exit()";

// Not required to be called bc it's using the auto saver!
#if defined(SURFING__USE_AUTOSAVE_FORCE_ON_EXIT) || !defined(SURFING__USE_AUTOSAVE_SETTINGS_ENGINE)
	save();
	ofxSurfing::saveGroup(params_Lights, pathSettings);
#endif
}

//--------------------------------------------------------------
void SurfingLights::begin() {

	ofEnableLighting();

	//Bg_material.begin();
	//{
	//if (bPoint) pointLight.enable();
	//if (bSpot) spotLight.enable();
	//if (bDirectional) directionalLight.enable();
}

//--------------------------------------------------------------
void SurfingLights::end() {

	//if (!bPoint) pointLight.disable();
	//if (bSpot) spotLight.disable();
	//if (bDirectional) directionalLight.disable();
	//}
	//Bg_material.end();

	ofDisableLighting();

	//-

	//drawDebug();
}

//--------------------------------------------------------------
void SurfingLights::beginLights() {
	if (bPoint) pointLight.enable();
	if (bSpot) spotLight.enable();
	if (bDirectional) directionalLight.enable();
}

//--------------------------------------------------------------
void SurfingLights::endLights() {
	if (bPoint) pointLight.disable();
	if (bSpot) spotLight.disable();
	if (bDirectional) directionalLight.disable();
}

//--------------------------------------------------------------
void SurfingLights::doResetShadow() {
	bDrawShadow.set(true);
	shadowBias.set(0.1);
	shadowNormalBias.set(-4.f);
	//shadowStrength.set(0.6f);//TODO
	//shadowSize.set(glm::vec2(0.25f,0.25f));//TODO
	//bDebugShadow.set(false);
}