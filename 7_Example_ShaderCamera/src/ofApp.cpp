#include "ofApp.h"
#include "ofGLProgrammableRenderer.h"

//--------------------------------------------------------------
void ofApp::setup() {

#if 1
	ofxSurfing::setOfxGuiTheme();
	//ofxSurfing::setWindowTitleAsProjectName();
	//ofxSurfing::setWindowAtMonitor(-1);
#endif

	ofSetLogLevel(OF_LOG_VERBOSE);

	//--

	// shadows and lights work with materials
	// create some different materials so the lighting and shadows can be applied

	setupLights();
	setupObjects();
	setupMaterials();

	setupShader();

	//--

	setupGui();
}

//--------------------------------------------------------------
void ofApp::setupShader() {

#if (USE_PAO_CAM == 1)
	camWidth = 640;
	camHeight = 480;
#else
	camWidth = 1920;
	camHeight = 1080;
#endif

	ofDisableArbTex();
	string pathShaderExtra = "shadersExtraGL3/shader";
	shader.load(pathShaderExtra);
	img.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
	plane.set(camWidth, camHeight, camWidth * 0.1, camHeight * 0.1);
	plane.mapTexCoordsFromTexture(img.getTexture());

	videoGrabber.setVerbose(true);
	videoGrabber.listDevices();

	videoGrabber.setDeviceID(indexCam);

	videoGrabber.setup(camWidth, camHeight);

	parametersPlaneShader.setName("Custom PlaneShader");
	parametersPlaneShader.add(indexMode.set("MODE", 1, 0, 1));
	parametersPlaneShader.add(bDrawPlaneShader.set("Draw PlaneShader", true));
	parametersPlaneShader.add(noiseAmplitude.set("Displacement", 0.5f, 0.0f, 1.0f));
	parametersPlaneShader.add(noiseScale.set("Noise Scale", 0.05f, 0.0f, 0.1f));
	parametersPlaneShader.add(noiseSpeed.set("Noise Speed", 0.5f, 0.0f, 1.0f));
	parametersPlaneShader.add(vRandomNoise.set("RandomNoise"));
	parametersPlaneShader.add(vRestartCamera.set("Restart Camera"));
	parametersPlaneShader.add(offsetPlaneHeight.set("Offest Height", 0.0f, -1.0f, 1.0f));
	parameters.add(parametersPlaneShader);

	ofAddListener(parametersPlaneShader.parameterChangedE(), this, &ofApp::ChangedParametersPlaneShader);
}
//--------------------------------------------------------------
void ofApp::setupObjects() {
	boxMesh = ofMesh::box(1, 1, 1, 24, 24, 24);

	logoMesh.load("models\\ofLogoHollow.ply");
	logoMesh.mergeDuplicateVertices();
	// we need to flip the normals for this mesh //
	for (size_t i = 0; i < logoMesh.getNumNormals(); i++) {
		logoMesh.getNormals()[i] *= -1.f;
	}
}

//--------------------------------------------------------------
void ofApp::setupLights() {
	// add two lights
	int numLights = 2;
	for (int i = 0; i < numLights; i++) {
		auto light = std::make_shared<ofLight>();
		light->enable();
		if (i == 0) {
			light->setPointLight();
			//light->setDirectional();
			//light->setSpotlight(60, 20);
			//light->getShadow().setNearClip(200);
			//light->getShadow().setFarClip(2000);
			//light->setPosition( 210, 330.0, 750 );
			//light->setAmbientColor(ofFloatColor(0.4));
		} else {
			// set the light to be a spot light with a cutoff (cone/fov) of 30 degrees
			// and a concentration (softness) of 50 out of a range 0 - 128
			light->setSpotlight(60, 20);
			light->getShadow().setNearClip(200);
			light->getShadow().setFarClip(2000);
			light->setPosition(210, 330.0, 750);
			light->setAmbientColor(ofFloatColor(0.4));
		}
		// set the strength of the shadow, how visible it is
		// default is 0.5
		light->getShadow().setStrength(0.6f);

		if (light->getType() != OF_LIGHT_POINT) {
			glm::quat xq = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1, 0, 0));
			glm::quat yq = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
			light->setOrientation(yq * xq);
		}

		if (!ofIsGLProgrammableRenderer()) {
			light->enable();
		}

		lights.push_back(light);
	}

	//--

	// shadows are disabled by default
	// call this function to enable all of them that are attached to lights
	ofShadow::enableAllShadows();
	// shadow bias is the margin of error the shadow depth
	// increasing the bias helps reduce shadow acne, but can cause light leaking
	// try to find a good balance that fits your needs
	// bias default is 0.005
	ofShadow::setAllShadowBias(0.007);
	// normal bias default is 0
	// moves the bias along the normal of the mesh, helps reduce shadow acne
	ofShadow::setAllShadowNormalBias(-4.f);
#ifndef TARGET_OPENGLES
	ofShadow::setAllShadowDepthResolutions(1024, 1024);
#endif
	shadowType = OF_SHADOW_TYPE_PCF_LOW;
	ofShadow::setAllShadowTypes(shadowType);
	// point light shadow depth maps require OF_PRIMITIVE_TRIANGLES
	// so we grab some meshes here for drawing and make sure they use OF_PRIMITIVE_TRIANGLES
	// we also use a higher resolution for the meshes than the default
}

//--------------------------------------------------------------
void ofApp::setupMaterials() {

	bDrawBoxes.set("Boxes", true);
	bDrawLogo.set("Logo", false);
	vResetMaterials.set("Reset Materials");

	parameters.setName("ofApp");
	parameters.add(bDrawLogo);
	parameters.add(bDrawBoxes);
	parameters.add(vResetMaterials);

	ofAddListener(parameters.parameterChangedE(), this, &ofApp::Changed);

	//--

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	// required to handle independent settings
	// and use the internal gui panels independently for each instance.
	refreshGuiAnchor();
	logoMaterial.setup(bDrawLogo.getName());
	boxesMaterial.setup(bDrawBoxes.getName());
#endif

	//--

	// json settings
	ofLogNotice() << "Loading App Settings... not found.";

	if (!load()) {
		ofLogNotice() << "App Settings not found.";

		resetMaterials(); //reset scene/materials if there's no settings file.
	} else
		ofLogNotice() << "Loaded app Settings successfuly";
}

//--------------------------------------------------------------
void ofApp::setupGui() {

	gui.setup(parameters);

	//--

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	// required to handle independent settings
	// and use the internal gui panels independently for each instance.
	logoMaterial.setup(bDrawLogo.getName());
	boxesMaterial.setup(bDrawBoxes.getName());

	refreshGuiAnchor();
#endif
}

//--------------------------------------------------------------
void ofApp::resetMaterials() {

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	//TODO: Why reset scene looks darker??

	// Refresh the addon to mirror/reflect
	// the same OF internal PBR settings!
	logoMaterial.doResetMaterial();
	boxesMaterial.doResetMaterial();
#else
	// Materials from the OF core example
	logoMaterial = ofMaterial();
	logoMaterial = ofMaterial();

	boxesMaterial.setDiffuseColor(ofFloatColor(0.25));
	boxesMaterial.setShininess(60);
	boxesMaterial.setSpecularColor(ofFloatColor(1));

	logoMaterial.setAmbientColor(ofFloatColor(0.85, 0.16, 0.43) * 0.8);
	logoMaterial.setDiffuseColor(ofFloatColor(0.85, 0.16, 0.43, 1.0));
	logoMaterial.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0, 1.0));
	logoMaterial.setShininess(50);
#endif
}

//--------------------------------------------------------------
void ofApp::update() {
	ofxSurfing::setWindowTitleAsProjectNameWithFPS();

	//FIX workaround to collapse all sub menus.. not working..
#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	static bool bDone = false;
	if (!bDone && (ofGetFrameNum() > 600)) {
		bDone = true;
		logoMaterial.refreshGui();
		boxesMaterial.refreshGui();
		ofLogNotice() << "Forced insternal refreshGuis";
	}
#endif

	//--

	float deltaTime = ofClamp(ofGetLastFrameTime(), 1.f / 5000.f, 1.f / 5.f);
	float etimef = ofGetElapsedTimef();

	if (lights.size() > 0) {
		if (lights[0]->getType() == OF_LIGHT_POINT) {
			float tangle = etimef * 1.05;
			lights[0]->setPosition(-120, sinf(tangle) * 180.f + 150, cosf(tangle) * 100.f);
		}
	}

	colorHue += deltaTime * 2.0;
	if (colorHue >= 255) {
		colorHue = 0.f;
	}
	lightColor.setHue(colorHue);

	if (lights.size() > 0) {
		auto & light = lights[0];
		lightColor.setHsb(ofWrap(colorHue, 0, 255.f), 210, 230);
		// ambient color applies all over regardless of being lit or not
		light->setAmbientColor(lightColor * 0.2);
		light->setDiffuseColor(lightColor);
		lightColor.setBrightness(250);
		light->setSpecularColor(lightColor);
	}

	//--

	updateShader();
}

//--------------------------------------------------------------
void ofApp::updateShader() {

	//PAO BEGIN

	videoGrabber.update();
	ofPixelsRef pixelsRef = videoGrabber.getPixels();

	ofFloatPixels & pixels = img.getPixels();

	float m = noiseAmplitude / 255.0f;
	float n = noiseSpeed * ofGetElapsedTimef();

	int w = img.getWidth();
	int h = img.getHeight();

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int i = y * w + x;

			// shader modes

			if (indexMode == 0) { // feeded by noise
				pixels[i] = noiseAmplitude * ofNoise(x * noiseScale, y * noiseScale, n);
			}

			else if (indexMode == 1) { // feeded by cam image
				pixels[i] = m * pixelsRef.getColor(x, y).getLightness();
			}

			else if (indexMode == 2) { // TODO
			}
		}
	}

	img.update();

	// PAO END
}

//--------------------------------------------------------------
void ofApp::draw() {
	drawScene();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawScene() {

	ofEnableDepthTest();

	// Compute lights
	for (int i = 0; i < lights.size(); i++) {
		auto & light = lights[i];
		// query the light to see if it has a depth pass
		if (light->shouldRenderShadowDepthPass()) {
			// Get the number of passes required.
			// By default the number of passes is 1. And we could just call beginShadowDepthPass() or beginShadowDepthPass(0);
			// It will be more than one pass if it is a pointlight with setSingleOmniPass set to false
			// or a platform that does not support geometry shaders.
			// Most likely it will be a single pass, but we get the number of passes to be safe.
			int numShadowPasses = light->getNumShadowDepthPasses();
			for (int j = 0; j < numShadowPasses; j++) {
				light->beginShadowDepthPass(j);
				// Shadows have the following gl culling enabled by default
				// this helps reduce z fighting by only rendering the rear facing triangles to the depth map
				// enables face culling
				//glEnable(GL_CULL_FACE);
				// sets the gl triangle winding order, default for ofShadow is GL_CW
				//glFrontFace(mGlFrontFaceWindingOrder);
				// tells OpenGL to cull front faces
				//glCullFace(GL_FRONT);

				// the culling can be disabled by calling
				//light->getShadow().setGlCullingEnabled(false);
				// or the culling winding order can be changed by calling
				//light->getShadow().setFrontFaceWindingOrder(GL_CCW); // default is GL_CW
				renderScene();
				light->endShadowDepthPass(j);
			}
		}
	}

	//--

	camera.begin();
	{
		if (!ofIsGLProgrammableRenderer()) {
			ofEnableLighting();
		}

		// CULL the back faces of the geometry for rendering
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		{
			renderScene();
		}
		glDisable(GL_CULL_FACE);

		if (!ofIsGLProgrammableRenderer()) {
			ofDisableLighting();
		}

		//--

		// debug lights
		ofPushStyle();
		for (int i = 0; i < lights.size(); i++) {
			auto & light = lights[i];

			ofSetColor(light->getDiffuseColor());
			if (light->getType() == OF_LIGHT_POINT) {
				ofDrawSphere(light->getPosition(), 12);
			} else {
				light->draw();
			}
			if (light->getIsEnabled() && light->getShadow().getIsEnabled() && bDrawFrustums) {
				light->getShadow().drawFrustum();
			}
		}
		ofPopStyle();
	}
	camera.end();

	//--

	// Camera image preview

	if (indexMode == 0) {
	}

	else if (indexMode == 1) {
		ofRectangle rect;
		int w = 320;
		int h = w * (9.f / 16.f);
		int x = ofGetWidth() - w;
		int y = 0;
		rect = ofRectangle(x, y, w, h);
#if 0
		img.draw(rect);
#else
		videoGrabber.draw(rect);
#endif
	}

	else if (indexMode == 2) {
	}
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	ofDisableDepthTest();

	buildHelp();
	ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);

	gui.draw();

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
	refreshGuiLinks();
	if (bDrawLogo) logoMaterial.drawGui();
	if (bDrawBoxes) boxesMaterial.drawGui();
#endif
}

// create a renderScene() function so the same drawing can happen in both ofApp::draw()
// and inside the ofLight::beginShadowDepthPass()
//--------------------------------------------------------------
void ofApp::renderScene() {
	ofPushStyle();

	float etimef = ofGetElapsedTimef();

	if (bDrawBoxes) {
		boxesMaterial.begin();
		{
			ofPushMatrix();
			{
				ofTranslate(250, cosf(etimef * 0.6) * 50 - 80, 200);
				ofRotateZDeg(ofWrapDegrees((etimef * 0.04) * 360));
				ofRotateXDeg(ofWrapDegrees((etimef * 0.06) * 360));
				ofScale(100, 100, 100.0);
				boxMesh.draw();
			}
			ofPopMatrix();

			ofPushMatrix();
			{
				ofTranslate(-250, cosf(etimef * 0.4) * 90 - 30, 200);
				ofRotateZDeg(ofWrapDegrees((etimef * 0.034) * 360));
				ofRotateXDeg(ofWrapDegrees((etimef * 0.067) * 360));
				ofScale(100, 100, 100.0);
				boxMesh.draw();
			}
			ofPopMatrix();
		}
		boxesMaterial.end();
	}

	if (bDrawLogo) {
		logoMaterial.begin();
		{
			ofPushMatrix();
			ofTranslate(-50, -170, 50);
			ofRotateXDeg(-90);
			ofScale(60, 60, 60);
			logoMesh.draw();
			ofPopMatrix();
		}
		logoMaterial.end();
	}

	ofPopStyle();

	//--

	// NOTE: it seems that this will fail bc this shader 
	// is chained inside the main PBR ones...
	if (bDrawPlaneShader) {
		ofPushMatrix();
		{
			ofTranslate(0, offsetPlaneHeight * SURFING__PBR__SCENE_SIZE_UNIT, 0);

			//--
			
			// here starts the custom shader,
			// but it could be replaced
			// by that one from ofMaterial
			//boxesMaterial.begin();

			shader.begin();
			{
				shader.setUniformTexture("displacement", img.getTexture(), 1);
				ofPushMatrix();
				ofRotateXDeg(-90);
				ofTranslate(0, 0, -200);
				plane.draw();
				ofDrawIcoSphere(0, 0, 0, 100);
				ofPopMatrix();
			}
			shader.end();

			//boxesMaterial.end();
		}
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::Changed(ofAbstractParameter & e) {

	std::string n = e.getName();

	ofLogNotice() << "Changed: " << n << ": " << e;

	if (n == vResetMaterials.getName()) {
		resetMaterials();
	}
}

//--------------------------------------------------------------
void ofApp::ChangedParametersPlaneShader(ofAbstractParameter & e) {

	std::string n = e.getName();

	ofLogNotice() << "Changed: " << n << ": " << e;

	if (n == vRandomNoise.getName()) {
		noiseAmplitude = ofRandom(noiseAmplitude.getMin(), noiseAmplitude.getMax());
		noiseScale = ofRandom(noiseScale.getMin(), noiseScale.getMax());
		noiseSpeed = ofRandom(noiseSpeed.getMin(), noiseSpeed.getMax());
	}
	if (n == vRestartCamera.getName()) {
		videoGrabber.close();
		videoGrabber.listDevices();
		videoGrabber.setDeviceID(indexCam);
		videoGrabber.setup(camWidth, camHeight);
	}
}

//--------------------------------------------------------------
bool ofApp::load() {
	return ofxSurfing::loadSettings(parameters);
}

//--------------------------------------------------------------
void ofApp::save() {
	ofxSurfing::saveSettings(parameters);
}

//--------------------------------------------------------------
void ofApp::buildHelp() {

	stringstream ss;
	ss << "HELP        ";
	ss << ofxSurfing::getProjectName() << endl
	   << endl;
	ss << "BACKSPACE   Reset Materials" << endl;
	if (!ofIsGLProgrammableRenderer()) {
		ss << endl
		   << "SHADOWS ONLY WORK WITH PROGRAMMABLE RENDERER!" << endl;
	} else if (!ofShadow::areShadowsSupported()) {
		ss << endl
		   << "SHADOWS NOT SUPPORTED ON THIS PLATFORM!" << endl;
	} else {
		ss << "SPACEBAR    Shadows " << (bEnableShadows ? "ON " : "OFF");
		ss << endl
		   << "F           Draw Frustums " << (bDrawFrustums ? "ON " : "OFF");
		ss << endl
		   << "LEFT/RIGHT  Shadow Type " << ofToUpper(ofShadow::getShadowTypeAsString(shadowType));
		ss << endl
		   << "UP/DOWN     Sample Radius " << shadowSampleRadius;
	}

	sHelp = ss.str();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == ' ') {
		bEnableShadows = !bEnableShadows;
		if (bEnableShadows) {
			ofShadow::enableAllShadows();
		} else {
			ofShadow::disableAllShadows();
		}
	}
	if (key == 'f' || key == 'F') {
		bDrawFrustums = !bDrawFrustums;
	}
	if (key == OF_KEY_RIGHT) {
		int stype = (int)shadowType + 1;
		if (stype == OF_SHADOW_TYPE_TOTAL) {
			stype = 0;
		}
		shadowType = (ofShadowType)stype;
		ofShadow::setAllShadowTypes(shadowType);
	}
	if (key == OF_KEY_LEFT) {
		int stype = (int)shadowType - 1;
		if (stype == OF_SHADOW_TYPE_HARD - 1) {
			stype = OF_SHADOW_TYPE_PCF_HIGH;
		}
		shadowType = (ofShadowType)stype;
		ofShadow::setAllShadowTypes(shadowType);
	}
	if (key == OF_KEY_UP) {
		shadowSampleRadius += 1.f;
		ofShadow::setAllShadowSampleRadius(shadowSampleRadius);
	}
	if (key == OF_KEY_DOWN) {
		shadowSampleRadius -= 1.f;
		if (shadowSampleRadius < 0) {
			shadowSampleRadius = 0.f;
		}
		ofShadow::setAllShadowSampleRadius(shadowSampleRadius);
	}
	if (key == OF_KEY_BACKSPACE) {
		resetMaterials();
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofRemoveListener(parameters.parameterChangedE(), this, &ofApp::Changed);
	ofRemoveListener(parametersPlaneShader.parameterChangedE(), this, &ofApp::ChangedParametersPlaneShader);

	save();
}

#ifndef SURFING__USE__OF_CORE_PBR_MATERIALS
//--------------------------------------------------------------
void ofApp::refreshGuiAnchor() {
	//set anchor gui (ofxPanel gui) positioned
	ofxSurfing::setGuiPositionToLayout(gui, ofxSurfing::SURFING_LAYOUT_TOP_LEFT);

	//minimize/collapse ofxPanel folders
	boxesMaterial.gui.minimizeAll();
	logoMaterial.gui.minimizeAll();
}
//--------------------------------------------------------------
void ofApp::refreshGuiLinks() {
	if (bDrawLogo && bDrawBoxes) {
		ofxSurfing::setGuiPositionRightTo(logoMaterial.gui, gui);
		ofxSurfing::setGuiPositionRightTo(boxesMaterial.gui, logoMaterial.gui);
	} else if (!bDrawLogo && bDrawBoxes) {
		ofxSurfing::setGuiPositionRightTo(boxesMaterial.gui, gui);
	} else if (bDrawLogo && !bDrawBoxes) {
		ofxSurfing::setGuiPositionRightTo(logoMaterial.gui, gui);
	}
}
#endif