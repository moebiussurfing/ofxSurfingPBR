#include "SurfingLights.h"

//--------------------------------------------------------------
SurfingLights::SurfingLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:SurfingLights()";
}

//--------------------------------------------------------------
SurfingLights::~SurfingLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:~SurfingLights()";

	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingLights::Changed);
	ofRemoveListener(params_Lights.parameterChangedE(), this, &SurfingLights::ChangedLights);
	ofRemoveListener(params_Brights.parameterChangedE(), this, &SurfingLights::ChangedBrights);
}

//--

//--------------------------------------------------------------
void SurfingLights::setFunctionRenderScene(callback_t f) {
	f_RenderScene = f;
}

//--------------------------------------------------------------
void SurfingLights::setupPBRlights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupPBRlights()";

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

		//light->setPosition(210, 330.0, 750);
		//light->setAmbientColor(ofFloatColor(0.4));

		light->getShadow().setStrength(SURFING__PBR__SHADOW_STRENGTH);

		//TODO
		glm::quat xq = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1, 0, 0));
		glm::quat yq = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
		light->setOrientation(yq * xq);

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

		//light->setPosition(210, 330.0, 750);
		//light->setAmbientColor(ofFloatColor(0.4));

		light->getShadow().setStrength(SURFING__PBR__SHADOW_STRENGTH);

		//TODO
		glm::quat xq = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1, 0, 0));
		glm::quat yq = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
		light->setOrientation(yq * xq);

		light->getShadow().setGlCullingEnabled(false);

		lights.push_back(light);
	}

	ofShadow::enableAllShadows();
	ofShadow::setAllShadowTypes(OF_SHADOW_TYPE_PCF_LOW);

	//----

	/*
	const int numLights = 1; 

	for (int i = 0; i < numLights; i++) {
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
void SurfingLights::updatePBRlights() {

	// POINT
	{
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

	// DIRECTIONAL
	{
		if (bDirectional) {
			if (!lights[1]->getIsEnabled()) lights[1]->enable();
		} else {
			if (lights[1]->getIsEnabled()) lights[1]->disable();
		}

		lights[1]->setPosition(directionalLightPosition);

		lights[1]->setAmbientColor(directionalLightAmbientColor);
		lights[1]->setDiffuseColor(directionalLightDiffuseColor);
		lights[1]->setSpecularColor(directionalLightSpecularColor);

		//TODO
		//glm::quat xq = glm::angleAxis((directionalLightOrientation.get()));
		//lights[1]->setOrientation(directionalLightOrientation.get());

		//glm::quat xq = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1, 0, 0));
		//glm::quat yq = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
		//light->setOrientation(yq * xq);
	}

	// SPOT
	{
		if (bSpot) {
			if (!lights[2]->getIsEnabled()) lights[2]->enable();
		} else {
			if (lights[2]->getIsEnabled()) lights[2]->disable();
		}

		lights[2]->setPosition(spotLightPosition);

		lights[2]->setAmbientColor(spotLightAmbientColor);
		lights[2]->setDiffuseColor(spotLightDiffuseColor);
		lights[2]->setSpecularColor(spotLightSpecularColor);
	}

	//ofShadow::setAllShadowBias(shadowBias.get());
	//ofShadow::setAllShadowNormalBias(shadowNormalBias.get());

	//------

	//// shadow
	//for (auto & light : lights) {
	//	light->getShadow().setEnabled(bDrawShadow);
	//}

	//// lights
	////TODO: not used bc limited to 1 single light!
	//if (lights.size() > 0) {
	//	if (lights[0]->getType() == OF_LIGHT_POINT) {
	//		float tangle = ofGetElapsedTimef() * 1.05;
	//		lights[0]->setPosition(lightX, lightY, lightZ);
	//		lights[0]->setAmbientColor(lightAmbientColor);
	//	}
	//}

	//ofShadow::setAllShadowBias(shadowBias.get());
	//ofShadow::setAllShadowNormalBias(shadowNormalBias.get());
}

//--------------------------------------------------------------
void SurfingLights::drawPBRlights() {
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
void SurfingLights::drawDebugPBRlights() {
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
			if (bDebug) ofDrawSphere(light->getPosition(), 12);
		} else {
			if (bDebug) light->draw();
		}
		if (light->getShadow().getIsEnabled()) {
			if (bDebug && bDebugShadow) light->getShadow().drawFrustum();
		}
	}
}

//--------------------------------------------------------------
void SurfingLights::doResetLight() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetLight()";

	lightX = 0;
	lightY = SURFING__SCENE_SIZE_UNIT * 0.5;
	lightZ = 0;
	lightAmbientColor.set(ofFloatColor(1.f, 1.f));
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
}

//--------------------------------------------------------------
void SurfingLights::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:refreshGui()";

	//gui.getGroup(lightParams.getName()).minimize();
	//gui.getGroup(params_Lights.getName()).minimize();

	gui.getGroup(params_Lights.getName())
		.getGroup(params_Brights.getName())
		.minimize();

	gui.getGroup(params_Lights.getName())
		.getGroup(params_pointLight.getName())
		.minimize();

	gui.getGroup(params_Lights.getName())
		.getGroup(params_spotLight.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(params_spotLight.getName())
		.getGroup(spotLightPosition.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(params_spotLight.getName())
		.getGroup(spotLightOrientation.getName())
		.minimize();

	gui.getGroup(params_Lights.getName())
		.getGroup(params_directionalLight.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(params_directionalLight.getName())
		.getGroup(directionalLightPosition.getName())
		.minimize();
	gui.getGroup(params_Lights.getName())
		.getGroup(params_directionalLight.getName())
		.getGroup(directionalLightOrientation.getName())
		.minimize();
}

//--------------------------------------------------------------
void SurfingLights::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:setupGui()";

	gui.setup(parameters);

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

	bGui.set("UI LIGHTS", true);

	bAnimLights.set("Anim Lights", false);

	bDebug.set("Debug", true);
	bDebugShadow.set("Debug Shadow", true);
	bDebugLights.set("Debug Lights", true);

	//--

	float u = 1.5 * SURFING__SCENE_SIZE_UNIT;

	//vResetLight.set("Reset Light");
	//lightParams.setName("LightPBR");
	//lightAmbientColor.set("Light Ambient Color",
	//	ofFloatColor(1.f, 1.f),
	//	ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	//lightParams.add(lightX.set("X", 0.0f, -u, u));
	//lightParams.add(lightY.set("Y", 0.0f, -u, u));
	//lightParams.add(lightZ.set("Z", 0.0f, -u, u));
	//lightParams.add(lightAmbientColor);
	//lightParams.add(vResetLight);

	//------

	//// Clear
	//params_pointLight.clear();
	//params_spotLight.clear();
	//params_directionalLight.clear();
	//params_Lights.clear();
	//parameters.clear();

	//--

	// Groups
	parameters.setName("PBR_LIGHTS");
	params_Lights.setName("Lights");
	params_pointLight.setName("Point Light");
	params_spotLight.setName("Spot Light");
	params_directionalLight.setName("Direct Light");
	params_Brights.setName("Brights Power");

	//--

	// Point
	bPoint.set("Point", false);
	bPointReset.set("Reset Point", false);
	pointBright.set("Bright Point", 0.5, 0, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	pointLightAmbientColor.set("p Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointLightDiffuseColor.set("p Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointLightSpecularColor.set("p Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	pointLightPosition.set("p Pos", glm::vec3(), glm::vec3(-u), glm::vec3(u));

	// Directional
	bDirectional.set("Direct", false);
	bDirectionalReset.set("Reset Direct", false);
	directionalBright.set("Bright Dir", 0.5, 0, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	directionalLightAmbientColor.set("d Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalLightDiffuseColor.set("d Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalLightSpecularColor.set("d Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	directionalLightOrientation.set("d Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));
	directionalLightPosition.set("d Position", glm::vec3(0),
		glm::vec3(-u), glm::vec3(u));

	// Spot
	bSpot.set("Spot", false);
	bSpotReset.set("Reset Spot", false);
	spotBright.set("Bright Spot", 0.5, 0, SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX);
	spotLightAmbientColor.set("s Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotLightDiffuseColor.set("s Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotLightSpecularColor.set("s Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
	spotLightPosition.set("s Pos", glm::vec3(), glm::vec3(-u), glm::vec3(u));
	spotLightOrientation.set("s Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));
	spotLightCutOff.set("s CutOff", 1, 0, 90);
	spotLightConcetration.set("s Conc", 1, 0, 128);

	//--

	bSmoothLights.set("Smooth Lights", false); // Default Low Poly

	//-

	// Lights

	params_pointLight.add(pointLightAmbientColor);
	params_pointLight.add(pointLightDiffuseColor);
	params_pointLight.add(pointLightSpecularColor);
	params_pointLight.add(pointLightPosition);
	params_pointLight.add(bPointReset);

	params_directionalLight.add(directionalLightAmbientColor);
	params_directionalLight.add(directionalLightDiffuseColor);
	params_directionalLight.add(directionalLightSpecularColor);
	params_directionalLight.add(directionalLightPosition);
	params_directionalLight.add(directionalLightOrientation);
	params_directionalLight.add(bDirectionalReset);

	params_spotLight.add(spotLightAmbientColor);
	params_spotLight.add(spotLightDiffuseColor);
	params_spotLight.add(spotLightSpecularColor);
	params_spotLight.add(spotLightPosition);
	params_spotLight.add(spotLightOrientation);
	params_spotLight.add(spotLightCutOff);
	params_spotLight.add(spotLightConcetration);
	params_spotLight.add(bSpotReset);

	params_Brights.add(bPoint);
	params_Brights.add(pointBright);
	params_Brights.add(bDirectional);
	params_Brights.add(directionalBright);
	params_Brights.add(bSpot);
	params_Brights.add(spotBright);

	params_Lights.add(bPoint);
	params_Lights.add(bDirectional);
	params_Lights.add(bSpot);

	params_Lights.add(params_pointLight);
	params_Lights.add(params_directionalLight);
	params_Lights.add(params_spotLight);

	params_Lights.add(params_Brights);

	params_Lights.add(bSmoothLights);
	params_Lights.add(bDebug);
	params_Lights.add(bDebugShadow);
	params_Lights.add(bDebugLights);

	//-

	// Group all
	//parameters.add(lightParams);
	parameters.add(params_Lights);

	//-

	// Callbacks
	ofAddListener(parameters.parameterChangedE(), this, &SurfingLights::Changed);
	ofAddListener(params_Lights.parameterChangedE(), this, &SurfingLights::ChangedLights);
	ofAddListener(params_Brights.parameterChangedE(), this, &SurfingLights::ChangedBrights);

	// Exclude
	bPointReset.setSerializable(false);
	bSpotReset.setSerializable(false);
	bDirectionalReset.setSerializable(false);
	bPointReset.setSerializable(false);

	//--

	// Startup
	startup();
}

//--------------------------------------------------------------
void SurfingLights::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:startup()";

	//doResetLights();
	//doResetAll();

	//-

	// Settings

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

		doResetAll();
	}
}

//--------------------------------------------------------------
void SurfingLights::updateAnims() {
	//mouseX = ofGetMouseX();
	//mouseY = ofGetMouseY();

	const float ratio = 3.f;
	float sizeScene = 1.f;

	// Anim by time
	if (bAnimLights) {
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

	//{
	//	// move by mouse
	//	spotLight.setOrientation(ofVec3f(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));
	//	spotLight.setPosition(spotLightPosition.get().z, mouseX, mouseY);
	//	//spotLight.setPosition(mouseX, mouseY, spotLightPosition.get().z);
	//}
}

//TODO
//--------------------------------------------------------------
void SurfingLights::drawDebug() {
	// draw debug
	if (bDebugLights) {
		ofPushStyle();

		if (bPoint) {
			ofSetColor(pointLight.getDiffuseColor());
			pointLight.draw();
		}

		if (bSpot) {
			ofSetColor(spotLight.getDiffuseColor());
			spotLight.draw();
		}

		ofPopStyle();
	}
}
//--------------------------------------------------------------
void SurfingLights::begin() {
	//updateAnims();

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

// All Lights
//--------------------------------------------------------------
void SurfingLights::doResetAll() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetAll()";

	doResetLight();
}
//--------------------------------------------------------------
void SurfingLights::doResetLights() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetLights()";

	doResetPoint();
	doResetDirectional();
	doResetSpot();

	refreshBrights();
}

// Point
//--------------------------------------------------------------
void SurfingLights::doResetPoint() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetPoint()";

	//bPoint.set(false);
	pointLightAmbientColor.set(ofFloatColor(1));
	pointLightDiffuseColor.set(ofFloatColor(1));
	pointLightSpecularColor.set(ofFloatColor(1));
	pointLightPosition.set(glm::vec3(0, SURFING__SCENE_SIZE_UNIT / 2,
		SURFING__SCENE_SIZE_UNIT));
	pointBright = 0.2;
}

// Spot
//--------------------------------------------------------------
void SurfingLights::doResetSpot() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetSpot()";

	//bSpot.set(false);
	spotLightCutOff.set(180);
	spotLightConcetration.set(0);
	spotLightAmbientColor.set(ofFloatColor(1));
	spotLightDiffuseColor.set(ofFloatColor(1));
	spotLightSpecularColor.set(ofFloatColor(1));
	spotLightPosition.set(glm::vec3(0, SURFING__SCENE_SIZE_UNIT / 2,
		SURFING__SCENE_SIZE_UNIT / 2));
	spotLightOrientation.set(glm::vec3(0));
	spotBright = 0.05;
}

// Directional
//--------------------------------------------------------------
void SurfingLights::doResetDirectional() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:doResetDirectional()";

	//bDirectional.set(false);
	directionalLightAmbientColor.set(ofFloatColor(1));
	directionalLightDiffuseColor.set(ofFloatColor(1));
	directionalLightSpecularColor.set(ofFloatColor(1));
	directionalLightOrientation.set(glm::vec3(180, 0, 0));
	directionalLightPosition.set(glm::vec3(0,
		SURFING__SCENE_SIZE_UNIT / 4,
		SURFING__SCENE_SIZE_UNIT / 2));
	directionalBright = 0.05;
}

//--------------------------------------------------------------
void SurfingLights::Changed(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Changed" << name << " : " << e;

	if (0) {
	}

	//// Minimize
	//else if (name == bMini_Scene.getName()) {
	//	//setupImGuiStyles();
	//}
}

//--------------------------------------------------------------
void SurfingLights::ChangedLights(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedLights " << name << " : " << e;

	if (0) {
	}

	// Resets
	else if (name == bPointReset.getName() && bPointReset) {
		bPointReset = false;
		doResetPoint();
	} else if (name == bSpotReset.getName() && bSpotReset) {
		bSpotReset = false;
		doResetSpot();
	} else if (name == bDirectionalReset.getName() && bDirectionalReset) {
		bDirectionalReset = false;
		doResetDirectional();
	}

	// Enablers
	else if (name == bPoint.getName()) {
		//setupImGuiStyles();
	} else if (name == bSpot.getName()) {
		//setupImGuiStyles();
	} else if (name == bDirectional.getName()) {
		//setupImGuiStyles();
	}

	// Point Light
	else if (name == pointLightAmbientColor.getName()) {
		pointLight.setAmbientColor(pointLightAmbientColor.get());
	} else if (name == pointLightDiffuseColor.getName()) {
		pointLight.setDiffuseColor(pointLightDiffuseColor.get());
	} else if (name == pointLightSpecularColor.getName()) {
		pointLight.setAmbientColor(pointLightSpecularColor.get());
	} else if (name == pointLightPosition.getName()) {
		pointLight.setPosition(pointLightPosition.get());
	}

	// Spot Light
	else if (name == spotLightAmbientColor.getName()) {
		spotLight.setAmbientColor(spotLightAmbientColor.get());
	} else if (name == spotLightDiffuseColor.getName()) {
		spotLight.setDiffuseColor(spotLightDiffuseColor.get());
	} else if (name == spotLightSpecularColor.getName()) {
		spotLight.setAmbientColor(spotLightSpecularColor.get());
	} else if (name == spotLightPosition.getName()) {
		spotLight.setPosition(spotLightPosition.get());
	} else if (name == spotLightOrientation.getName()) {
		spotLight.setOrientation(spotLightOrientation.get());
	} else if (name == spotLightCutOff.getName()) {
		spotLight.setSpotlightCutOff(spotLightCutOff.get());
	} else if (name == spotLightConcetration.getName()) {
		spotLight.setSpotConcentration(spotLightConcetration.get());
	}

	// Directional Light
	else if (name == directionalLightAmbientColor.getName()) {
		directionalLight.setAmbientColor(directionalLightAmbientColor.get());
	} else if (name == directionalLightDiffuseColor.getName()) {
		directionalLight.setDiffuseColor(directionalLightDiffuseColor.get());
	} else if (name == directionalLightSpecularColor.getName()) {
		directionalLight.setAmbientColor(directionalLightSpecularColor.get());
	} else if (name == directionalLightPosition.getName()) {
		directionalLight.setPosition(directionalLightPosition.get());
	} else if (name == directionalLightOrientation.getName()) {
		directionalLight.setOrientation(directionalLightOrientation.get());
	} else if (name == bSmoothLights.getName()) {
		ofSetSmoothLighting(bSmoothLights);
	}

	//--

	if (name == vResetLight.getName()) {
		doResetAll();
	}
}

//--------------------------------------------------------------
void SurfingLights::ChangedBrights(ofAbstractParameter & e) {
	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingLights:ChangedBrights " << name << " : " << e;

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

//--------------------------------------------------------------
void SurfingLights::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Save: " << pathSettings;

	ofxSurfing::saveSettings(parameters, pathSettings);
}

//--------------------------------------------------------------
void SurfingLights::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:Load: " << pathSettings;

	//#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	//	autoSaver.pause();
	//#endif

	ofxSurfing::loadSettings(parameters, pathSettings);

	//#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	//	autoSaver.start();
	//#endif
}

//--------------------------------------------------------------
void SurfingLights::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingLights:exit()";

	//// Not required to be called bc it's using the auto saver!
	//#if defined(SURFING__USE_AUTOSAVE_FORCE_ON_EXIT) || !defined(SURFING__USE_AUTOSAVE_SETTINGS_ENGINE)
	save();

	//ofxSurfing::saveGroup(params_Lights, pathSettings);
}