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
	ofRemoveListener(lightsParams.parameterChangedE(), this, &SurfingLights::ChangedLights);
	ofRemoveListener(brightsParams.parameterChangedE(), this, &SurfingLights::ChangedBrights);
	//ofRemoveListener(pointParams.parameterChangedE(), this, &SurfingLights::ChangedPoint);
	//ofRemoveListener(spotParams.parameterChangedE(), this, &SurfingLights::ChangedSpot);
	//ofRemoveListener(directionalParams.parameterChangedE(), this, &SurfingLights::ChangedDirectional);
	ofRemoveListener(shadowParams.parameterChangedE(), this, &SurfingLights::ChangedShadow);
}

//--

//--------------------------------------------------------------
void SurfingLights::setFunctionRenderScene(callback_t f) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setFunctionRenderScene()";

	if (f_RenderScene == nullptr) {
		ofLogError("ofxSurfingPBR") << "SurfingLights:setFunctionRenderScene(). Wrong / empty callback_t";
	}
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

	// 0 Point
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

	// 1 Directional
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

	// 2 Spot
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

	//TODO: add from OF example
	ofShadow::setAllShadowTypes(OF_SHADOW_TYPE_PCF_LOW);
}

//--------------------------------------------------------------
void SurfingLights::updateLights() {
	if (lights.size() == 0) {
		ofLogWarning("ofxSurfingPBR") << "SurfingLights:updateLights(). Skipped. Lights are not yet created!";
		return;
	}

	updateAnims();

	//--

	// Point
	{
		lights[0]->getShadow().setEnabled(bDrawShadow);

		if (bPoint) {
			if (!lights[0]->getIsEnabled()) lights[0]->enable();
		} else {
			if (lights[0]->getIsEnabled()) lights[0]->disable();
		}

		lights[0]->setAmbientColor(pointAmbientColor);
		lights[0]->setDiffuseColor(pointDiffuseColor);
		lights[0]->setSpecularColor(pointSpecularColor);

		if (!bAnimLights && !bAnimLightsMouse) {
			lights[0]->setPosition(pointPosition);
		}
	}

	//--

	// Directional
	{
		lights[1]->getShadow().setEnabled(bDrawShadow);

		if (bDirectional) {
			if (!lights[1]->getIsEnabled()) lights[1]->enable();
		} else {
			if (lights[1]->getIsEnabled()) lights[1]->disable();
		}

		lights[1]->setAmbientColor(directionalAmbientColor);
		lights[1]->setDiffuseColor(directionalDiffuseColor);
		lights[1]->setSpecularColor(directionalSpecularColor);

		if (!bAnimLights && !bAnimLightsMouse) {
			lights[1]->setPosition(directionalPosition);

			glm::vec3 rad = glm::radians(directionalOrientation.get());
			glm::quat q = glm::quat(rad);
			lights[1]->setOrientation(q);
		}
	}

	//--

	// Spot
	{
		lights[2]->getShadow().setEnabled(bDrawShadow);

		if (bSpot) {
			if (!lights[2]->getIsEnabled()) lights[2]->enable();
		} else {
			if (lights[2]->getIsEnabled()) lights[2]->disable();
		}

		lights[2]->setAmbientColor(spotAmbientColor);
		lights[2]->setDiffuseColor(spotDiffuseColor);
		lights[2]->setSpecularColor(spotSpecularColor);

		if (!bAnimLights && !bAnimLightsMouse) {
			lights[2]->setPosition(spotPosition);

			glm::vec3 rad = glm::radians(spotOrientation.get());
			glm::quat q = glm::quat(rad);
			lights[2]->setOrientation(q);
		}
	}

	//--

	ofShadow::setAllShadowBias(shadowBias.get());
	ofShadow::setAllShadowNormalBias(shadowNormalBias.get());
}

//--------------------------------------------------------------
void SurfingLights::drawLights() {
	if (f_RenderScene == nullptr) return;

	//ofPushStyle();
	//ofSetColor(ofFloatColor::white, 1.f);

	for (int i = 0; i < lights.size(); i++) {
		auto & light = lights[i];

		if (light->shouldRenderShadowDepthPass()) {

			int numShadowPasses = light->getNumShadowDepthPasses();

			for (int j = 0; j < numShadowPasses; j++) {
				light->beginShadowDepthPass(j);
				{
					f_RenderScene();
				}
				light->endShadowDepthPass(j);
			}
		}
	}

	//ofPopStyle();
}

//--------------------------------------------------------------
void SurfingLights::drawDebugLights() {
	if (!bGui) return;
	if (!bDebug) return;

	//--

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

		if (bDebugLights) {
			ofSetColor(light->getAmbientColor());
			if (light->getType() == OF_LIGHT_POINT) {
				ofDrawSphere(light->getPosition(), 10); //workaround to remove axis
			}

			else {
				light->draw();
			}
		}

		if (bDebugShadow) {
			if (light->getShadow().getIsEnabled()) {
				light->getShadow().drawFrustum();
			}
		}
	}

	ofPopStyle();
}

//--

//--------------------------------------------------------------
void SurfingLights::setGuiPosition(glm::vec2 pos) {
	gui.setPosition(pos.x, pos.y);
}

//--------------------------------------------------------------
void SurfingLights::drawGui() {
	if (bGui) gui.draw();

	if (bGui_Shadows) {
		glm::vec2 p;
		if (bGui) {
			p = gui.getShape().getTopRight();
			p += glm::vec2 { (float)SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0.f };
		} else {
			p = gui.getPosition();
		}
		guiShadows.setPosition(p.x, p.y);
		guiShadows.draw();
	}
}

//--------------------------------------------------------------
void SurfingLights::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshGui()";

	//--

	// Point

	gui.getGroup(lightsParams.getName())
		.getGroup(params_Extra.getName())
		.minimize();

	gui.getGroup(lightsParams.getName())
		.getGroup(pointParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(pointParams.getName())
		.getGroup(pointPosition.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(pointParams.getName())
		.getGroup(pointColorsParams.getName())
		.minimize();
	auto & gp = gui.getGroup(lightsParams.getName())
					.getGroup(pointParams.getName());
	if (bPoint)
		gp.maximize();
	else
		gp.minimize();

	//--

	// Spot

	gui.getGroup(lightsParams.getName())
		.getGroup(spotParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(spotParams.getName())
		.getGroup(spotPosition.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(spotParams.getName())
		.getGroup(spotOrientation.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(spotParams.getName())
		.getGroup(spotColorsParams.getName())
		.minimize();
	auto & gs = gui.getGroup(lightsParams.getName())
					.getGroup(spotParams.getName());
	if (bSpot)
		gs.maximize();
	else
		gs.minimize();

	//--

	// Directional

	gui.getGroup(lightsParams.getName())
		.getGroup(directionalParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(directionalParams.getName())
		.getGroup(directionalPosition.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(directionalParams.getName())
		.getGroup(directionalOrientation.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(directionalParams.getName())
		.getGroup(directionalColorsParams.getName())
		.minimize();
	auto & gd = gui.getGroup(lightsParams.getName())
					.getGroup(directionalParams.getName());
	if (bDirectional)
		gd.maximize();
	else
		gd.minimize();

	//--
}

//--------------------------------------------------------------
ofRectangle SurfingLights::getGuiShape() const {
	ofRectangle bb { 0, 0, 0, 0 };
	if (bGui && bGui_Shadows) {
		ofRectangle r1 = gui.getShape();
		ofRectangle r2 = guiShadows.getShape();
		bb = r1.getUnion(r2);
	} else if (bGui && !bGui_Shadows) {
		bb = gui.getShape();
	} else if (!bGui && bGui_Shadows) {
		bb = guiShadows.getShape();
	}
	return bb;
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

	setupLights();

	setupParameters();

	setupGui();

	bDoneSetup = true;
}

//--------------------------------------------------------------
void SurfingLights::setupParameters() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupParameters()";

	float sz = 1.f * SURFING__SCENE_SIZE_UNIT;

	bGui.set("UI LIGHTS", false);

	bAnimLights.set("Anim Lights", false);
	bAnimLightsMouse.set("Mouse Lights", false);

	bDebug.set("Debug", true);
	bDebugShadow.set("Debug Shadow", true);
	bDebugLights.set("Debug Lights", true);
	bRefreshGui.set("Refresh Gui", false);

	//--

	vResetLights.set("Reset Lights");

	//--

	// Groups

	parameters.setName("PBR_LIGHTS");
	lightsParams.setName("Lights");
	params_Enablers.setName("Enable");
	params_Extra.setName("Extra");

	pointParams.setName("Light Point");
	spotParams.setName("Light Spot");
	directionalParams.setName("Light Direct");

	pointColorsParams.setName("p Colors");
	directionalColorsParams.setName("d Colors");
	spotColorsParams.setName("s Colors");

	brightsParams.setName("Power");
	globalColorsParams.setName("Global Colors");

	//--

	// Point

	bPoint.set("Point", false);
	vPointReset.set("Reset Point");
	pointPosition.set("p Position", glm::vec3(), glm::vec3(-sz), glm::vec3(sz));

	pointAmbientColor.set("p Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointDiffuseColor.set("p Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointSpecularColor.set("p Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	pointGlobalColor.set("p Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	pointGlobalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	//--

	// Directional

	bDirectional.set("Direct", false);
	vDirectionalReset.set("Reset Direct");
	directionalPosition.set("d Position", glm::vec3(0), glm::vec3(-sz), glm::vec3(sz));
	directionalOrientation.set("d Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));

	directionalAmbientColor.set("d Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalDiffuseColor.set("d Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalSpecularColor.set("d Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	directionalGlobalColor.set("d Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	directionalGlobalColor.set("d Global",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	//--

	// Spot

	bSpot.set("Spot", false);
	vSpotReset.set("Reset Spot");
	spotPosition.set("s Position", glm::vec3(), glm::vec3(-sz), glm::vec3(sz));

	spotAmbientColor.set("s Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotDiffuseColor.set("s Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotSpecularColor.set("s Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	spotGlobalColor.set("s Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	spotGlobalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	spotOrientation.set("s Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));

	//TODO
	spotCutOff.set("s CutOff", 1, 0, 90);
	spotConcentration.set("s Concentration", 1, 0, 128);

	//--

	pointGlobalColor.set(ofFloatColor(1));
	directionalGlobalColor.set(ofFloatColor(1));
	spotGlobalColor.set(ofFloatColor(1));

	//--

	bSmoothLights.set("Smooth Lights", false); // Default Low poly

	// Bright
	spotBright.set("Bright Spot", 0.1, 0, 1);
	pointBright.set("Bright Point", 0.1, 0, 1);
	directionalBright.set("Bright Dir", 0.1, 0, 1);

	//--

	// Lights

	// Point
	pointParams.add(pointBright);
	pointParams.add(pointGlobalColor);
	pointColorsParams.add(pointAmbientColor);
	pointColorsParams.add(pointDiffuseColor);
	pointColorsParams.add(pointSpecularColor);
	pointParams.add(pointColorsParams);
	pointParams.add(pointPosition);
	pointParams.add(vPointReset);

	// Directional
	directionalParams.add(directionalBright);
	directionalParams.add(directionalGlobalColor);
	directionalColorsParams.add(directionalAmbientColor);
	directionalColorsParams.add(directionalDiffuseColor);
	directionalColorsParams.add(directionalSpecularColor);
	directionalParams.add(directionalColorsParams);
	directionalParams.add(directionalPosition);
	directionalParams.add(directionalOrientation);
	directionalParams.add(vDirectionalReset);

	// Spot
	spotParams.add(spotBright);
	spotParams.add(spotGlobalColor);
	spotColorsParams.add(spotAmbientColor);
	spotColorsParams.add(spotDiffuseColor);
	spotColorsParams.add(spotSpecularColor);
	spotParams.add(spotColorsParams);
	spotParams.add(spotPosition);
	spotParams.add(spotOrientation);
	spotParams.add(spotCutOff);
	spotParams.add(spotConcentration);
	spotParams.add(vSpotReset);

	// Enablers
	params_Enablers.add(bPoint);
	params_Enablers.add(bDirectional);
	params_Enablers.add(bSpot);
	lightsParams.add(params_Enablers);

	// Bright
	brightsParams.add(pointBright);
	brightsParams.add(directionalBright);
	brightsParams.add(spotBright);
	lightsParams.add(brightsParams);

	// Global colors
	globalColorsParams.add(pointGlobalColor);
	globalColorsParams.add(directionalGlobalColor);
	globalColorsParams.add(spotGlobalColor);
	lightsParams.add(globalColorsParams);

	lightsParams.add(pointParams);
	lightsParams.add(directionalParams);
	lightsParams.add(spotParams);

	// Extra
	params_Extra.add(bSmoothLights);
	params_TestAnims.setName("TesAnims");
	params_TestAnims.add(bAnimLights);
	params_TestAnims.add(bAnimLightsMouse);
	params_Extra.add(params_TestAnims);
	params_Extra.add(bDebugLights);
	params_Extra.add(bDebugShadow);
	lightsParams.add(params_Extra);

	lightsParams.add(bDebug);
	lightsParams.add(bRefreshGui);
	lightsParams.add(vResetLights);

	/// Exclude
	globalColorsParams.setSerializable(false);
	pointGlobalColor.setSerializable(false);
	spotGlobalColor.setSerializable(false);
	directionalGlobalColor.setSerializable(false);

	//----

	// Shadows

	bGui_Shadows.set("UI SHADOWS", false);
	bDrawShadow.set("Draw Shadow", true);
	shadowBias.set("Bias", 0.07, 0.0, 1.0);
	shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0);
	bDebugShadow.set("Debug Shadow", true);
	vResetShadow.set("Reset Shadow");

	shadowParams.setName("PBR_SHADOWS");
	shadowParams.add(bDrawShadow);
	shadowParams.add(shadowBias);
	shadowParams.add(shadowNormalBias);

	//TODO
	//shadowParams.add(shadowStrength.set("Strength", 0.6f, 0.f, 1.f));
	//shadowParams.add(shadowSize.set("Shadow Size", glm::vec2(0.25f, 0.25f), glm::vec2(0, 0), glm::vec2(1.f, 1.f)));

	shadowParams.add(vResetShadow);
	shadowParams.add(bDebugShadow);

	//-

	parameters.add(bGui_Shadows);
	parameters.add(lightsParams);

	//--

	// Callbacks

	ofAddListener(lightsParams.parameterChangedE(), this, &SurfingLights::ChangedLights);
	ofAddListener(brightsParams.parameterChangedE(), this, &SurfingLights::ChangedBrights);
	//ofAddListener(pointParams.parameterChangedE(), this, &SurfingLights::ChangedPoint);
	//ofAddListener(spotParams.parameterChangedE(), this, &SurfingLights::ChangedSpot);
	//ofAddListener(directionalParams.parameterChangedE(), this, &SurfingLights::ChangedDirectional);
	ofAddListener(shadowParams.parameterChangedE(), this, &SurfingLights::ChangedShadow);

	//--

	startup();
}

//--------------------------------------------------------------
void SurfingLights::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:startup()";

	doResetLights();

	//--

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	callback_t f = std::bind(&SurfingLights::save, this);
	autoSaver.setFunctionSaver(f);
#endif

	//--

	//TODO

#if 0
	// Settings
	bool bNotFound = true;
	//bNotFound |= !ofxSurfing::loadGroup(lightsParams, pathSettings);
	//bNotFound |= !ofxSurfing::loadGroup(paramsScene, pathSettings_Bg);
#endif

	//load();
	bFlagLoad = true;

#if 0
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
		bDebugShadow = true;
	}
#endif

	bDoneStartup = true;
}

//--------------------------------------------------------------
void SurfingLights::update(ofEventArgs & args) {
	update();
}

//// Will be called on the first update frame.
///	Use this pattern carefully, as could be a bad practice in some scenarios!
//--------------------------------------------------------------
void SurfingLights::startupDelayed() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:startupDelayed() Start";

	//load();
	bFlagLoad = true;

	bDoneStartupDelayed = true;
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:startupDelayed() Done! at frame number: " << ofGetFrameNum();
}

//--------------------------------------------------------------
void SurfingLights::update() { // App flow controls
	{
		// On the 1st frame
		int f = (int)(ofGetFrameNum());
		if (f >= 0) {
			if (!bAppRunning) {
				bAppRunning = true;
				ofLogNotice("ofxSurfingPBR") << "SurfingLights:Starting app at frame number: " << ofGetFrameNum();
			}
		}

		// After the 1st frame
		if (f > 0 && bAppRunning && !bDoneStartupDelayed) {
			//TODO fix settings loader
			startupDelayed();
		}
	}

	//----

	if (bFlagRefreshGui) {
		bFlagRefreshGui = false;

		if (bRefreshGui) refreshGui();
	}

	//--

	if (bFlagDoRefreshBrights) {
		bFlagDoRefreshBrights = false;

		doRefreshBrights();
	}

	else if (bFlagDoRefreshBrightPoint) {
		bFlagDoRefreshBrightPoint = false;

		doRefreshBrightPoint();
	}

	else if (bFlagDoRefreshBrightDirect) {
		bFlagDoRefreshBrightDirect = false;

		doRefreshBrightDirect();
	}

	else if (bFlagDoRefreshBrightSpot) {
		bFlagDoRefreshBrightSpot = false;

		doRefreshBrightSpot();
	}

	//--

	else if (bFlagDoResetLights) {
		bFlagDoResetLights = false;

		doResetLights();
	}

	else if (bFlagDoResetPoint) {
		bFlagDoResetPoint = false;

		doResetPoint();
	}

	else if (bFlagDoResetSpot) {
		bFlagDoResetSpot = false;

		doResetSpot();
	}

	else if (bFlagDoResetDirectional) {
		bFlagDoResetDirectional = false;

		doResetDirectional();
	}

	//--

	else if (bFlagDoResetShadow) {
		bFlagDoResetShadow = false;

		doResetShadow();
	}

	//--

	// workaround to fix startup loading
	// bc flag pattern used for callbacks
	else if (bFlagLoad) {
		bFlagLoad = false;

		load();
	}
}

//--------------------------------------------------------------
void SurfingLights::restoreAnims() {
	if (lights.size() == 0) return;

	lights[0]->setPosition(pointPosition);

	{
		lights[1]->setPosition(directionalPosition);
		glm::vec3 rad = glm::radians(directionalOrientation.get());
		glm::quat q = glm::quat(rad);
		lights[1]->setOrientation(q);
	}

	{
		lights[2]->setPosition(spotPosition);
		glm::vec3 rad = glm::radians(spotOrientation.get());
		glm::quat q = glm::quat(rad);
		lights[2]->setOrientation(q);
	}
}

//--------------------------------------------------------------
void SurfingLights::updateAnims() {

	// Anim lights by time

	if (bAnimLights) {

		const float r = SURFING__SCENE_SIZE_UNIT * 0.25f;

		auto t = ofGetElapsedTimef() * 0.7;

		// Point
		lights[0]->setPosition(
			pointPosition.get().x,
			cos(t) * r + pointPosition.get().y - r,
			sin(t) * r * 2 + pointPosition.get().z);

		// Directional
		lights[1]->setPosition(
			directionalPosition.get().x,
			sin(t) * r + directionalPosition.get().y - r,
			cos(t) * r * 2 + directionalPosition.get().z);

		// Spot
		lights[2]->setPosition(
			spotPosition.get().x,
			sin(t) * r + spotPosition.get().y - r,
			-cos(t) * r * 6 + spotPosition.get().z);
	}

	//--

	// Move lights by mouse

	if (bAnimLightsMouse) {
		mouseX = ofGetMouseX();
		mouseY = ofGetMouseY();

		float r = 1.5f * SURFING__SCENE_SIZE_UNIT;
		float x = ofMap(mouseX, 0, ofGetWidth(), -r, r, true);
		float y = ofMap(mouseY, 0, ofGetHeight(), r, -r, true);
		float oy = 200;

		// Point
		lights[0]->setPosition(x, y + oy, pointPosition.get().z);
		lights[0]->setOrientation(glm::vec3(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));

		// Directional
		lights[1]->setPosition(x, y, directionalPosition.get().z);
		lights[1]->setOrientation(glm::vec3(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));

		// Spot
		lights[2]->setPosition(x, y - oy, spotPosition.get().z);
		lights[2]->setOrientation(glm::vec3(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));
	}
}

//--

//--------------------------------------------------------------
void SurfingLights::doResetLights(bool bColorsToo) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetLights()";

	if (bColorsToo) {
		if (!bPoint) bPoint = true;
		if (bSpot) bSpot = false;
		if (bDirectional) bDirectional = false;
	}

	doResetPoint();
	doResetDirectional();
	doResetSpot();

	doRefreshBrights();
}

// Point
//--------------------------------------------------------------
void SurfingLights::doResetPoint(bool bColorsToo) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetPoint()";

	//bPoint = false;

	// touch colors too
	if (bColorsToo) {

		pointGlobalColor.set(ofFloatColor(1));

		//pointGlobalColor.setWithoutEventNotifications(ofFloatColor(1));
		//pointAmbientColor.set(ofFloatColor(1));
		//pointDiffuseColor.set(ofFloatColor(1));
		//pointSpecularColor.set(ofFloatColor(1));
	}

	pointPosition.set(glm::vec3(
		SURFING__SCENE_SIZE_UNIT * 0.2f,
		SURFING__SCENE_SIZE_UNIT,
		SURFING__SCENE_SIZE_UNIT * 0.1f));

	pointBright = SURFING__PBR__HELPER_GLOBAL_BRIGHT_RESET;
}

// Directional
//--------------------------------------------------------------
void SurfingLights::doResetDirectional(bool bColorsToo) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetDirectional()";

	//bDirectional = false;

	// touch colors too
	if (bColorsToo) {

		directionalGlobalColor.set(ofFloatColor(1));

		//directionalGlobalColor.setWithoutEventNotifications(ofFloatColor(1));
		//directionalAmbientColor.set(ofFloatColor(1));
		//directionalDiffuseColor.set(ofFloatColor(1));
		//directionalSpecularColor.set(ofFloatColor(1));
	}

	directionalPosition.set(glm::vec3(0,
		SURFING__SCENE_SIZE_UNIT * 1.f,
		SURFING__SCENE_SIZE_UNIT * 0.5f));

	directionalOrientation.set(glm::vec3(-60, 0, 0));

	directionalBright = SURFING__PBR__HELPER_GLOBAL_BRIGHT_RESET;
}

// Spot
//--------------------------------------------------------------
void SurfingLights::doResetSpot(bool bColorsToo) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetSpot()";

	//bSpot = false;

	// touch colors too
	if (bColorsToo) {
		spotGlobalColor.set(ofFloatColor(1));

		//spotGlobalColor.setWithoutEventNotifications(ofFloatColor(1));
		//spotAmbientColor.set(ofFloatColor(1));
		//spotDiffuseColor.set(ofFloatColor(1));
		//spotSpecularColor.set(ofFloatColor(1));
	}

	spotPosition.set(glm::vec3(
		-SURFING__SCENE_SIZE_UNIT * 0.2f,
		SURFING__SCENE_SIZE_UNIT,
		SURFING__SCENE_SIZE_UNIT * 0.3f));

	spotOrientation.set(glm::vec3(-60, 0, 0));

	//TODO
	spotCutOff.set(90);
	spotConcentration.set(0);

	spotBright = SURFING__PBR__HELPER_GLOBAL_BRIGHT_RESET;
}

//--

////--------------------------------------------------------------
//void SurfingLights::ChangedPoint(ofAbstractParameter & e) {
//	string name = e.getName();
//
//	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedPoint " << name << " : " << e;
//
//#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
//	if (e.isSerializable()) {
//		autoSaver.saveSoon();
//	}
//#endif
//}
//
////--------------------------------------------------------------
//void SurfingLights::ChangedSpot(ofAbstractParameter & e) {
//	string name = e.getName();
//
//	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedSpot " << name << " : " << e;
//
//#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
//	if (e.isSerializable()) {
//		autoSaver.saveSoon();
//	}
//#endif
//}
//
////--------------------------------------------------------------
//void SurfingLights::ChangedDirectional(ofAbstractParameter & e) {
//	string name = e.getName();
//
//	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedDirectional " << name << " : " << e;
//
//#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
//	if (e.isSerializable()) {
//		autoSaver.saveSoon();
//	}
//#endif
//}

//--------------------------------------------------------------
void SurfingLights::ChangedLights(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedLights " << name << " : " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	// Resets

	if (name == vPointReset.getName()) {
		bFlagDoResetPoint = true;
	}

	else if (name == vSpotReset.getName()) {
		bFlagDoResetSpot = true;
	}

	else if (name == vDirectionalReset.getName()) {
		bFlagDoResetDirectional = true;

	}

	else if (name == vResetLights.getName()) {
		bFlagDoResetLights = true;
	}

	//--

	// Enablers

	else if (name == bPoint.getName()) {
		bFlagRefreshGui = true;
	}

	else if (name == bSpot.getName()) {
		bFlagRefreshGui = true;
	}

	else if (name == bDirectional.getName()) {
		bFlagRefreshGui = true;
	}

	//--

	else if (name == spotCutOff.getName()) {
		if (lights.size() > 3)
			lights[2]->setSpotlightCutOff(spotCutOff.get());
	}

	else if (name == spotConcentration.getName()) {
		if (lights.size() > 3)
			lights[2]->setSpotConcentration(spotConcentration.get());
	}

	//--

	else if (name == bAnimLights.getName()) {
		if (bAnimLights && bAnimLightsMouse) bAnimLightsMouse.setWithoutEventNotifications(false);
		if (bAnimLights) {
			restoreAnims();
		}
	}

	else if (name == bAnimLightsMouse.getName()) {
		if (bAnimLightsMouse && bAnimLights) bAnimLights.setWithoutEventNotifications(false);
		if (bAnimLightsMouse) {
			restoreAnims();
		}
	}

	//--

	else if (name == bSmoothLights.getName()) {
		ofSetSmoothLighting(bSmoothLights);
	}

	//--

	// Global colors

	else if (name == pointGlobalColor.getName()) {

		if (pointAmbientColor.get() != pointGlobalColor.get())
			pointAmbientColor.set(pointGlobalColor.get());

		if (pointDiffuseColor.get() != pointGlobalColor.get())
			pointDiffuseColor.set(pointGlobalColor.get());

		if (pointSpecularColor.get() != pointGlobalColor.get())
			pointSpecularColor.set(pointGlobalColor.get());
	}

	else if (name == directionalGlobalColor.getName()) {

		if (directionalAmbientColor.get() != directionalGlobalColor.get())
			directionalAmbientColor.set(directionalGlobalColor.get());

		if (directionalDiffuseColor.get() != directionalGlobalColor.get())
			directionalDiffuseColor.set(directionalGlobalColor.get());

		if (directionalSpecularColor.get() != directionalGlobalColor.get())
			directionalSpecularColor.set(directionalGlobalColor.get());
	}

	else if (name == spotGlobalColor.getName()) {

		if (spotAmbientColor.get() != spotGlobalColor.get())
			spotAmbientColor.set(spotGlobalColor.get());

		if (spotDiffuseColor.get() != spotGlobalColor.get())
			spotDiffuseColor.set(spotGlobalColor.get());

		if (spotSpecularColor.get() != spotGlobalColor.get())
			spotSpecularColor.set(spotGlobalColor.get());
	}
}

//--------------------------------------------------------------
void SurfingLights::ChangedShadow(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedShadow " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	if (name == vResetShadow.getName()) {
		bFlagDoResetShadow = true;
	}

	//workflow
	else if (name == bDebugShadow.getName()) {
		if (bDebugShadow) {
			if (!bDebug) bDebug = true;
		}
	}
}

//--

//--------------------------------------------------------------
void SurfingLights::ChangedBrights(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedBrights " << name << " : " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	if (name == pointBright.getName()) {
		bFlagDoRefreshBrightPoint = true;
	}

	else if (name == directionalBright.getName()) {
		bFlagDoRefreshBrightDirect = true;
	}

	else if (name == spotBright.getName()) {
		bFlagDoRefreshBrightSpot = true;
	}
}

//--

//--------------------------------------------------------------
void SurfingLights::doRefreshBrights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshBrights()";

	doRefreshBrightPoint();
	doRefreshBrightDirect();
	doRefreshBrightSpot();
}

//--------------------------------------------------------------
void SurfingLights::doRefreshBrightPoint() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshBrightPoint()";

	auto ca = pointAmbientColor.get();
	auto cd = pointDiffuseColor.get();
	auto cs = pointSpecularColor.get();

	float brg = ofClamp(pointBright,
		SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MIN, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	ca.setBrightness(brg);
	cd.setBrightness(brg);
	cs.setBrightness(brg);

	pointAmbientColor.set(ca);
	pointDiffuseColor.set(cd);
	pointSpecularColor.set(cs);
}

//--------------------------------------------------------------
void SurfingLights::doRefreshBrightDirect() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshBrightDirect()";

	auto ca = directionalAmbientColor.get();
	auto cd = directionalDiffuseColor.get();
	auto cs = directionalSpecularColor.get();

	float brg = ofClamp(directionalBright,
		SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MIN, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	ca.setBrightness(brg);
	cd.setBrightness(brg);
	cs.setBrightness(brg);

	directionalAmbientColor.set(ca);
	directionalDiffuseColor.set(cd);
	directionalSpecularColor.set(cs);
}

//--------------------------------------------------------------
void SurfingLights::doRefreshBrightSpot() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshBrightSpot()";

	auto ca = spotAmbientColor.get();
	auto cd = spotDiffuseColor.get();
	auto cs = spotSpecularColor.get();

	float brg = ofClamp(spotBright,
		SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MIN, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	ca.setBrightness(brg);
	cd.setBrightness(brg);
	cs.setBrightness(brg);

	spotAmbientColor.set(ca);
	spotDiffuseColor.set(cd);
	spotSpecularColor.set(cs);
}

//--

//--------------------------------------------------------------
void SurfingLights::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Save -> " << pathSettings << " , " << pathSettingsShadows;

	ofxSurfing::saveSettings(shadowParams, pathSettingsShadows);
	ofxSurfing::saveSettings(lightsParams, pathSettings);
}

//--------------------------------------------------------------
void SurfingLights::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Load -> " << pathSettings << " , " << pathSettingsShadows;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	ofxSurfing::loadSettings(shadowParams, pathSettingsShadows);
	ofxSurfing::loadSettings(lightsParams, pathSettings);

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
#endif
}

//--------------------------------------------------------------
void SurfingLights::begin() {
	ofEnableLighting();
}

//--------------------------------------------------------------
void SurfingLights::end() {
	ofDisableLighting();
}

//--------------------------------------------------------------
void SurfingLights::doResetShadow() {

	//bDebugShadow.set(false);
	bDrawShadow.set(true);

	shadowBias.set(0.1);
	shadowNormalBias.set(-4.f);

	//shadowStrength.set(0.6f);//TODO
	//shadowSize.set(glm::vec2(0.25f,0.25f));//TODO
}