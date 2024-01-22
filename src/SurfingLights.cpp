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
	ofRemoveListener(powersParams.parameterChangedE(), this, &SurfingLights::ChangedPowers);
	ofRemoveListener(shadowParams.parameterChangedE(), this, &SurfingLights::ChangedShadow);

#ifdef SURFING__PBR__USE_AUTO_CALL_EXIT_ON_DESTRUCTOR_IF_REQUIRED
	if (!bDoneExit) {
		ofLogWarning("ofxSurfingPBR") << "SurfingLights:~SurfingLights() Force calling exit() bc has not been called until now!";

		exit();
	}
#endif
}

//--

//--------------------------------------------------------------
void SurfingLights::setFunctionRenderScene(callback_t f) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setFunctionRenderScene()";

	if (f_RenderScene == nullptr) {
		ofLogError("ofxSurfingPBR") << "SurfingLights:setFunctionRenderScene(). Wrong callback_t nullptr";
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

		//light->getShadow().setStrength(SURFING__PBR__SHADOW_DEFAULT_STRENGTH);

		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	// 1 Directional
	{
		auto light = make_shared<ofLight>();
		light->enable();

		light->setDirectional();

		//light->getShadow().setNearClip(SURFING__PBR__LIGHTS_NEAR);
		light->getShadow().setFarClip(SURFING__PBR__LIGHTS_FAR);

		//light->getShadow().setStrength(SURFING__PBR__SHADOW_DEFAULT_STRENGTH);

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

		//light->getShadow().setStrength(SURFING__PBR__SHADOW_DEFAULT_STRENGTH);

		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	// 3 Area
	{
		auto light = make_shared<ofLight>();
		light->enable();

		light->setAreaLight(areaSize.get().x, areaSize.get().y);

		light->getShadow().setNearClip(SURFING__PBR__LIGHTS_NEAR);
		light->getShadow().setFarClip(SURFING__PBR__LIGHTS_FAR);

		//light->getShadow().setStrength(SURFING__PBR__SHADOW_DEFAULT_STRENGTH);

		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	//--

	// Shadows

	ofShadow::enableAllShadows();

	//TODO: add from OF example
	//ofShadow::setAllShadowTypes(OF_SHADOW_TYPE_PCF_LOW);
}

//--------------------------------------------------------------
void SurfingLights::updateLights() {

	/*
		Usually to be called with ofEnableDepthTest() and before camera.begin() 
		void ofApp::draw(){
			ofEnableDepthTest();
			//computeLights();
			camera.begin();
	*/

	//--

	computeLights();
	computeShadows();
}

//--------------------------------------------------------------
void SurfingLights::computeLights() {
	if (lights.size() == 0) {
		ofLogWarning("ofxSurfingPBR") << "SurfingLights:computeLights(). Skipped. Lights are not yet created!";
		return;
	}

	updateAnims();

	//--

	// Point
	{
		lights[0]->getShadow().setEnabled(bDrawShadow && bPointShadow);

		//TODO: ?
		//if( !ofIsGLProgrammableRenderer() ) {
		//light->enable();
		//}

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

		float v = ofMap(pointSizeFar, 0, 1,
			SURFING__PBR__LIGHTS_FAR, SURFING__PBR__LIGHTS_FAR * SURFING__PBR__LIGHTS_FAR_RATIO_MAX, true);
		lights[0]->getShadow().setFarClip(v);
	}

	//--

	// Directional
	{
		lights[1]->getShadow().setEnabled(bDrawShadow && bDirectionalShadow);

		if (bDirectional) {
			if (!lights[1]->getIsEnabled()) lights[1]->enable();
		} else {
			if (lights[1]->getIsEnabled()) lights[1]->disable();
		}

		lights[1]->setAmbientColor(directionalAmbientColor);
		lights[1]->setDiffuseColor(directionalDiffuseColor);
		lights[1]->setSpecularColor(directionalSpecularColor);

		////TODO: make the zone bigger?
		//lights[1]->set();

		if (!bAnimLights && !bAnimLightsMouse) {
			lights[1]->setPosition(directionalPosition);

			glm::vec3 rad = glm::radians(directionalOrientation.get());
			glm::quat q = glm::quat(rad);
			lights[1]->setOrientation(q);
		}

		float vf = ofMap(directionalSizeFar, 0, 1,
			SURFING__PBR__LIGHTS_FAR, SURFING__PBR__LIGHTS_FAR * SURFING__PBR__LIGHTS_FAR_RATIO_MAX, true);
		lights[1]->getShadow().setFarClip(vf);

		float vn = ofMap(directionalSizeNear, 0, 1,
			SURFING__PBR__LIGHTS_NEAR, SURFING__PBR__LIGHTS_NEAR * SURFING__PBR__LIGHTS_FAR_RATIO_MAX, true);
		lights[1]->getShadow().setNearClip(vn);
	}

	//--

	// Spot
	{
		lights[2]->getShadow().setEnabled(bDrawShadow && bSpotShadow);

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

		float v = ofMap(spotSizeFar, 0, 1,
			SURFING__PBR__LIGHTS_FAR, SURFING__PBR__LIGHTS_FAR * SURFING__PBR__LIGHTS_FAR_RATIO_MAX, true);
		lights[2]->getShadow().setFarClip(v);
	}

	//--

	// Area
	{
		lights[3]->getShadow().setEnabled(bDrawShadow && bAreaShadow);

		if (bArea) {
			if (!lights[3]->getIsEnabled()) lights[3]->enable();
		} else {
			if (lights[3]->getIsEnabled()) lights[3]->disable();
		}

		lights[3]->setAmbientColor(areaAmbientColor);
		lights[3]->setDiffuseColor(areaDiffuseColor);
		lights[3]->setSpecularColor(areaSpecularColor);

		if (!bAnimLights && !bAnimLightsMouse) {
			lights[3]->setPosition(areaPosition);

			glm::vec3 rad = glm::radians(areaOrientation.get());
			glm::quat q = glm::quat(rad);
			lights[3]->setOrientation(q);
		}

		float v = ofMap(areaSizeFar, 0, 1,
			SURFING__PBR__LIGHTS_FAR, SURFING__PBR__LIGHTS_FAR * SURFING__PBR__LIGHTS_FAR_RATIO_MAX, true);
		lights[3]->getShadow().setFarClip(v);
	}
}

//--------------------------------------------------------------
void SurfingLights::computeShadows() {

	if (f_RenderScene == nullptr) return;

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
}

//--------------------------------------------------------------
void SurfingLights::drawDebugLights() {
	if (!bGui) return;
	if (!bDebug) return;

	//--

	ofPushStyle();

	for (size_t i = 0; i < lights.size(); i++) {

		// skip if not enabled
		if (i == 0 && !bPoint)
			continue;
		else if (i == 1 && !bDirectional)
			continue;
		else if (i == 2 && !bSpot)
			continue;
		else if (i == 3 && !bArea)
			continue;

		//--

		auto & light = lights[i];

		if (bDebugLights && light->getIsEnabled()) {
			ofSetColor(light->getAmbientColor());

			if (light->getType() == OF_LIGHT_POINT) {
				ofDrawSphere(light->getPosition(), 10); //workaround to remove axis
			}

			else {
				light->draw();
			}
		}

		if (bDebugShadow && light->getShadow().getIsEnabled()) {
			light->getShadow().drawFrustum();
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
	if (!bGui) return;
	gui.draw();

	if (bGui_Shadows) {
		glm::vec2 p;
		if (bGui) {
			p = gui.getShape().getTopRight();
			p += glm::vec2 { (float)SURFING__OFXGUI__PAD_BETWEEN_PANELS, 0.f };
		} else {
			p = gui.getPosition();
		}

		guiShadows.setPosition(p.x, p.y);
		guiShadows.draw();
	}
}

//--------------------------------------------------------------
void SurfingLights::refreshGui(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshGui()";

	gui.getGroup(lightsParams.getName())
		.getGroup(globalColorsParams.getName())
		.minimize();

	if (bHard) {
		gui.getGroup(lightsParams.getName())
			.getGroup(lightsSettingsParams.getName())
			.minimize();

		//gui.getGroup(lightsParams.getName())
		//	.getGroup(params_Extra.getName())
		//	.minimize();
	}

	//gui.getGroup(lightsParams.getName())
	//	.getGroup(lightsSettingsParams.getName())
	//	.minimize();

	gui.getGroup(lightsParams.getName())
		.getGroup(params_Extra.getName())
		.minimize();

	//--

	// Point

	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(pointParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(pointParams.getName())
		.getGroup(pointColorsParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(pointParams.getName())
		.getGroup(pointPosition.getName())
		.minimize();

	auto & gp = gui.getGroup(lightsParams.getName())
					.getGroup(lightsSettingsParams.getName())
					.getGroup(pointParams.getName());
	if (bPoint)
		gp.maximize();
	else
		gp.minimize();

	//--

	// Spot

	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(spotParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(spotParams.getName())
		.getGroup(spotOrientation.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(spotParams.getName())
		.getGroup(spotColorsParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(spotParams.getName())
		.getGroup(spotPosition.getName())
		.minimize();

	auto & gs = gui.getGroup(lightsParams.getName())
					.getGroup(lightsSettingsParams.getName())
					.getGroup(spotParams.getName());
	if (bSpot)
		gs.maximize();
	else
		gs.minimize();

	//--

	// Directional

	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(directionalParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(directionalParams.getName())
		.getGroup(directionalOrientation.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(directionalParams.getName())
		.getGroup(directionalColorsParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(directionalParams.getName())
		.getGroup(directionalPosition.getName())
		.minimize();

	auto & gd = gui.getGroup(lightsParams.getName())
					.getGroup(lightsSettingsParams.getName())
					.getGroup(directionalParams.getName());
	if (bDirectional)
		gd.maximize();
	else
		gd.minimize();

	//--

	// Area

	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(areaParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(areaParams.getName())
		.getGroup(areaOrientation.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(areaParams.getName())
		.getGroup(areaSize.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(areaParams.getName())
		.getGroup(areaColorsParams.getName())
		.minimize();
	gui.getGroup(lightsParams.getName())
		.getGroup(lightsSettingsParams.getName())
		.getGroup(areaParams.getName())
		.getGroup(areaPosition.getName())
		.minimize();

	auto & gar = gui.getGroup(lightsParams.getName())
					 .getGroup(lightsSettingsParams.getName())
					 .getGroup(areaParams.getName());
	if (bArea)
		gar.maximize();
	else
		gar.minimize();

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

	refreshGui(true);
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

	//----

	setupParametersLights();

	setupParametersShadows();

	//---

	parameters.add(bGui_Shadows);
	parameters.add(lightsParams);

	//--

	// Callbacks

	ofAddListener(lightsParams.parameterChangedE(), this, &SurfingLights::ChangedLights);
	ofAddListener(powersParams.parameterChangedE(), this, &SurfingLights::ChangedPowers);
	ofAddListener(shadowParams.parameterChangedE(), this, &SurfingLights::ChangedShadow);

	//--

	startup();
}

//--------------------------------------------------------------
void SurfingLights::setupParametersLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupParametersLights()";

	float sz = SURFING__PBR__SCENE_SIZE_UNIT * 1.5f;

	//--

	bGui.set("UI LIGHTS", false);

	bAnimLights.set("Anim Lights", false);
	bAnimLightsMouse.set("Mouse Lights", false);

	bDebug.set("Debug", true);
	bDebugLights.set("Debug Lights", true);
	bDebugShadow.set("Debug Shadow", false);

	bAutoLayout.set("Refresh Gui", true);

	//--

	vResetAllLights.set("Reset All Lights");

	//--

	// Groups

	parameters.setName("LIGHTS");

	lightsParams.setName("Lights");
	lightsSettingsParams.setName("Lights Settings");

	params_Enablers.setName("Enable");
	params_Extra.setName("Extra");

	pointParams.setName("L Point");
	spotParams.setName("L Spot");
	directionalParams.setName("L Direct");
	areaParams.setName("L Area");

	pointColorsParams.setName("P Colors");
	directionalColorsParams.setName("D Colors");
	spotColorsParams.setName("S Colors");
	areaColorsParams.setName("A Colors");

	powersParams.setName("Powers");
	globalColorsParams.setName("Global Colors");

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	globalColor.set("Global Color",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	globalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	//--

	// Point

	bPoint.set("Point", false);
	bPointShadow.set("P Shadow", true);
	vPointReset.set("P Reset");
	pointPosition.set("P Position", glm::vec3(), glm::vec3(-sz), glm::vec3(sz));

	pointAmbientColor.set("P Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointSpecularColor.set("P Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointDiffuseColor.set("P Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	pointGlobalColor.set("P Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	pointGlobalColor.set("P Global",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif
	pointSizeFar.set("P Size Far", 0, 0, 1);

	//--

	// Directional

	bDirectional.set("Direct", false);
	bDirectionalShadow.set("D Shadow", true);
	vDirectionalReset.set("D Reset");
	directionalPosition.set("D Position", glm::vec3(0), glm::vec3(-sz), glm::vec3(sz));
	directionalOrientation.set("D Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));

	directionalAmbientColor.set("D Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalSpecularColor.set("D Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalDiffuseColor.set("D Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	directionalGlobalColor.set("D Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	directionalGlobalColor.set("D Global",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif
	directionalSizeFar.set("D Size Far", 0, 0, 1);
	directionalSizeNear.set("D Size Near", 0, 0, 1);

	//--

	// Spot

	bSpot.set("Spot", false);
	bSpotShadow.set("S Shadow", true);
	vSpotReset.set("S Reset");
	spotPosition.set("S Position", glm::vec3(), glm::vec3(-sz), glm::vec3(sz));

	spotAmbientColor.set("S Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotSpecularColor.set("S Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotDiffuseColor.set("S Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	spotGlobalColor.set("S Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	spotGlobalColor.set("Global Color",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif

	spotOrientation.set("S Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));

	spotCutOff.set("S CutOff", 1, 0, 90);
	spotConcentration.set("S Concentration", 1, 0, 128);
	spotSizeFar.set("S Size Far", 0, 0, 1);

	//--

	// Area

	bArea.set("Area", false);
	bAreaShadow.set("A Shadow", true);
	vAreaReset.set("A Reset");
	areaPosition.set("A Position", glm::vec3(0), glm::vec3(-sz), glm::vec3(sz));
	areaOrientation.set("A Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));

	areaAmbientColor.set("A Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	areaSpecularColor.set("A Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	areaDiffuseColor.set("A Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));

#ifdef SURFING__PBR__PLANE_COLORS_NO_ALPHA
	areaGlobalColor.set("A Global",
		ofFloatColor(1.f), ofFloatColor(0.f), ofFloatColor(1.f));
#else
	areaGlobalColor.set("A Global",
		ofFloatColor(1.f, 1.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
#endif
	modeAnimArea.set("Mode Anim Area", 0, 0, 2);

	float sza = SURFING__PBR__SCENE_SIZE_UNIT;

	areaSize.set("A Size", glm::vec2(400, 120),
		glm::vec2(sza / 100, sza / 100),
		glm::vec2(sza, sz));
	areaSizeFar.set("A Size Far", 0, 0, 1);

	//--

	pointGlobalColor.set(ofFloatColor(1));
	directionalGlobalColor.set(ofFloatColor(1));
	spotGlobalColor.set(ofFloatColor(1));
	areaGlobalColor.set(ofFloatColor(1));

	//--

	bSmoothLights.set("Smooth Lights", false); // Default Low poly

	// Powers
	pointPower.set("P Point", 0.1, 0, 1);
	spotPower.set("P Spot", 0.1, 0, 1);
	directionalPower.set("P Direct", 0.1, 0, 1);
	areaPower.set("P Area", 0.1, 0, 1);

	//--

	// Lights

	// Point
	pointParams.add(pointPower);
	pointParams.add(pointGlobalColor);
	pointColorsParams.add(pointAmbientColor);
	pointColorsParams.add(pointDiffuseColor);
	pointColorsParams.add(pointSpecularColor);
	pointParams.add(pointColorsParams);
	pointParams.add(pointPosition);
	pointParams.add(pointSizeFar);
	pointParams.add(bPointShadow);
	pointParams.add(vPointReset);

	// Directional
	directionalParams.add(directionalPower);
	directionalParams.add(directionalGlobalColor);
	directionalColorsParams.add(directionalAmbientColor);
	directionalColorsParams.add(directionalDiffuseColor);
	directionalColorsParams.add(directionalSpecularColor);
	directionalParams.add(directionalColorsParams);
	directionalParams.add(directionalPosition);
	directionalParams.add(directionalOrientation);
	directionalParams.add(directionalSizeFar);
	directionalParams.add(directionalSizeNear);
	directionalParams.add(bDirectionalShadow);
	directionalParams.add(vDirectionalReset);

	// Spot
	spotParams.add(spotPower);
	spotParams.add(spotGlobalColor);
	spotColorsParams.add(spotAmbientColor);
	spotColorsParams.add(spotDiffuseColor);
	spotColorsParams.add(spotSpecularColor);
	spotParams.add(spotColorsParams);
	spotParams.add(spotCutOff);
	spotParams.add(spotConcentration);
	spotParams.add(spotPosition);
	spotParams.add(spotOrientation);
	spotParams.add(spotSizeFar);
	spotParams.add(bSpotShadow);
	spotParams.add(vSpotReset);

	// Area
	areaParams.add(areaPower);
	areaParams.add(areaGlobalColor);
	areaColorsParams.add(areaAmbientColor);
	areaColorsParams.add(areaDiffuseColor);
	areaColorsParams.add(areaSpecularColor);
	areaParams.add(areaColorsParams);
	areaParams.add(areaPosition);
	areaParams.add(areaOrientation);
	areaParams.add(areaSizeFar);
	areaParams.add(areaSize);
	areaParams.add(vAreaReset);
	areaParams.add(bAreaShadow);

	//--

	// Enablers
	params_Enablers.add(bPoint);
	params_Enablers.add(bDirectional);
	params_Enablers.add(bSpot);
	params_Enablers.add(bArea);
	lightsParams.add(params_Enablers);

	// Powers
	powersParams.add(pointPower);
	powersParams.add(directionalPower);
	powersParams.add(spotPower);
	powersParams.add(areaPower);
	lightsParams.add(powersParams);

	//--

	// Global colors
	lightsParams.add(globalColor);

	globalColorsParams.add(pointGlobalColor);
	globalColorsParams.add(directionalGlobalColor);
	globalColorsParams.add(spotGlobalColor);
	globalColorsParams.add(areaGlobalColor);
	globalColorsParams.add(indexFromColorToGlobal.set("Get G Color", 0, 0, 2));
	globalColorsParams.add(nameSourceGlobal.set("from Source", "NONE"));
	nameSourceGlobal.setSerializable(false);
	lightsParams.add(globalColorsParams);

	lightsSettingsParams.add(pointParams);
	lightsSettingsParams.add(directionalParams);
	lightsSettingsParams.add(spotParams);
	lightsSettingsParams.add(areaParams);
	lightsParams.add(lightsSettingsParams);

	//--

	// Extra
	params_Extra.add(bSmoothLights);
	params_TestAnims.setName("Test Anims");
	params_TestAnims.add(bAnimLights);
	params_TestAnims.add(modeAnimArea);
	params_TestAnims.add(bAnimLightsMouse);
	params_Extra.add(params_TestAnims);
	params_Extra.add(bAutoLayout);
	params_Extra.add(bDebugLights);
	params_Extra.add(bDebugShadow);
	lightsParams.add(params_Extra);

	lightsParams.add(bDebug);
	lightsParams.add(vResetAllLights);

	// Exclude
	globalColor.setSerializable(false);
	globalColorsParams.setSerializable(false);
	pointGlobalColor.setSerializable(false);
	spotGlobalColor.setSerializable(false);
	directionalGlobalColor.setSerializable(false);
	areaGlobalColor.setSerializable(false);
}

//--------------------------------------------------------------
void SurfingLights::setupParametersShadows() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupParametersShadows()";

	// Shadows
	bGui_Shadows.set("UI SHADOWS", false);
	bDrawShadow.set("Draw Shadows", true);
	shadowBias.set("Bias", 0.07, 0.0, 1.0);
	shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0);
	indexShadowType.set("Type", 0, 0, OF_SHADOW_TYPE_TOTAL - 1);
	nameShadowType.set("Name", "NONE");
	shadowSampleRadius.set("Radius", 2.f, .0, 10.0);
	shadowStrength.set("Strength", SURFING__PBR__SHADOW_DEFAULT_STRENGTH, 0.f, 1.f);
	shadowResolution.set("Resolution", 1024, 256, 2048);
	bDebugShadow.set("Debug Shadow", true);
	vResetShadow.set("Reset Shadow");

	nameShadowType.setSerializable(false);

	shadowParams.setName("SHADOWS");
	shadowParams.add(bDrawShadow);
	shadowParams.add(shadowStrength);
	shadowParams.add(indexShadowType);
	shadowParams.add(nameShadowType);
	shadowParams.add(shadowBias);
	shadowParams.add(shadowNormalBias);
	shadowParams.add(shadowSampleRadius);
	shadowParams.add(shadowResolution);
	shadowParams.add(bDebugShadow);
	shadowParams.add(vResetShadow);
}

//--------------------------------------------------------------
void SurfingLights::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:startup()";

	doResetAllLights(true);
	doResetShadow();

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	callback_t f = std::bind(&SurfingLights::save, this);
	autoSaver.setFunctionSaver(f);
#endif

	//--

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

		if (bAutoLayout) refreshGui();
	}

	//--

	if (bFlagDoRefreshPowers) {
		bFlagDoRefreshPowers = false;

		doRefreshPowers();
	}

	else if (bFlagDoRefreshPowerPoint) {
		bFlagDoRefreshPowerPoint = false;

		doRefreshPowerPoint();
	}

	else if (bFlagDoRefreshPowerDirect) {
		bFlagDoRefreshPowerDirect = false;

		doRefreshPowerDirect();
	}

	else if (bFlagDoRefreshPowerSpot) {
		bFlagDoRefreshPowerSpot = false;

		doRefreshPowerSpot();
	}

	else if (bFlagDoRefreshPowerArea) {
		bFlagDoRefreshPowerArea = false;

		doRefreshPowerArea();
	}

	//--

	else if (bFlagDoResetAllLights) {
		bFlagDoResetAllLights = false;

		doResetAllLights(true);
	}

	else if (bFlagDoResetPoint) {
		bFlagDoResetPoint = false;

		doResetPoint(true);
	}

	else if (bFlagDoResetDirectional) {
		bFlagDoResetDirectional = false;

		doResetDirectional(true);
	}

	else if (bFlagDoResetSpot) {
		bFlagDoResetSpot = false;

		doResetSpot(true);
	}

	else if (bFlagDoResetArea) {
		bFlagDoResetArea = false;

		doResetArea(true);
	}

	if (bFlagIndexFromColorToGlobal) {
		bFlagIndexFromColorToGlobal = false;

		if (bAttendingColors) return;

		doIndexFromColorToGlobal();
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

	// Point
	{
		lights[0]->setPosition(pointPosition);
	}

	// Directional
	{
		lights[1]->setPosition(directionalPosition);
		glm::vec3 rad = glm::radians(directionalOrientation.get());
		glm::quat q = glm::quat(rad);
		lights[1]->setOrientation(q);
	}

	// Spot
	{
		lights[2]->setPosition(spotPosition);
		glm::vec3 rad = glm::radians(spotOrientation.get());
		glm::quat q = glm::quat(rad);
		lights[2]->setOrientation(q);
	}

	// Area
	{
		lights[3]->setPosition(areaPosition);
		glm::vec3 rad = glm::radians(areaOrientation.get());
		glm::quat q = glm::quat(rad);
		lights[3]->setOrientation(q);
	}
}

//--------------------------------------------------------------
void SurfingLights::updateAnims() {

	// Anim lights by time

	auto t = ofGetElapsedTimef() * 0.7;

	if (bAnimLights) {

		const float r = SURFING__PBR__SCENE_SIZE_UNIT * 0.25f;

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

		// Area
		updateAnimsArea();
	}

	//--

	// Move lights by mouse

	if (bAnimLightsMouse) {
		mouseX = ofGetMouseX();
		mouseY = ofGetMouseY();

		float r = SURFING__PBR__SCENE_SIZE_UNIT * 3;
		float x = ofMap(mouseX, 0, ofGetWidth(), -r, r, true);
		float y = ofMap(mouseY, 0, ofGetHeight(), r, -r, true);
		float oy = 200;

		////TODO: make cool animators
		//float za = -cos(t) * r + areaPosition.get().z;

		// Point
		lights[0]->setPosition(x, y + oy, pointPosition.get().z);
		lights[0]->setOrientation(glm::vec3(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));

		// Directional
		lights[1]->setPosition(x, y, directionalPosition.get().z);
		lights[1]->setOrientation(glm::vec3(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));

		// Spot
		lights[2]->setPosition(x, y - oy, spotPosition.get().z);
		lights[2]->setOrientation(glm::vec3(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));

		// Area
		lights[3]->setPosition(x, y - oy, areaPosition.get().z);
		lights[3]->setOrientation(glm::vec3(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));
	}
}

//--------------------------------------------------------------
void SurfingLights::updateAnimsArea() {
	if (lights.size() < 4) return;

	bool bMouse = false; //disabled

	float elapsedTime = ofGetElapsedTimef();
	float deltaTime = ofClamp(ofGetLastFrameTime(), 1.f / 10000.0, 1.f / 5.f);

	if (modeAnimArea == 0) {
		float speed;
		if (!bMouse)
			speed = 5;
		else
			speed = ofMap(ofGetMouseX(), 100, ofGetWidth() - 100, 0.1, 24, true);
		orbitSpeed += deltaTime * speed;
		lights[3]->setPosition(cos(orbitSpeed) * -600, (sin(elapsedTime * 1.1) * 200), sin(orbitSpeed) * -600);
		lights[3]->lookAt(glm::vec3(0, -150, 0));
		lights[3]->setAmbientColor(ofFloatColor(0.3, 0.3, 0.3));
	}

	else if (modeAnimArea == 1) {
		float mousePercent;
		if (!bMouse)
			mousePercent = 0.01;
		else
			mousePercent = ofMap(ofGetMouseX(), 100, ofGetWidth() - 100, 0, 1, true);

		highwaySpeed = ofMap(mousePercent, 0, 1, 500, 5000, true);
		lights[3]->setAmbientColor(ofFloatColor(0.0, 0.0, 0.0));
		float zPosition = ofClamp(sin(elapsedTime), 0, 1) * 500.0 + 200;
		lights[3]->setPosition(lights[3]->getPosition().x + deltaTime * highwaySpeed, 350, zPosition);
		if (lights[3]->getPosition().x > 1500) {
			lights[3]->setPosition(-1500, lights[3]->getPosition().y, zPosition);
		}
		lights[3]->lookAt(glm::vec3(lights[3]->getPosition().x, lights[3]->getPosition().y - 300, lights[3]->getPosition().z));
		lights[3]->rollDeg(90);

		float noiseValue = ofSignedNoise(lights[3]->getPosition().y * mousePercent * 0.001, lights[3]->getPosition().x * mousePercent * 0.001, mousePercent * elapsedTime);
		if (noiseValue > 1.0 - mousePercent * 0.5 && bump > 1.0) {
			bump = 0.0;
			bumpHeight = ofRandom(10, 80);
		}
		bump += deltaTime * (3.0 + (mousePercent + 0.2));
	}

	else if (modeAnimArea == 2) {
		float mousePercent;
		if (!bMouse)
			mousePercent = 0.1;
		else
			mousePercent = ofMap(ofGetMouseX(), 100, ofGetWidth() - 100, 0, 1, true);
		lights[3]->setPosition(cos(elapsedTime * 2.0) * -600, fabs(sin(elapsedTime * 2.0) * 600) - 200, sin(elapsedTime * 3.4f) * 200 + 400);
		lights[3]->tiltDeg(deltaTime * (mousePercent * 360.0 * 3.0 + 5.0f));
	}
}

//--

//--------------------------------------------------------------
void SurfingLights::doResetAllLights(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetAllLights()";

	if (bHard) {
		if (!bPoint) bPoint = true; //enable
		if (!bDirectional) bDirectional = true; //enable
		if (bSpot) bSpot = false;
		if (bArea) bArea = false;
	}

	doResetPoint(bHard);
	doResetDirectional(bHard);
	doResetSpot(bHard);
	doResetArea(bHard);

	doRefreshPowers();
}

// Point
//--------------------------------------------------------------
void SurfingLights::doResetPoint(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetPoint()";

	bPointShadow = true;

	// touch colors too
	if (bHard) {

		pointGlobalColor.set(ofFloatColor(1));
	}

	pointPosition.set(glm::vec3(
		0,
		SURFING__PBR__SCENE_SIZE_UNIT,
		SURFING__PBR__SCENE_SIZE_UNIT * 0.3));

	pointSizeFar = 1;

	if (bHard)
		pointPower = SURFING__PBR__HELPER_GLOBAL_POWER_RESET;
}

// Directional
//--------------------------------------------------------------
void SurfingLights::doResetDirectional(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetDirectional()";

	bDirectionalShadow = true;

	// touch colors too
	if (bHard) {

		directionalGlobalColor.set(ofFloatColor(1));
	}

	directionalPosition.set(glm::vec3(
		0,
		SURFING__PBR__SCENE_SIZE_UNIT,
		SURFING__PBR__SCENE_SIZE_UNIT * 0.4));

	directionalOrientation.set(glm::vec3(-60, 0, 0));

	directionalSizeNear = 0.5;
	directionalSizeFar = 1;

	if (bHard)
		directionalPower = SURFING__PBR__HELPER_GLOBAL_POWER_RESET;
}

// Spot
//--------------------------------------------------------------
void SurfingLights::doResetSpot(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetSpot()";

	bSpotShadow = true;

	// touch colors too
	if (bHard) {
		spotGlobalColor.set(ofFloatColor(1));
	}

	spotPosition.set(glm::vec3(
		0,
		SURFING__PBR__SCENE_SIZE_UNIT,
		SURFING__PBR__SCENE_SIZE_UNIT * 0.5));

	spotOrientation.set(glm::vec3(-60, 0, 0));

	//spotCutOff.set(45);
	spotCutOff.set(90);
	spotConcentration.set(10);

	spotSizeFar = 1;

	if (bHard)
		spotPower = SURFING__PBR__HELPER_GLOBAL_POWER_RESET;
}

// Area
//--------------------------------------------------------------
void SurfingLights::doResetArea(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetArea()";

	bAreaShadow = true;

	areaSize = { SURFING__PBR__SCENE_SIZE_UNIT / 8, SURFING__PBR__SCENE_SIZE_UNIT / 8 };

	// touch colors too
	if (bHard) {
		areaGlobalColor.set(ofFloatColor(1));
	}

	areaPosition.set(glm::vec3(
		0,
		SURFING__PBR__SCENE_SIZE_UNIT,
		SURFING__PBR__SCENE_SIZE_UNIT * 0.6));

	areaOrientation.set(glm::vec3(-60, 0, 0));

	areaSizeFar = 1;

	if (bHard)
		areaPower = SURFING__PBR__HELPER_GLOBAL_POWER_RESET;
}

//--

//--------------------------------------------------------------
void SurfingLights::ChangedLights(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedLights " << name << " : " << e;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
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

	else if (name == vAreaReset.getName()) {
		bFlagDoResetArea = true;

	}

	//--

	else if (name == vResetAllLights.getName()) {
		bFlagDoResetAllLights = true;
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

	else if (name == bArea.getName()) {
		bFlagRefreshGui = true;
		//fix
		if (bArea) restoreAnims();
	}

	//--

	else if (name == spotCutOff.getName()) {
		if (lights.size() < 3) return;
		lights[2]->setSpotlightCutOff(spotCutOff.get());
	}

	else if (name == spotConcentration.getName()) {
		if (lights.size() < 3) return;
		lights[2]->setSpotConcentration(spotConcentration.get());
	}

	else if (name == areaSize.getName()) {
		if (lights.size() < 4) return;
		lights[3]->setAreaLight(areaSize.get().x, areaSize.get().y);
	}

	else if (name == bSmoothLights.getName()) {
		ofSetSmoothLighting(bSmoothLights);
	}

	//--

	else if (name == bAnimLights.getName()) {
		if (bAnimLights && bAnimLightsMouse) bAnimLightsMouse.setWithoutEventNotifications(false);
		//if (bAnimLights)
		{
			restoreAnims();
		}
	}

	else if (name == bAnimLightsMouse.getName()) {
		if (bAnimLightsMouse && bAnimLights) bAnimLights.setWithoutEventNotifications(false);
		//if (bAnimLightsMouse)
		{
			restoreAnims();
		}
	}

	//--

	// Global colors

	else if (name == pointAmbientColor.getName() || name == pointDiffuseColor.getName() || name == pointSpecularColor.getName()) {
		if (bAttendingColors) return;
		bFlagIndexFromColorToGlobal = true;
	}

	else if (name == directionalAmbientColor.getName() || name == directionalDiffuseColor.getName() || name == directionalSpecularColor.getName()) {
		if (bAttendingColors) return;
		bFlagIndexFromColorToGlobal = true;
	}

	else if (name == spotAmbientColor.getName() || name == spotDiffuseColor.getName() || name == spotSpecularColor.getName()) {
		if (bAttendingColors) return;
		bFlagIndexFromColorToGlobal = true;
	}

	else if (name == areaAmbientColor.getName() || name == areaDiffuseColor.getName() || name == areaSpecularColor.getName()) {
		if (bAttendingColors) return;
		bFlagIndexFromColorToGlobal = true;
	}

	//--

	else if (name == globalColor.getName()) {
		if (bAttendingColors) return;

		if (globalColor.get() != globalColor_) {
			globalColor_ = globalColor.get();

			pointGlobalColor.set(globalColor.get());
			directionalGlobalColor.set(globalColor.get());
			spotGlobalColor.set(globalColor.get());
			areaGlobalColor.set(globalColor.get());
		}
	}

	else if (name == pointGlobalColor.getName()) {
		if (bAttendingColors) return;

		if (pointAmbientColor.get() != pointGlobalColor.get())
			pointAmbientColor.set(pointGlobalColor.get());

		if (pointDiffuseColor.get() != pointGlobalColor.get())
			pointDiffuseColor.set(pointGlobalColor.get());

		if (pointSpecularColor.get() != pointGlobalColor.get())
			pointSpecularColor.set(pointGlobalColor.get());
	}

	else if (name == directionalGlobalColor.getName()) {
		if (bAttendingColors) return;

		if (directionalAmbientColor.get() != directionalGlobalColor.get())
			directionalAmbientColor.set(directionalGlobalColor.get());

		if (directionalDiffuseColor.get() != directionalGlobalColor.get())
			directionalDiffuseColor.set(directionalGlobalColor.get());

		if (directionalSpecularColor.get() != directionalGlobalColor.get())
			directionalSpecularColor.set(directionalGlobalColor.get());
	}

	else if (name == spotGlobalColor.getName()) {
		if (bAttendingColors) return;

		if (spotAmbientColor.get() != spotGlobalColor.get())
			spotAmbientColor.set(spotGlobalColor.get());

		if (spotDiffuseColor.get() != spotGlobalColor.get())
			spotDiffuseColor.set(spotGlobalColor.get());

		if (spotSpecularColor.get() != spotGlobalColor.get())
			spotSpecularColor.set(spotGlobalColor.get());
	}

	else if (name == areaGlobalColor.getName()) {
		if (bAttendingColors) return;

		if (areaAmbientColor.get() != areaGlobalColor.get())
			areaAmbientColor.set(areaGlobalColor.get());

		if (areaDiffuseColor.get() != areaGlobalColor.get())
			areaDiffuseColor.set(areaGlobalColor.get());

		if (areaSpecularColor.get() != areaGlobalColor.get())
			areaSpecularColor.set(areaGlobalColor.get());
	}

	else if (name == indexFromColorToGlobal.getName()) {
		bFlagIndexFromColorToGlobal = true;
	}
}

//--------------------------------------------------------------
void SurfingLights::ChangedShadow(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedShadow " << name << ": " << e;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	if (name == vResetShadow.getName()) {
		bFlagDoResetShadow = true;
	}

	//--

	else if (name == shadowResolution.getName()) {
#ifndef TARGET_OPENGLES
		ofShadow::setAllShadowDepthResolutions(shadowResolution, shadowResolution);
#endif
	}

	else if (name == shadowBias.getName()) {
		ofShadow::setAllShadowBias(shadowBias.get());
	}

	else if (name == shadowNormalBias.getName()) {
		ofShadow::setAllShadowNormalBias(shadowNormalBias.get());
	}

	else if (name == shadowSampleRadius.getName()) {
		ofShadow::setAllShadowSampleRadius(shadowSampleRadius.get());
	}

	else if (name == shadowStrength.getName()) {
		for (size_t i = 0; i < lights.size(); i++) {
			auto & light = lights[i];
			light->getShadow().setStrength(shadowStrength.get());
		}
	}

	else if (name == indexShadowType.getName()) {
		//int i = indexShadowType.get();
		//if (i == OF_SHADOW_TYPE_TOTAL) {
		//	i = 0;
		//}
		//indexShadowType.set(i);

		switch (indexShadowType.get()) {
		case 0:
			nameShadowType = "HARD";
			break;
		case 1:
			nameShadowType = "PCF_LOW";
			break;
		case 2:
			nameShadowType = "PCF_MED";
			break;
		case 3:
			nameShadowType = "PCF_HIGH";
			break;
		}

		ofShadow::setAllShadowTypes((ofShadowType)indexShadowType.get());
	}

	////workflow
	//else if (name == bDebugShadow.getName()) {
	//	if (bDebugShadow) {
	//		if (!bDebug) bDebug = true;
	//	}
	//}
}

//--

//--------------------------------------------------------------
void SurfingLights::ChangedPowers(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedPowers " << name << " : " << e;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	if (name == pointPower.getName()) {
		bFlagDoRefreshPowerPoint = true;
	}

	else if (name == directionalPower.getName()) {
		bFlagDoRefreshPowerDirect = true;
	}

	else if (name == spotPower.getName()) {
		bFlagDoRefreshPowerSpot = true;
	}

	else if (name == areaPower.getName()) {
		bFlagDoRefreshPowerArea = true;
	}
}

//--

//--------------------------------------------------------------
void SurfingLights::doRefreshPowers() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshPowers()";

	doRefreshPowerPoint();
	doRefreshPowerDirect();
	doRefreshPowerSpot();
	doRefreshPowerArea();
}

//--------------------------------------------------------------
void SurfingLights::doRefreshPowerPoint() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshPowerPoint()";

	auto ca = pointAmbientColor.get();
	auto cd = pointDiffuseColor.get();
	auto cs = pointSpecularColor.get();

	float brg = ofClamp(pointPower,
		SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MIN, SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MAX);
	ca.setBrightness(brg);
	cd.setBrightness(brg);
	cs.setBrightness(brg);

	pointAmbientColor.set(ca);
	pointDiffuseColor.set(cd);
	pointSpecularColor.set(cs);
}

//--------------------------------------------------------------
void SurfingLights::doRefreshPowerDirect() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshPowerDirect()";

	auto ca = directionalAmbientColor.get();
	auto cd = directionalDiffuseColor.get();
	auto cs = directionalSpecularColor.get();

	float brg = ofClamp(directionalPower,
		SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MIN, SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MAX);
	ca.setBrightness(brg);
	cd.setBrightness(brg);
	cs.setBrightness(brg);

	directionalAmbientColor.set(ca);
	directionalDiffuseColor.set(cd);
	directionalSpecularColor.set(cs);
}

//--------------------------------------------------------------
void SurfingLights::doRefreshPowerSpot() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshPowerSpot()";

	auto ca = spotAmbientColor.get();
	auto cd = spotDiffuseColor.get();
	auto cs = spotSpecularColor.get();

	float brg = ofClamp(spotPower,
		SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MIN, SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MAX);
	ca.setBrightness(brg);
	cd.setBrightness(brg);
	cs.setBrightness(brg);

	spotAmbientColor.set(ca);
	spotDiffuseColor.set(cd);
	spotSpecularColor.set(cs);
}

//--------------------------------------------------------------
void SurfingLights::doRefreshPowerArea() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doRefreshPowerArea()";

	auto ca = areaAmbientColor.get();
	auto cd = areaDiffuseColor.get();
	auto cs = areaSpecularColor.get();

	float brg = ofClamp(areaPower,
		SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MIN, SURFING__PBR__HELPER_GLOBAL_POWER_LIMIT_MAX);
	ca.setBrightness(brg);
	cd.setBrightness(brg);
	cs.setBrightness(brg);

	areaAmbientColor.set(ca);
	areaDiffuseColor.set(cd);
	areaSpecularColor.set(cs);
}

//--

//--------------------------------------------------------------
void SurfingLights::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Save -> " << pathSettings;
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Save -> " << pathSettingsShadows;

	ofxSurfing::saveSettings(shadowParams, pathSettingsShadows);
	ofxSurfing::saveSettings(lightsParams, pathSettings);
}

//--------------------------------------------------------------
bool SurfingLights::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Load -> " << pathSettings;
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Load -> " << pathSettingsShadows;

	bool b;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	b = ofxSurfing::loadSettings(shadowParams, pathSettingsShadows);
	b &= ofxSurfing::loadSettings(lightsParams, pathSettings);

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	//fix
	bFlagIndexFromColorToGlobal = true;

	return b;
}

//--------------------------------------------------------------
void SurfingLights::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:exit()";

// Not required to be called bc it's using the auto saver!
#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	save();
#endif

	bDoneExit = true;
}

//--------------------------------------------------------------
void SurfingLights::begin() {
	if (!ofIsGLProgrammableRenderer()) {
		ofEnableLighting();
	}
}

//TODO: works?
//--------------------------------------------------------------
shared_ptr<ofLight> SurfingLights::getLight(size_t index) {
	if (index > lights.size() - 1) return nullptr;

	return lights[index];
}

//--------------------------------------------------------------
void SurfingLights::keyPressed(int key) {

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:keyPressed(" << (char)key << ")";

	// toggle enable lights
	switch (key) {
	case '1':
		bPoint = !bPoint;
		break;
	case '2':
		bDirectional = !bDirectional;
		break;
	case '3':
		bSpot = !bSpot;
		break;
	case '4':
		bArea = !bArea;
		break;
	}
}

//--------------------------------------------------------------
void SurfingLights::end() {
	if (!ofIsGLProgrammableRenderer()) {
		ofDisableLighting();
	}
}

//--------------------------------------------------------------
void SurfingLights::doResetShadow() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetShadow()";

	bDrawShadow.set(true);

	shadowStrength.set(SURFING__PBR__SHADOW_DEFAULT_STRENGTH);
	indexShadowType.set(2);
	shadowBias.set(0.1);
	shadowNormalBias.set(0.007);
	shadowSampleRadius.set(2.f);
	shadowResolution.set(SURFING__PBR__SHADOW_DEFAULT_RESOLUTION);
}

//--------------------------------------------------------------
void SurfingLights::doIndexFromColorToGlobal() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doIndexFromColorToGlobal()";

	string s = "NONE";

	const int i = indexFromColorToGlobal.get();

	if (i == 0)
		s = "Ambient";
	else if (i == 1)
		s = "Specular";
	else if (i == 2)
		s = "Diffuse";

	nameSourceGlobal.set(s);

	//workflow. auto get
	doGetGlobalColorFromIndexColor();
}

//--------------------------------------------------------------
void SurfingLights::doGetGlobalColorFromIndexColor() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doGetGlobalColorFromIndexColor()";

	bAttendingColors = true;
	// avoid affecting the colors when touching global color!
	// this disables colors callbacks!

	const int i = indexFromColorToGlobal.get();
	ofFloatColor c = ofFloatColor(1.f, 1.f);

	ofFloatColor ca;
	ofFloatColor cd;
	ofFloatColor cs;

	//--

	ca = pointAmbientColor.get();
	cd = pointDiffuseColor.get();
	cs = pointSpecularColor.get();

	if (i == 0)
		c = ca;
	else if (i == 1)
		c = cs;
	else if (i == 2)
		c = cd;

	pointGlobalColor.set(c);

	//--

	ca = directionalAmbientColor.get();
	cd = directionalDiffuseColor.get();
	cs = directionalSpecularColor.get();

	if (i == 0)
		c = ca;
	else if (i == 1)
		c = cs;
	else if (i == 2)
		c = cd;

	directionalGlobalColor.set(c);

	//--

	ca = spotAmbientColor.get();
	cd = spotDiffuseColor.get();
	cs = spotSpecularColor.get();

	if (i == 0)
		c = ca;
	else if (i == 1)
		c = cs;
	else if (i == 2)
		c = cd;

	spotGlobalColor.set(c);

	//--

	ca = areaAmbientColor.get();
	cd = areaDiffuseColor.get();
	cs = areaSpecularColor.get();

	if (i == 0)
		c = ca;
	else if (i == 1)
		c = cs;
	else if (i == 2)
		c = cd;

	areaGlobalColor.set(c);

	//--

	bAttendingColors = false;
}