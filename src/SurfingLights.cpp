////
////  SurfingLights.cpp
////  practiceEtc
////
//
//#include "SurfingLights.h"
//
////--------------------------------------------------------------
//SurfingLights::SurfingLights() {
//	ofLogNotice("SurfingLights") << "SurfingLights()";
//}
//
////--------------------------------------------------------------
//SurfingLights::~SurfingLights() {
//	ofLogNotice("SurfingLights") << "~SurfingLights()";
//
//	ofRemoveListener(params_Lights.parameterChangedE(), this, &SurfingLights::ChangedLights);
//	ofRemoveListener(paramsColors.parameterChangedE(), this, &SurfingLights::ChangedBg);
//	ofRemoveListener(paramsScene.parameterChangedE(), this, &SurfingLights::ChangedBg);
//	ofRemoveListener(params_Brights.parameterChangedE(), this, &SurfingLights::ChangedBrights);
//
//	ofxSurfing::saveGroup(params_Lights, pathSettings_Lights);
//	ofxSurfing::saveGroup(paramsScene, pathSettings_Bg);
//}
//
////--------------------------------------------------------------
//void SurfingLights::setGuiPosition(glm::vec2 pos) {
//	gui.setPosition(pos.x,pos.y);
//}
//
////--------------------------------------------------------------
//void SurfingLights::drawGui() {
//	gui.draw();
//
//	glm::vec2 p = gui.getShape().getTopRight();
//	p += glm::vec2(SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0);
//	guiUser.setPosition(p.x,p.y);
//	guiUser.draw();
//}
//
////--------------------------------------------------------------
//void SurfingLights::setupGui() {
//	ofLogNotice("SurfingLights") << "setupGui()";
//
//	gui.setup("Lights");
//	gui.add(params);
//
//	guiUser.setup("USER");
//
//	params_User.clear();
//	params_User.setName("User");
//
//	for (int i = 0; i < materials.size(); i++) {
//		params_User.add(materials[i].params_Material);
//	}
//	for (int i = 0; i < materials.size(); i++) {
//		params_User.add(materials[i].bEnable);
//	}
//
//	guiUser.add(params_User);
//
//	gui.minimizeAll();
//	guiUser.minimizeAll();
//}
//
////--------------------------------------------------------------
//void SurfingLights::setup() {
//
//	//--
//
//	//ofSetBoxResolution(128);
//	//ofSetSphereResolution(128);
//
//	pointLight.setPointLight();
//	spotLight.setSpotlight();
//	directionalLight.setDirectional();
//
//	materials.clear();
//
//	// Bg Text
//	ofDisableArbTex();
//	img.load("images/001.jpg");
//
//	//--
//
//	setupParameters();
//}
//
////--------------------------------------------------------------
//void SurfingLights::setupParameters() {
//#define SURFING__PBR__MAX_BRIGHT_LIMIT 0.75f
//
//	params.setName("SurfingLights");
//
//	// Groups
//	params.setName("SurfingLights");
//	params_Lights.setName("Lights");
//	params_pointLight.setName("Point Light");
//	params_spotLight.setName("Spot Light");
//	params_directionalLight.setName("Direct Light");
//	params_Brights.setName("Brights Power");
//
//	// Point
//	bPoint.set("Point", false);
//	bPointReset.set("Reset Point", false);
//	pointBright.set("Bright Point", 0.5, 0, SURFING__PBR__MAX_BRIGHT_LIMIT);
//	pointLightAmbientColor.set("p Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	pointLightDiffuseColor.set("p Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	pointLightSpecularColor.set("p Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	pointLightPosition.set("p Pos", glm::vec3(), glm::vec3(-SURFING__SCENE_SIZE_UNIT), glm::vec3(SURFING__SCENE_SIZE_UNIT));
//
//	// Spot
//	bSpot.set("Spot", false);
//	bSpotReset.set("Reset Spot", false);
//	spotBright.set("Bright Spot", 0.5, 0, SURFING__PBR__MAX_BRIGHT_LIMIT);
//	spotLightAmbientColor.set("s Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	spotLightDiffuseColor.set("s Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	spotLightSpecularColor.set("s Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	spotLightPosition.set("s Pos", glm::vec3(), glm::vec3(-SURFING__SCENE_SIZE_UNIT), glm::vec3(SURFING__SCENE_SIZE_UNIT));
//	spotLightOrientation.set("s Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));
//	spotLightCutOff.set("s CutOff", 1, 0, 90);
//	spotLightConcetration.set("s Conc", 1, 0, 128);
//
//	// Directional
//	bDirectional.set("Direct", false);
//	bDirectionalReset.set("Reset Direct", false);
//	directionalBright.set("Bright Dir", 0.5, 0, SURFING__PBR__MAX_BRIGHT_LIMIT);
//	directionalLightAmbientColor.set("d Ambient", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	directionalLightDiffuseColor.set("d Diffuse", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	directionalLightSpecularColor.set("d Specular", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	directionalLightOrientation.set("d Orientation", glm::vec3(0), glm::vec3(-180), glm::vec3(180));
//	directionalLightPosition.set("d Position", glm::vec3(0), glm::vec3(-SURFING__SCENE_SIZE_UNIT), glm::vec3(SURFING__SCENE_SIZE_UNIT));
//
//	//-
//
//	//// Background Object
//	//Bg_shininess.set("Bg_Shininess", 120, 0, 120);
//	//Bg_color.set("Bg_Color", ofFloatColor(0), ofFloatColor(0), ofFloatColor(1));
//	//Bg_diffuse.set("Bg_Diffuse", ofFloatColor(0.8f, 0.8f, 0.8f), ofFloatColor(0), ofFloatColor(1));
//	//Bg_ambient.set("Bg_Ambient", ofFloatColor(0.2f, 0.2f, 0.2f), ofFloatColor(0), ofFloatColor(1));
//	//Bg_specular.set("Bg_Specular", ofFloatColor(0.f, 0.f, 0.f), ofFloatColor(0), ofFloatColor(1));
//	//Bg_emissive.set("Bg_Emissive", ofFloatColor(0.f, 0.f, 0.f), ofFloatColor(0), ofFloatColor(1));
//	//Bg_Bright.set("Bg_Bright", 0.5f, 0.05f, SURFING__PBR__MAX_BRIGHT_LIMIT); //avoid reset at black..
//
//	// Bright Power
//	params_Brights.add(directionalBright);
//	params_Brights.add(spotBright);
//	params_Brights.add(pointBright);
//	params_Brights.add(Bg_Bright);
//
//	//--
//
////	// Background Scene
////	bAutoSetColor.set("Bg AutoColor", false);
////	//Bg_AutoSetColorPick.set("Bg AutoPick", false);
////	vReset.set("Reset Bg", false);
////	bDrawTex.set("Use Tex", false);
////	bDrawBox.set("Box", false);
////	bDrawSphere.set("Sphere", false);
////
////#ifndef SURFING_CALIBRATE
////	sizeScene.set("Size", 0.5, 0, 1);
////#endif
////#ifdef SURFING_CALIBRATE
////	sizeScene.set("Size", SURFING__SCENE_SIZE_UNIT / 2, 0, 10 * SURFING__SCENE_SIZE_UNIT);
////#endif
//
//	bSmoothLights.set("Smooth Ligths", false); // Default Low Poly
//
//	bMini_Scene.set("Minimize", false);
//
//	//--
//
//	// Clear
//	params_pointLight.clear();
//	params_spotLight.clear();
//	params_directionalLight.clear();
//	params_Lights.clear();
//	//paramsScene.clear();
//	//paramsColors.clear();
//	params.clear();
//
//	//-
//
//	// Lights
//
//	params_pointLight.add(pointLightAmbientColor);
//	params_pointLight.add(pointLightDiffuseColor);
//	params_pointLight.add(pointLightSpecularColor);
//	params_pointLight.add(pointLightPosition);
//	params_pointLight.add(bPointReset);
//
//	params_spotLight.add(spotLightAmbientColor);
//	params_spotLight.add(spotLightDiffuseColor);
//	params_spotLight.add(spotLightSpecularColor);
//	params_spotLight.add(spotLightPosition);
//	params_spotLight.add(spotLightOrientation);
//	params_spotLight.add(spotLightCutOff);
//	params_spotLight.add(spotLightConcetration);
//	params_spotLight.add(bSpotReset);
//
//	params_directionalLight.add(directionalLightAmbientColor);
//	params_directionalLight.add(directionalLightDiffuseColor);
//	params_directionalLight.add(directionalLightSpecularColor);
//	params_directionalLight.add(directionalLightPosition);
//	params_directionalLight.add(directionalLightOrientation);
//	params_directionalLight.add(bDirectionalReset);
//
//	params_Lights.add(bPoint);
//	params_Lights.add(pointBright);
//	params_Lights.add(bSpot);
//	params_Lights.add(spotBright);
//	params_Lights.add(bDirectional);
//	params_Lights.add(directionalBright);
//
//	params_Lights.add(params_pointLight);
//	params_Lights.add(params_spotLight);
//	params_Lights.add(params_directionalLight);
//
//	params_Lights.add(bSmoothLights);
//	params_Lights.add(bMini_Scene); //-> to use callback
//
//	//-
//
//	//// Bg Scene Object
//	//// sphere/box with or (colored) without textured
//
//	//paramsScene.setName("BgScene");
//	//paramsColors.setName("BgColors");
//
//	//paramsScene.add(bDrawSphere);
//	//paramsScene.add(bDrawBox);
//	//paramsScene.add(sizeScene);
//	//paramsScene.add(bDrawTex);
//	//paramsScene.add(bRotate);
//	//paramsScene.add(speedRotate);
//	//paramsScene.add(bInCam);
//	//paramsScene.add(bAutoSetColor);
//	////paramsScene.add(Bg_AutoSetColorPick);//disabled bc break bright..
//	//paramsScene.add(bAnimLights);
//
//	//paramsColors.add(Bg_Bright);
//	//paramsColors.add(Bg_shininess);
//	//paramsColors.add(Bg_color);
//	//paramsColors.add(Bg_diffuse);
//	//paramsColors.add(Bg_ambient);
//	//paramsColors.add(Bg_specular);
//	//paramsColors.add(Bg_emissive);
//	//paramsScene.add(paramsColors);
//
//	//paramsScene.add(vReset);
//
//	// Group all
//	params.add(params_Lights);
//	//params.add(paramsScene);
//
//	//params.add(bMini_Scene);
//
//	//-
//
//	// Callbacks
//	ofAddListener(params_Lights.parameterChangedE(), this, &SurfingLights::ChangedLights);
//	//ofAddListener(paramsColors.parameterChangedE(), this, &SurfingLights::ChangedBg);
//	//ofAddListener(paramsScene.parameterChangedE(), this, &SurfingLights::ChangedBg);
//	ofAddListener(params_Brights.parameterChangedE(), this, &SurfingLights::ChangedBrights);
//
//	// Exclude
//	bPointReset.setSerializable(false);
//	bSpotReset.setSerializable(false);
//	bDirectionalReset.setSerializable(false);
//	bPointReset.setSerializable(false);
//	//vReset.setSerializable(false);
//
//	//--
//
//	// Startup
//	startup();
//}
//
////--------------------------------------------------------------
//void SurfingLights::startup() {
//
//	//doResetLights();
//	//doResetAll();
//
//	////-
//
//	//// Settings
//	//bool bNotFound = true;
//	//bNotFound |= !ofxSurfing::loadGroup(params_Lights, pathSettings_Lights);
//	//bNotFound |= !ofxSurfing::loadGroup(paramsScene, pathSettings_Bg);
//
//	//TODO:
//	// Settings file not found!
//	// Should trig the callbacks to update engine!
//	if (bNotFound) {
//		ofLogError() << __FUNCTION__ << " Settings files not found!";
//		ofLogError() << __FUNCTION__ << " Initializing...";
//
//		bPoint = true;
//		bSpot = true;
//		bDirectional = true;
//
//		doResetAll();
//	}
//}
//
////--------------------------------------------------------------
//void SurfingLights::begin() {
//	//mouseX = ofGetMouseX();
//	//mouseY = ofGetMouseY();
//
//	const float ratio = 3.f;
//
//	// Anim by time
//	if (bAnimLights) {
//		pointLight.setPosition(
//			cos(ofGetElapsedTimef() * .6f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + pointLightPosition.get().x,
//			sin(ofGetElapsedTimef() * .8f) * SURFING__SCENE_SIZE_UNIT * sizeScene + pointLightPosition.get().y,
//			-cos(ofGetElapsedTimef() * .8f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + pointLightPosition.get().z);
//
//		spotLight.setPosition(
//			cos(ofGetElapsedTimef() * .6f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + spotLightPosition.get().x,
//			sin(ofGetElapsedTimef() * .8f) * SURFING__SCENE_SIZE_UNIT * sizeScene + spotLightPosition.get().y,
//			-cos(ofGetElapsedTimef() * .8f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + spotLightPosition.get().z);
//
//		directionalLight.setPosition(
//			cos(ofGetElapsedTimef() * .6f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + directionalLightPosition.get().x,
//			sin(ofGetElapsedTimef() * .8f) * SURFING__SCENE_SIZE_UNIT * sizeScene + directionalLightPosition.get().y,
//			-cos(ofGetElapsedTimef() * .8f) * ratio * SURFING__SCENE_SIZE_UNIT * sizeScene + directionalLightPosition.get().z);
//	}
//	//{
//	//	// move by mouse
//	//	spotLight.setOrientation(ofVec3f(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));
//	//	spotLight.setPosition(spotLightPosition.get().z, mouseX, mouseY);
//	//	//spotLight.setPosition(mouseX, mouseY, spotLightPosition.get().z);
//	//}
//
//	ofEnableLighting();
//
//	//Bg_material.begin();
//	//{
//	//if (bPoint) pointLight.enable();
//	//if (bSpot) spotLight.enable();
//	//if (bDirectional) directionalLight.enable();
//}
//
////--------------------------------------------------------------
//void SurfingLights::end() {
//
//	//if (!bPoint) pointLight.disable();
//	//if (bSpot) spotLight.disable();
//	//if (bDirectional) directionalLight.disable();
//	//}
//	//Bg_material.end();
//
//	ofDisableLighting();
//
//	//-
//
//	if (bDebugLights) {
//
//		// debug
//		ofSetColor(pointLight.getDiffuseColor());
//		if (bPoint) {
//			pointLight.draw();
//		}
//		ofSetColor(spotLight.getDiffuseColor());
//		if (bSpot) spotLight.draw();
//	}
//}
//
////--------------------------------------------------------------
//void SurfingLights::draw() {
//}
//
////--------------------------------------------------------------
//void SurfingLights::drawScene(bool bInCam) {
//	Bg_material.begin();
//
//	float r;
//
//	if (bDrawSphere) {
//#ifndef SURFING_CALIBRATE
//		if (bInCam)
//			r = ofMap(sizeScene, 0, 1, 11, 115, true); // 11, 115
//		else
//			r = ofMap(sizeScene, 0, 1, 900, 2 * 1200, true); // 900, 1200
//#endif
//#ifdef SURFING_CALIBRATE
//		r = sizeScene;
//#endif
//	} else if (bDrawBox) {
//#ifndef SURFING_CALIBRATE
//		if (bInCam)
//			r = ofMap(sizeScene, 0, 1, 11, 70, true); // 11, 50?
//		else
//			r = ofMap(sizeScene, 0, 1, 900, 2 * 1200, true); // 900, 1200
//#endif
//#ifdef SURFING_CALIBRATE
//		r = sizeScene;
//#endif
//	}
//
//	//-
//
//	ofPushStyle();
//	ofPushMatrix();
//	{
//		if (bDrawTex) img.getTexture().bind();
//		{
//			ofSetColor(255, 255, 255, 255);
//
//			if (bDrawSphere) {
//				ofPushMatrix();
//				if (bInCam) {
//					ofTranslate(center.x, center.y, center.z);
//				} else {
//					ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
//				}
//				if (bRotate) ofRotate(-ofGetElapsedTimef() * speedRotate * .2 * RAD_TO_DEG, 0, 1, 0);
//				if (bDrawSphere) ofDrawSphere(0, 0, 0, r); //TODO:offset
//				ofPopMatrix();
//			}
//
//			if (bDrawBox) {
//				ofPushMatrix();
//				if (bInCam) {
//					ofTranslate(center.x, center.y, 0);
//				} else {
//					ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
//				}
//				if (bRotate) ofRotate(-ofGetElapsedTimef() * speedRotate * .2 * RAD_TO_DEG, 0, 1, 0);
//				if (bDrawBox) ofDrawBox(0, 0, 0, 2 * r); //TODO:offset
//				ofPopMatrix();
//			}
//		}
//		if (bDrawTex) img.getTexture().unbind();
//	}
//	ofPopMatrix();
//	ofPopStyle();
//
//	Bg_material.end();
//}
//
////----
//
//// Reset All: Lights + Bg
////--------------------------------------------------------------
//void SurfingLights::doResetAll() {
//	doResetAll();
//	doResetLights();
//}
//
//// Bg
////--------------------------------------------------------------
//void SurfingLights::doResetAll() {
//	ofSetSmoothLighting(bSmoothLights);
//
//	bDrawSphere = true;
//	bDrawTex = false;
//	sizeScene = 0.5f;
//	bRotate = false;
//	speedRotate = 0.5;
//
//	bAutoSetColor = true;
//
//	bAnimLights = false;
//
//	if (materials.size() > 0)
//		if (bAutoSetColor) {
//			auto c = materials[materials.size() - 1].color.get();
//			setBgColorPlain(c);
//		}
//
//	Bg_Bright = 0.15f;
//	//setBrightToColorGroup();
//}
//
//// All Lights
////--------------------------------------------------------------
//void SurfingLights::doResetLights() {
//	doResetPoint();
//	doResetDirectional();
//	doResetSpot();
//
//	refreshBrights();
//}
//
//// Point
////--------------------------------------------------------------
//void SurfingLights::doResetPoint() {
//	//bPoint.set(false);
//	pointLightAmbientColor.set(ofFloatColor(1));
//	pointLightDiffuseColor.set(ofFloatColor(1));
//	pointLightSpecularColor.set(ofFloatColor(1));
//	pointLightPosition.set(glm::vec3(0, SURFING__SCENE_SIZE_UNIT / 2, SURFING__SCENE_SIZE_UNIT));
//	pointBright = 0.2;
//}
//
//// Spot
////--------------------------------------------------------------
//void SurfingLights::doResetSpot() {
//	//bSpot.set(false);
//	spotLightCutOff.set(180);
//	spotLightConcetration.set(0);
//	spotLightAmbientColor.set(ofFloatColor(1));
//	spotLightDiffuseColor.set(ofFloatColor(1));
//	spotLightSpecularColor.set(ofFloatColor(1));
//	spotLightPosition.set(glm::vec3(0, SURFING__SCENE_SIZE_UNIT / 2, SURFING__SCENE_SIZE_UNIT / 2));
//	spotLightOrientation.set(glm::vec3(0));
//	spotBright = 0.05;
//}
//
//// Directional
////--------------------------------------------------------------
//void SurfingLights::doResetDirectional() {
//	//bDirectional.set(false);
//	directionalLightAmbientColor.set(ofFloatColor(1));
//	directionalLightDiffuseColor.set(ofFloatColor(1));
//	directionalLightSpecularColor.set(ofFloatColor(1));
//	directionalLightOrientation.set(glm::vec3(180, 0, 0));
//	directionalLightPosition.set(glm::vec3(0, SURFING__SCENE_SIZE_UNIT / 4, SURFING__SCENE_SIZE_UNIT / 2));
//	directionalBright = 0.05;
//}
//
////--------------------------------------------------------------
//void SurfingLights::ChangedLights(ofAbstractParameter & e) {
//	string name = e.getName();
//
//	ofLogNotice() << __FUNCTION__ << " " << name << " : " << e;
//
//	if (0) {
//	}
//
//	// Minimize
//	else if (name == bMini_Scene.getName()) {
//		//setupImGuiStyles();
//	}
//
//	// Resets
//	else if (name == bPointReset.getName() && bPointReset) {
//		bPointReset = false;
//		doResetPoint();
//	} else if (name == bSpotReset.getName() && bSpotReset) {
//		bSpotReset = false;
//		doResetSpot();
//	} else if (name == bDirectionalReset.getName() && bDirectionalReset) {
//		bDirectionalReset = false;
//		doResetDirectional();
//	}
//
//	// Enablers
//	else if (name == bPoint.getName()) {
//		//setupImGuiStyles();
//	} else if (name == bSpot.getName()) {
//		//setupImGuiStyles();
//	} else if (name == bDirectional.getName()) {
//		//setupImGuiStyles();
//	}
//
//	// Point Light
//	else if (name == pointLightAmbientColor.getName()) {
//		pointLight.setAmbientColor(pointLightAmbientColor.get());
//	} else if (name == pointLightDiffuseColor.getName()) {
//		pointLight.setDiffuseColor(pointLightDiffuseColor.get());
//	} else if (name == pointLightSpecularColor.getName()) {
//		pointLight.setAmbientColor(pointLightSpecularColor.get());
//	} else if (name == pointLightPosition.getName()) {
//		pointLight.setPosition(pointLightPosition.get());
//	}
//
//	// Spot Light
//	else if (name == spotLightAmbientColor.getName()) {
//		spotLight.setAmbientColor(spotLightAmbientColor.get());
//	} else if (name == spotLightDiffuseColor.getName()) {
//		spotLight.setDiffuseColor(spotLightDiffuseColor.get());
//	} else if (name == spotLightSpecularColor.getName()) {
//		spotLight.setAmbientColor(spotLightSpecularColor.get());
//	} else if (name == spotLightPosition.getName()) {
//		spotLight.setPosition(spotLightPosition.get());
//	} else if (name == spotLightOrientation.getName()) {
//		spotLight.setOrientation(spotLightOrientation.get());
//	} else if (name == spotLightCutOff.getName()) {
//		spotLight.setSpotlightCutOff(spotLightCutOff.get());
//	} else if (name == spotLightConcetration.getName()) {
//		spotLight.setSpotConcentration(spotLightConcetration.get());
//	}
//
//	// Directional Light
//	else if (name == directionalLightAmbientColor.getName()) {
//		directionalLight.setAmbientColor(directionalLightAmbientColor.get());
//	} else if (name == directionalLightDiffuseColor.getName()) {
//		directionalLight.setDiffuseColor(directionalLightDiffuseColor.get());
//	} else if (name == directionalLightSpecularColor.getName()) {
//		directionalLight.setAmbientColor(directionalLightSpecularColor.get());
//	} else if (name == directionalLightPosition.getName()) {
//		directionalLight.setPosition(directionalLightPosition.get());
//	} else if (name == directionalLightOrientation.getName()) {
//		directionalLight.setOrientation(directionalLightOrientation.get());
//	} else if (name == bSmoothLights.getName()) {
//		ofSetSmoothLighting(bSmoothLights);
//	}
//}
//
////--------------------------------------------------------------
//void SurfingLights::ChangedBrights(ofAbstractParameter & e) {
//	string name = e.getName();
//
//	ofLogNotice() << __FUNCTION__ << " " << name << " : " << e;
//
//	if (0) {
//	}
//
//	else if (name == pointBright.getName()) {
//		refreshBrightPoint();
//	} else if (name == directionalBright.getName()) {
//		refreshBrightDirect();
//	} else if (name == spotBright.getName()) {
//		refreshBrightSpot();
//	}
//
//	else if (name == Bg_Bright.getName()) {
//		setBrightToColorGroup();
//	}
//}
//
////--------------------------------------------------------------
//void SurfingLights::ChangedBg(ofAbstractParameter & e) {
//	string name = e.getName();
//
//	ofLogNotice() << __FUNCTION__ << " " << name << " : " << e;
//
//	if (0) {
//	}
//
//	//disabled bc break bright..
//	//else if (name == Bg_AutoSetColorPick.getName())
//	//{
//	//	if (Bg_AutoSetColorPick) bAutoSetColor = false;
//	//	//bAutoSetColor = !Bg_AutoSetColorPick;
//	//}
//	//else if (name == bAutoSetColor.getName())
//	//{
//	//	if (bAutoSetColor) Bg_AutoSetColorPick = false;
//	//	//Bg_AutoSetColorPick = !bAutoSetColor;
//	//}
//
//	else if (name == bDrawBox.getName()) {
//		bDrawSphere = !bDrawBox;
//	} else if (name == bDrawSphere.getName()) {
//		bDrawBox = !bDrawSphere;
//	}
//
//	else if (name == Bg_color.getName()) {
//		setColorBgGroup(Bg_color.get());
//	}
//
//	else if (name == Bg_shininess.getName()) {
//		Bg_material.setShininess(Bg_shininess);
//	} else if (name == Bg_diffuse.getName()) {
//		Bg_material.setDiffuseColor(Bg_diffuse.get());
//	} else if (name == Bg_ambient.getName()) {
//		Bg_material.setAmbientColor(Bg_ambient.get());
//	} else if (name == Bg_specular.getName()) {
//		Bg_material.setSpecularColor(Bg_specular.get());
//	} else if (name == Bg_emissive.getName()) {
//		Bg_material.setEmissiveColor(Bg_emissive.get());
//	}
//
//	else if (name == vReset.getName() && vReset) {
//		vReset = false;
//		doResetAll();
//	}
//
//	else if (name == bRotate.getName()) {
//		//setupImGuiStyles();
//	}
//}
//
///*
//
////--------------------------------------------------------------
//void SurfingLights::setupImGuiStyles() {
//
//	ui.clearStyles();
//
//	// Bg Bright
//	//ui.AddStyle(Bg_Bright, OFX_IM_HSLIDER_NO_LABELS);
//	ui.AddStyle(Bg_Bright, OFX_IM_HSLIDER_SMALL_NO_LABELS);
//
//	// Lights
//	// Reset
//	ui.AddStyle(bPointReset, OFX_IM_BUTTON_SMALL);
//	ui.AddStyle(bSpotReset, OFX_IM_BUTTON_SMALL);
//	ui.AddStyle(bDirectionalReset, OFX_IM_BUTTON_SMALL);
//
//	// Mini
//	if (bMini_Scene) {
//		//ui.AddStyleGroup(paramsColors, OFX_IM_GROUP_HIDDEN, ImGuiTreeNodeFlags_CollapsingHeader);
//
//		ui.AddStyle(bDrawSphere, OFX_IM_HIDDEN);
//		ui.AddStyle(bDrawBox, OFX_IM_HIDDEN);
//		ui.AddStyle(bDrawTex, OFX_IM_HIDDEN);
//		ui.AddStyle(bRotate, OFX_IM_HIDDEN);
//		ui.AddStyle(bInCam, OFX_IM_HIDDEN);
//		ui.AddStyle(bAnimLights, OFX_IM_HIDDEN);
//		ui.AddStyle(vReset, OFX_IM_HIDDEN);
//
//		// Bg
//		ui.AddStyle(Bg_color, OFX_IM_COLOR_INPUTS_NO_ALPHA);
//		//ui.AddStyle(Bg_color, OFX_IM_COLOR_BOX_FULL_WIDTH);
//		ui.AddStyle(Bg_shininess, OFX_IM_HIDDEN);
//		ui.AddStyle(Bg_diffuse, OFX_IM_HIDDEN);
//		ui.AddStyle(Bg_specular, OFX_IM_HIDDEN);
//		ui.AddStyle(Bg_ambient, OFX_IM_HIDDEN);
//		ui.AddStyle(Bg_emissive, OFX_IM_HIDDEN);
//
//		ui.AddStyle(bAutoSetColor, OFX_IM_HIDDEN);
//		//ui.AddStyle(Bg_AutoSetColorPick, OFX_IM_HIDDEN);
//		ui.AddStyle(sizeScene, OFX_IM_HIDDEN);
//	}
//	// No Mini
//	else {
//		//ui.AddStyleGroup(paramsColors, OFX_IM_GROUP_COLLAPSED, ImGuiTreeNodeFlags_CollapsingHeader);
//
//		ui.AddStyle(bDrawSphere, OFX_IM_TOGGLE_SMALL);
//		ui.AddStyle(bDrawBox, OFX_IM_TOGGLE_SMALL);
//		ui.AddStyle(bDrawTex, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//		ui.AddStyle(bRotate, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//		ui.AddStyle(bInCam, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//		ui.AddStyle(bAnimLights, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//		ui.AddStyle(vReset, OFX_IM_BUTTON_SMALL);
//
//		// Bg
//		ui.AddStyle(Bg_color, OFX_IM_COLOR_INPUTS_NO_ALPHA);
//		ui.AddStyle(Bg_shininess, OFX_IM_COLOR_INPUTS_NO_ALPHA);
//		ui.AddStyle(Bg_diffuse, OFX_IM_COLOR_INPUTS_NO_ALPHA);
//		ui.AddStyle(Bg_specular, OFX_IM_COLOR_INPUTS_NO_ALPHA);
//		ui.AddStyle(Bg_ambient, OFX_IM_COLOR_INPUTS_NO_ALPHA);
//		ui.AddStyle(Bg_emissive, OFX_IM_COLOR_INPUTS_NO_ALPHA);
//
//		//ui.AddStyle(Bg_AutoSetColorPick, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//		ui.AddStyle(bAutoSetColor, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//		ui.AddStyle(sizeScene, OFX_IM_SLIDER);
//	}
//
//	if (bRotate) {
//		ui.AddStyle(speedRotate, OFX_IM_DEFAULT);
//	} else {
//		ui.AddStyle(speedRotate, OFX_IM_HIDDEN);
//	}
//
//	//-
//
//	// Groups
//
//	ui.AddStyleGroup(params_Lights, OFX_IM_GROUP_COLLAPSED, ImGuiTreeNodeFlags_None);
//
//	ui.AddStyleGroup(params_pointLight, OFX_IM_GROUP_COLLAPSED, ImGuiTreeNodeFlags_None);
//	ui.AddStyleGroup(params_spotLight, OFX_IM_GROUP_COLLAPSED, ImGuiTreeNodeFlags_None);
//	ui.AddStyleGroup(params_directionalLight, OFX_IM_GROUP_COLLAPSED, ImGuiTreeNodeFlags_None);
//
//	ui.AddStyleGroup(paramsScene, OFX_IM_GROUP_COLLAPSED, ImGuiTreeNodeFlags_DefaultOpen);
//	ui.AddStyleGroup(paramsColors, OFX_IM_GROUP_COLLAPSED, ImGuiTreeNodeFlags_DefaultOpen);
//}
//
////--------------------------------------------------------------
//void SurfingLights::drawImGuiMaterials() {
//
//	if (ui.BeginWindow("Materials", NULL, ui.bAutoResize ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None)) {
//		//TODO: fixing groups nestings..
//		//ui.reset();
//
//		for (int i = 0; i < materials.size(); i++) {
//			ui.AddGroup(materials[i].params_Material, OFX_IM_GROUP_DEFAULT);
//		}
//
//		ui.EndWindow();
//	}
//}
//
////--------------------------------------------------------------
//void SurfingLights::drawImGuiParts() {
//
//	if (ui.BeginWindow(bGui_Parts)) {
//		for (int i = 0; i < materials.size(); i++) {
//			ui.Add(materials[i].bEnable, OFX_IM_TOGGLE_BIG);
//		}
//
//		ui.EndWindow();
//	}
//}
//
////--------------------------------------------------------------
//void SurfingLights::drawImGuiScene() {
//
//	if (ui.BeginWindow(bGui)) {
//		float _w1 = ofxImGuiSurfing::getWidgetsWidth(1);
//		float _w2 = ofxImGuiSurfing::getWidgetsWidth(2);
//		float _h = ofxImGuiSurfing::getWidgetsHeightUnit();
//
//		//-
//
//		// Mini
//		ui.Add(bMini_Scene, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//
//		//--
//
//		// Sections
//
//		ui.Add(bGui_SubLights, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
//		ui.Add(bGui_SubBg, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
//
//		// 1. Sub Lights
//
//		if (bGui_SubLights) {
//			ui.AddSeparator();
//
//			bool bOpen = true;
//			ImGuiColorEditFlags _flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);
//			if (ImGui::CollapsingHeader("Lights", _flagw)) {
//				ImGui::Indent();
//				{
//					ui.refreshLayout(); //-> required due to above indent
//					_w1 = ofxImGuiSurfing::getWidgetsWidth(1);
//					_w2 = ofxImGuiSurfing::getWidgetsWidth(2);
//					_h = ofxImGuiSurfing::getWidgetsHeightUnit();
//
//					//-
//
//					// Main Enablers Controls
//					ui.Add(bPoint, OFX_IM_TOGGLE_BIG);
//					ui.Add(bSpot, OFX_IM_TOGGLE_BIG);
//					ui.Add(bDirectional, OFX_IM_TOGGLE_BIG);
//
//					ui.AddSeparator();
//
//					// Bright Controls v sliders
//					if (bPoint) ui.Add(pointBright, OFX_IM_VSLIDER_NO_LABELS, true, 3);
//					if (bSpot) ui.Add(spotBright, OFX_IM_VSLIDER_NO_LABELS, true, 3);
//					if (bDirectional) ui.Add(directionalBright, OFX_IM_VSLIDER_NO_LABELS, false, 3);
//
//					//-
//
//					// v Sliders
//
//					if (!bMini_Scene) {
//						ui.AddSeparator();
//
//						if (bPoint) {
//							//-> TODO: bug when chaining multiple groups on a window.
//							ui.AddGroup(params_pointLight);
//						}
//						if (bSpot) {
//
//							ui.AddGroup(params_spotLight);
//						}
//						if (bDirectional) {
//
//							ui.AddGroup(params_directionalLight);
//						}
//					}
//
//					if (!bMini_Scene)
//						if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_Bullet)) {
//							if (ImGui::Button("Reset Lights", ImVec2(_w1, _h))) {
//								doResetLights();
//							}
//
//							if (ImGui::Button("Save Lights", ImVec2(_w1, _h))) {
//								ofxSurfingHelpers::saveGroup(params_Lights, pathSettings_Lights);
//							}
//
//							if (ImGui::Button("Load Lights", ImVec2(_w1, _h))) {
//								ofxSurfingHelpers::loadGroup(params_Lights, pathSettings_Lights);
//							}
//						}
//				}
//				ImGui::Unindent();
//			}
//		}
//
//		//--
//
//		// 2. Sub Bg
//
//		if (bGui_SubBg) {
//			ui.AddSeparator();
//
//			bool bOpen = true;
//			ImGuiColorEditFlags _flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);
//			if (ImGui::CollapsingHeader("Background", _flagw)) {
//				ImGui::Indent();
//
//				_w1 = ofxImGuiSurfing::getWidgetsWidth(1);
//				_w2 = ofxImGuiSurfing::getWidgetsWidth(2);
//				_h = ofxImGuiSurfing::getWidgetsHeightUnit();
//
//				//-
//
//				ui.AddGroup(paramsScene);
//
//				if (!bMini_Scene) {
//					if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_Bullet)) {
//						if (ImGui::Button("Reset Bg", ImVec2(_w1, _h))) {
//							doResetAll();
//						}
//
//						if (ImGui::Button("Save Bg", ImVec2(_w1, _h))) {
//							ofxSurfingHelpers::saveGroup(paramsScene, pathSettings_Bg);
//						}
//
//						//ImGui::SameLine();
//
//						if (ImGui::Button("Load Bg", ImVec2(_w1, _h))) {
//							ofxSurfingHelpers::loadGroup(paramsScene, pathSettings_Bg);
//						}
//					}
//				}
//
//				ImGui::Unindent();
//			}
//		}
//
//		//--
//
//		ui.AddSeparator();
//
//		// Extra
//
//		if (!bMini_Scene) {
//			ui.Add(ui.bExtra, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//			if (ui.bExtra) {
//				ImGui::Indent();
//				ui.Add(ui.bAutoResize, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//				ui.Add(bSmoothLights, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
//				ofxImGuiSurfing::ToggleRoundedButton("Debug", &bDebugLights);
//				ImGui::Unindent();
//			}
//		}
//
//		if (ImGui::Button("Reset All", ImVec2(_w1, _h))) {
//			doResetAll();
//		}
//
//		//--
//
//		ui.EndWindow();
//	}
//}
//
//*/