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
void ofxSurfingPBR::doResetLights() {
	x = 0;
	y = SURFING__SZ_UNIT / 2.f;
	z = 0;
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetPlane() {
	planeSz.set(glm::vec2(0.8f, 0.8f));
	planeRot.set(-45.f);
	planePos.set(0);
	planeDiffuse.set(0.6);
	planeShiness.set(0.85);
	planeSpecular.set(1);
}

//--------------------------------------------------------------
void ofxSurfingPBR::doResetShadow() {
	bEnableShadow.set(true);
	bDebug.set(false);
	shadowBias.set(0.07);
	shadowNormalBias.set(-4.f);
}

//--------------------------------------------------------------
void ofxSurfingPBR::setup() {

	parameters.setName("PBR_Scene");

	planeParams.setName("Floor Plane");
	planeTransform.setName("Transform");
	bDrawPlane.set("Plane Draw", true);
	planeSz.set("Size", glm::vec2(0.8f, 0.8f), glm::vec2(0, 0), glm::vec2(1, 1));
	planeRot.set("x Rotation", 0, -180, 180);
	planePos.set("y Position", 0, -1, 1);
	planeDiffuse.set("Diffuse", 0.6, 0, 1);
	planeShiness.set("Shiness", 0.85, 0, 1);
	planeSpecular.set("Specular", 1, 0, 1);
	planeParams.add(bDrawPlane);
	planeParams.add(wireframe.set("Wireframe", true));
	planeTransform.add(planeSz);
	planeTransform.add(planeRot);
	planeTransform.add(planePos);
	planeParams.add(planeTransform);

	planeParams.add(planeDiffuse);
	planeParams.add(planeShiness);
	planeParams.add(planeSpecular);
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
	lightParams.add(x.set("X", 0.0f, -1 * u, 1 * u));
	lightParams.add(y.set("Y", 0.0f, -1 * u, 1 * u));
	lightParams.add(z.set("Z", 0.0f, -1 * u, 1 * u));
	lightParams.add(resetLights);
	parameters.add(lightParams);

	shadowParams.setName("Shadow");
	shadowParams.add(bEnableShadow.set("Enable", true));
	shadowParams.add(bDebug.set("Debug", false));
	shadowParams.add(shadowBias.set("Bias", 0.07, 0.0, 1.0));
	shadowParams.add(shadowNormalBias.set("Normal Bias", -4.f, -10.0, 10.0));
	shadowParams.add(resetShadow);
	parameters.add(shadowParams);

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

	// lights
	for (int i = 0; i < 1; i++) {
		auto light = make_shared<ofLight>();
		light->enable();
		if (i == 0) {
			light->setPointLight();
			light->getShadow().setNearClip(20);
			light->getShadow().setFarClip(500);
		} else {
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

	// object
	material.setup();

	//--

	// reset
	doResetPlane();
	doResetLights();
	doResetShadow();

	setupGui();
}

//--------------------------------------------------------------
void ofxSurfingPBR::setupGui() {
	gui.setup(parameters);
	gui.loadFromFile(path);
}

//--------------------------------------------------------------
void ofxSurfingPBR::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void ofxSurfingPBR::update() {
	for (auto & light : lights) {
		light->getShadow().setEnabled(bEnableShadow);
	}

	if (lights.size() > 0) {
		if (lights[0]->getType() == OF_LIGHT_POINT) {
			float tangle = ofGetElapsedTimef() * 1.05;
			lights[0]->setPosition(x, y, z);
			//lights[0]->setAmbientColor(ofColor::red);
		}
	}

	ofShadow::setAllShadowBias(shadowBias);
	ofShadow::setAllShadowNormalBias(shadowNormalBias);
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
				if(bDebug) light->getShadow().drawFrustum();
			}
		}
	}
	camera->end();
}

//--------------------------------------------------------------
void ofxSurfingPBR::Changed(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice(__FUNCTION__) << name << " : " << e;

	if (name == planeSz.getName()) {
		float u = 10.f * SURFING__SZ_UNIT;
		//plane.set(u * planeSz.get().x, u * planeSz.get().y,
		//	(u * planeSz.get().x) / 10.f, (u * planeSz.get().y) / 10.f);
		plane.set(u * planeSz.get().x, u * planeSz.get().y, 10.f, 10.f);
	}

	else if (name == planeRot.getName()) {
		glm::vec3 axis(1.0f, 0.0f, 0.0f);
		float angle = planeRot.get();
		glm::quat q = glm::angleAxis(glm::radians(angle), axis);
		plane.setOrientation(q);
	}

	else if (name == planePos.getName()) {
		plane.setPosition(0, planePos.get() * SURFING__SZ_UNIT, 0);
	}

	else if (name == planeDiffuse.getName()) {
		materialPlane.setDiffuseColor(ofFloatColor(planeDiffuse.get()));
	}

	else if (name == planeShiness.getName()) {
		materialPlane.setShininess(planeShiness.get() * 100);
	}

	else if (name == planeSpecular.getName()) {
		materialPlane.setSpecularColor(ofFloatColor(planeSpecular.get()));
	}

	else if (name == resetPlane.getName()) {
		doResetPlane();
	}

	else if (name == resetLights.getName()) {
		doResetLights();
	}

	else if (name == resetShadow.getName()) {
		doResetShadow();
	}
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