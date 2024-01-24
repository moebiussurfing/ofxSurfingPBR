#include "SurfingFloor.h"

//--------------------------------------------------------------
SurfingFloor::SurfingFloor() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:SurfingFloor() Constructor";

	ofAddListener(ofEvents().windowResized, this, &SurfingFloor::windowResized);
	ofAddListener(ofEvents().update, this, &SurfingFloor::update);
}

//--------------------------------------------------------------
SurfingFloor::~SurfingFloor() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:~SurfingFloor() Destructor";

	ofRemoveListener(ofEvents().windowResized, this, &SurfingFloor::windowResized);
	ofRemoveListener(ofEvents().update, this, &SurfingFloor::update);

	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingFloor::Changed);

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	ofRemoveListener(displacersParams.parameterChangedE(), this, &SurfingFloor::ChangedDisplacers);
#endif

	//--

#ifdef SURFING__PBR__USE_AUTO_CALL_EXIT_ON_DESTRUCTOR_IF_REQUIRED
	if (!bDoneExit) {
		ofLogWarning("SurfingFloor") << "Force calling exit()!";
		ofLogWarning("SurfingFloor") << "That's bc has not been called until now!";

		exit();
	}
#endif
}
//--------------------------------------------------------------
void SurfingFloor::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:setupParams()";

	//bKeys.set("Keys", true);
	//bDebug.set("Debug", false);

	bGui.set("UI FLOOR", false);

	//--

	bDraw.set("Draw Floor", true);
	bDrawPlane.set("Draw Mode Plane", true);
	bDrawBox.set("Draw Mode Box", false);
	bDrawWireframe.set("Draw Floor Wireframe", false);

	//--

	parameters.setName("FLOOR");
	paramsTransform.setName("Transform");

	//--

	vResetFloor.set("Reset Floor");
	vResetFloorTransform.set("Reset Transform");

	//--

	bInfinite.set("Infinite", true);
	bSquaredSize.set("Squared", false);
	size.set("Size", glm::vec2(0.5f, 0.5f), glm::vec2(0, 0), glm::vec2(1.f, 1.f));
	resolution.set("Resolution", glm::vec2(0.5f, 0.5f), glm::vec2(0, 0), glm::vec2(1.f, 1.f));
	rotation.set("x Rotation", 0, -45, 135);
	position.set("y Position", 0, -1, 1);
	floorBoxDepth.set("Box Depth", 0, 1, 100);

	//--

	paramsDraw.setName("DRAW");
	paramsDraw.add(bDraw);
	paramsDraw.add(bDrawPlane);
	paramsDraw.add(bDrawBox);
	paramsDraw.add(bDrawWireframe);
	//--

	material.setup("MATERIAL FLOOR");

	paramsTransform.add(position);
	paramsTransform.add(rotation);
	paramsTransform.add(size);
	paramsTransform.add(bSquaredSize);
	paramsTransform.add(bInfinite);
	paramsTransform.add(floorBoxDepth);
	paramsTransform.add(resolution);
	paramsTransform.add(vResetFloorTransform);

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	setupParamsDisplace();
#endif

	//--

	parameters.add(material.bGui);
	parameters.add(paramsDraw);
	parameters.add(paramsTransform);
	parameters.add(vResetFloor);
	//paramsAdvanced.add(material.getIndexStateParam());//to include and store history index

	//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	setupShaderPlane();
#endif

	//--

	// Callbacks
	ofAddListener(parameters.parameterChangedE(), this, &SurfingFloor::Changed);

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	ofAddListener(displacersParams.parameterChangedE(), this, &SurfingFloor::ChangedDisplacers);
#endif

	//--

	bDoneSetupParams = true;
}

//--------------------------------------------------------------
bool SurfingFloor::isVisibleDebugShader() {
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDebug && (bShaderToPlane || bDisplaceToMaterial))
		return true;
	else
		return false;
#else
	return false;
#endif
}

//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS

//--------------------------------------------------------------
void SurfingFloor::setupShaderPlane() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:setupShaderPlane()";

	ofDisableArbTex();

	bLoadedShaderPlane = shaderPlane.load("shadersGL3/shader");

	refreshImgShaderPlane();

	doResetNoise();
	doResetDisplace();
}

//--------------------------------------------------------------
void SurfingFloor::setupParamsDisplace() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:setupParamsDisplace()";

	vResetDisplace.set("Reset Displace");
	vResetNoise.set("Reset Noise");

	bShaderToPlane.set("ShaderToPlane", false);
	bDisplaceToMaterial.set("DispToMaterial", false);
	bLimitImage.set("Limit IMG", true);

	noiseParams.setName("Noise Generator");
	noiseParams.add(noiseAmplitude.set("Noise Amp D", 0.0f, 0.0f, 2.f));
	noiseParams.add(noiseScale.set("Noise Scale", 0.0f, 0.0f, 0.1f));
	noiseParams.add(noiseSpeed.set("Noise Speed", 0.0f, 0.0f, 1.0f));
	noiseParams.add(vResetNoise);

	displaceMaterialParams.setName("Material Disp");
	displaceMaterialParams.add(displacementStrength.set("D Strength", 0.0, 0.0, 500.0));
	displaceMaterialParams.add(displacementNormalsStrength.set("D Normals Strength", 0.0, 0.0, 1.0));
	displaceMaterialParams.add(normalGeomToNormalMapMix.set("NGeom NMapMix", 0.0, 0.0, 1.0));
	displaceMaterialParams.add(vResetDisplace);

	displacersParams.setName("Displacers");
	displacersParams.add(bLimitImage);
	displacersParams.add(bDisplaceToMaterial);
	displacersParams.add(bShaderToPlane);
	displacersParams.add(noiseParams);
	displacersParams.add(displaceMaterialParams);

	parameters.add(displacersParams);
}

//--------------------------------------------------------------
void SurfingFloor::doResetNoise() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:doResetNoise()";

	noiseAmplitude.set(1.f);
	noiseScale.set(0.05f);
	noiseSpeed.set(0.5f);
}

//--------------------------------------------------------------
void SurfingFloor::doResetDisplace() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:doResetDisplace()";

	// reduce callbacks amount a bit not updating params if not required..

	float displacementStrength_ = 100;
	//float displacementStrength_ = displacementStrength.getMax() * 0.75;
	float displacementNormalsStrength_ = displacementNormalsStrength.getMax() / 2;
	float normalGeomToNormalMapMix_ = normalGeomToNormalMapMix.getMax() / 2;

	if (displacementStrength.get() != displacementStrength_)
		displacementStrength.set(displacementStrength_);

	if (displacementNormalsStrength.get() != displacementNormalsStrength_)
		displacementNormalsStrength.set(displacementNormalsStrength_);

	if (normalGeomToNormalMapMix.get() != normalGeomToNormalMapMix_)
		normalGeomToNormalMapMix.set(normalGeomToNormalMapMix_);

	//if (!bLimitImage) bLimitImage.set(true);
	//if (bShaderToPlane) bShaderToPlane.set(false);
	//if (bDisplaceToMaterial) bDisplaceToMaterial.set(false);
}

//--------------------------------------------------------------
void SurfingFloor::updateDisplace() {

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
		material.material.setDisplacementTexture(img.getTexture());

		material.material.setDisplacementStrength(displacementStrength);
		material.material.setDisplacementNormalsStrength(displacementNormalsStrength);
		material.material.setNormalGeomToNormalMapMix(normalGeomToNormalMapMix);
		//material.material.setTexCoordScale(scaleX, scaleY);
	}
}

//--------------------------------------------------------------
void SurfingFloor::beginShaderPlane() {
	shaderPlane.begin();
	shaderPlane.setUniformTexture("displacement", img.getTexture(), 1);
}
//--------------------------------------------------------------
void SurfingFloor::endShaderPlane() {
	shaderPlane.end();
}
#endif

//--------------------------------------------------------------
void SurfingFloor::setup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:setup() Start";

	setupParams();

	//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	setupShaderPlane();
#endif

	//--

	// reset all except material
	// to avoid do it twice and overwrite loaded settings!
	//doResetAll(true);

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	callback_t f = std::bind(&SurfingFloor::save, this);
	autoSaver.setFunctionSaver(f);
#endif

	//--

	setupGui();

	//--

	bDoneSetup = true;
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:setup() Done!";

	//--

	startup();
}

//--------------------------------------------------------------
ofRectangle SurfingFloor::getGuiShape() const {
	ofRectangle r1 = gui.getShape();
	ofRectangle r2 = material.gui.getShape();
	ofRectangle bb = r1.getUnion(r2);
	return bb;
}

//--------------------------------------------------------------
void SurfingFloor::setColorGlobal(ofFloatColor color) {
	material.setColorGlobal(color);
}

//--------------------------------------------------------------
void SurfingFloor::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:startup() Begins!";

	bool b = load();

	if (!b) {
		ofLogWarning("SurfingFloor") << "No file settings found!";
		ofLogWarning("SurfingFloor") << "Initialize settings for the first time!";
		ofLogWarning("SurfingFloor") << "Potential Newbie User Found!";

		//--

		// Force enable Help and Debug states!
		// Ready to a newbie user!
		// All important/learning controls will be visible.
		ofLogWarning("SurfingFloor") << "Forcing states and some default stuff visible:";
		ofLogWarning("SurfingFloor") << "Enable help, debug, reset camera and settings, etc...";

		bGui = true;
		bDebug = true;
	}

	else {
		ofLogNotice("ofxSurfingPBR") << "SurfingFloor:Located scene settings!";
		ofLogNotice("ofxSurfingPBR") << "SurfingFloor:App has been opened before. Not the first time!";
	}

	//--

	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:startup() Done! at frame number: " << ofGetFrameNum();
	bDoneStartup = true;
}
//--------------------------------------------------------------
void SurfingFloor::setupGui() {

	gui.setup(parameters);

	refreshGui();

	// assign to ofxGui icons
	eSaveOfxGui = gui.savePressedE.newListener([this] {
		save();
	});
	eLoadOfxGui = gui.loadPressedE.newListener([this] {
		load();
	});
}

//--------------------------------------------------------------
void SurfingFloor::refreshGui() {

	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:refreshGui()";

	// minimize

	gui.getGroup(paramsDraw.getName())
		.minimize();

	gui.getGroup(paramsTransform.getName())
		.minimize();

	gui.getGroup(paramsTransform.getName())
		.getGroup(size.getName())
		.minimize();

	gui.getGroup(paramsTransform.getName())
		.getGroup(resolution.getName())
		.minimize();

	material.refreshGui();

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	gui.getGroup(displacersParams.getName())
		.minimize();

	gui.getGroup(displacersParams.getName())
		.getGroup(displaceMaterialParams.getName())
		.minimize();

	gui.getGroup(displacersParams.getName())
		.getGroup(noiseParams.getName())
		.minimize();
#endif
}

//--------------------------------------------------------------
void SurfingFloor::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void SurfingFloor::update() {

	if (bFlagRefresPlane) {
		bFlagRefresPlane = false;

		refreshPlane();
	}

	if (bFlagRefreshBox) {
		bFlagRefreshBox = false;

		refreshBox();
	}
}

//--------------------------------------------------------------
void SurfingFloor::updateScene() {

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDisplaceToMaterial || bShaderToPlane) {
		updateDisplace();
	}
#endif
}

//--------------------------------------------------------------
void SurfingFloor::beginMaterial() {
	material.begin();
}
//--------------------------------------------------------------
void SurfingFloor::endMaterial() {
	material.end();
}

//--------------------------------------------------------------
void SurfingFloor::drawGui() {

	if (bGui) {
		ofxSurfing::setGuiPositionBelowTo(material.gui, gui);
		material.drawGui();
	}
}

//--------------------------------------------------------------
void SurfingFloor::refreshPlane() {

	if (bSquaredSize) {
		float x = size.get().x;
		float y = size.get().y;
		if (y != x) {
			size.set(glm::vec2(x, x));
		}
	}

	int w, h;

	if (bInfinite) {
		// size hardcoded to a safety max!
		w = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
		h = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
	} else {
		// size from normalized param multiplied by a unit
		int planeSizeUnit = SURFING__PBR__SCENE_SIZE_UNIT * SURFING__PBR__PLANE_SIZE_MULTIPLIER;
		w = size.get().x * planeSizeUnit;
		h = size.get().y * planeSizeUnit;
	}

	//--

	int xResolution;
	int yResolution;

	xResolution = (int)ofMap(resolution.get().x, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	yResolution = (int)ofMap(resolution.get().y, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	floorPlane.set(w, h, xResolution, yResolution);

	//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	if (bDisplaceToMaterial || bShaderToPlane)
		refreshImgShaderPlane();
#endif
}

//--------------------------------------------------------------
void SurfingFloor::refreshBox() {

	if (bSquaredSize) {
		float x = size.get().x;
		float y = size.get().y;
		if (y != x) {
			size.set(glm::vec2(x, x));
		}
	}

	int w, h;

	if (bInfinite) {
		// size hardcoded to a safety max!
		w = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
		h = SURFING__PBR__PLANE_SIZE_INFINITE_MODE;
	} else {
		// size from normalized param multiplied by a unit
		int planeSizeUnit = SURFING__PBR__SCENE_SIZE_UNIT * SURFING__PBR__PLANE_SIZE_MULTIPLIER;
		w = size.get().x * planeSizeUnit;
		h = size.get().y * planeSizeUnit;
	}

	//--

	int xResolution;
	int yResolution;

	xResolution = (int)ofMap(resolution.get().x, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	yResolution = (int)ofMap(resolution.get().y, 0.f, 1.f,
		(int)SURFING__PBR__PLANE_RESOLUTION_MIN, (int)SURFING__PBR__PLANE_RESOLUTION_MAX, true);

	floorBox.set(w, h, floorBoxDepth, xResolution, yResolution, 1);
}

//--------------------------------------------------------------
void SurfingFloor::Changed(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:Changed " << name << ": " << e;

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	if (0) {
	}

	else if (name == bDrawPlane.getName()) {
		if (bDrawPlane)
			if (bDrawBox) bDrawBox = false;
	}

	else if (name == bDrawBox.getName()) {
		if (bDrawBox)
			if (bDrawPlane) bDrawPlane = false;
	}

	else if (name == bSquaredSize.getName()) {
		if (bSquaredSize) {
			bFlagRefresPlane = true;
			bFlagRefreshBox = true;
		}
	}

	else if (name == size.getName()) {
		static glm::vec2 floorSize_ = glm::vec2(-1, -1);
		if (size.get() != floorSize_) { // if changed

			floorSize_ = size.get();

			bFlagRefresPlane = true;
			bFlagRefreshBox = true;
		} else {
			ofLogVerbose("SurfingFloor") << "Floor Plane size not Changed. Skipped refresh!";
		}
	}

	else if (name == resolution.getName()) {
		bFlagRefresPlane = true;
		bFlagRefreshBox = true;
	}

	else if (name == bInfinite.getName()) {
		bFlagRefresPlane = true;
		bFlagRefreshBox = true;
	}

	else if (name == floorBoxDepth.getName()) {
		bFlagRefreshBox = true;
	}

	else if (name == rotation.getName()) {
		glm::vec3 axis(1.0f, 0.0f, 0.0f);
		float angle = rotation.get() - 90;
		glm::quat q = glm::angleAxis(glm::radians(angle), axis);

		floorPlane.setOrientation(q);
		floorBox.setOrientation(q);
	}

	else if (name == position.getName()) {
		floorPlane.setPosition(0, position.get() * SURFING__PBR__SCENE_SIZE_UNIT * 5.f, 0);
		floorBox.setPosition(0, position.get() * SURFING__PBR__SCENE_SIZE_UNIT * 5.f, 0);
	}

	else if (name == vResetFloor.getName()) {
		doResetFloor();
	}

	else if (name == vResetFloorTransform.getName()) {
		doResetFloorTransform();
	}
}

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void SurfingFloor::ChangedDisplacers(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:ChangedDisplacers " << name << ": " << e;

	#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
	#endif

	//--

	if (name == vResetDisplace.getName()) {
		doResetDisplace();
	} else if (name == vResetNoise.getName()) {
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
			material.material.setDisplacementStrength(0);
			material.material.setDisplacementNormalsStrength(0);
			material.material.setNormalGeomToNormalMapMix(0);
		}
	}

	else if (name == bLimitImage.getName()) {
		//refreshPlane();
		bFlagRefresPlane = true;
	}
}
#endif

//--------------------------------------------------------------
void SurfingFloor::draw() {
	if (!bDraw) return;

	drawPlaneFloor();
	drawBoxFloor();
}

//--------------------------------------------------------------
void SurfingFloor::drawPlaneFloor() {
	if (!bDrawPlane) return;

	// plane
	if (bDrawWireframe) {
		floorPlane.drawWireframe();
	}

	//else
	{
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
		if (bShaderToPlane)
			beginShaderPlane();
		else
			beginMaterial();

		{
			floorPlane.draw();
		}

		if (bShaderToPlane)
			endShaderPlane();
		else
			endMaterial();
#else
		beginMaterial();
		{
			floorPlane.draw();
		}
		endMaterial();
#endif
	}
}

//--------------------------------------------------------------
void SurfingFloor::drawBoxFloor() {
	if (!bDrawBox) return;

	if (bDrawWireframe) {
		floorBox.drawWireframe();
	}

	//else
	{
		beginMaterial();
		{
			floorBox.draw();
		}
		endMaterial();
	}
}

//--------------------------------------------------------------
void SurfingFloor::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:exit()";

#ifndef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	// Should not mandatory as settings should be internally auto saved when changing.

	// Not required to be called bc it's using the auto saver!
	save();

	material.exit();
#endif

	bDoneExit = true;
}

//--------------------------------------------------------------
void SurfingFloor::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:save() " << path;

	//save scene
	ofxSurfing::saveSettings(parameters, path);
}

//--------------------------------------------------------------
bool SurfingFloor::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:load() " << path;

	bool b;

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	//--

	b = ofxSurfing::loadSettings(parameters, path);

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	return b;
}

//--------------------------------------------------------------
void SurfingFloor::doResetFloor() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:doResetFloor()";

	doResetFloorTransform();

	material.doResetMaterial();

	bDrawWireframe = false;

	bDrawBox = true;

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	// displacers
	if (!bLimitImage) bLimitImage.set(true);
	if (bShaderToPlane) bShaderToPlane.set(false);
	if (bDisplaceToMaterial) bDisplaceToMaterial.set(false);
	if (!bDrawPlane) bDrawPlane = true;
#endif
}

//--------------------------------------------------------------
void SurfingFloor::doResetFloorTransform() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:doResetFloorTransform()";

	size.set(glm::vec2(0.5, 0.2));
	resolution.set(glm::vec2(0.01f, 0.01f));
	position.set(0.f);
	rotation.set(0.f);

	floorBoxDepth = 5;

	bSquaredSize = false;
	bInfinite = false;
}

//--------------------------------------------------------------
void SurfingFloor::doResetAll() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:doResetAll()";

	bDebug = false;

	// plane
	doResetFloor();

	// shader displacer
#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	doResetNoise();
	doResetDisplace();
#endif

	// material
	material.doResetMaterial();
}

//--

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
//--------------------------------------------------------------
void SurfingFloor::refreshImgShaderPlane() {
	ofLogNotice("ofxSurfingPBR") << "SurfingFloor:refreshImgShaderPlane()";

	int w = floorPlane.getWidth();
	int h = floorPlane.getHeight();

	if (bLimitImage) {
		w = ofClamp(w, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MIN);
		h = ofClamp(h, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MIN);
	} else {
		w = ofClamp(w, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MAX);
		h = ofClamp(h, 0, SURFING__PBR__SHADER_LIMIT_IMAGE_MAX);
	}

	{

	#ifdef TARGET_OPENGLES
		OPENGL ES supports GL_RGBA32F but not GL_R32F
			img.allocate(80, 60, OF_IMAGE_COLOR_ALPHA);
	#else
		//img.clear();

		img.allocate(w, h, OF_IMAGE_GRAYSCALE);

		ofLogNotice("ofxSurfingPBR") << "SurfingFloor:Allocated img: " << w << "," << h;
	#endif

		//TODO: seems breaking some plane colors/materials props..
		//apply to plane
		floorPlane.mapTexCoordsFromTexture(img.getTexture());

		ofLogNotice("ofxSurfingPBR") << "SurfingFloor:mapTexCoordsFromTexture(img.getTexture()";
	}
}

#endif

//--------------------------------------------------------------
void SurfingFloor::keyPressed(int key) {
	if (!bKeys) return;

	ofLogNotice("ofxSurfingPBR") << "keyPressed(" << key << ")";

	material.keyPressed(key);

	if (key == 'p') bDrawPlane = !bDrawPlane;
	if (key == 'B') bDrawBox = !bDrawBox;
	if (key == 'w') bDrawWireframe = !bDrawWireframe;
	if (key == 'i') bInfinite = !bInfinite;
}

//--------------------------------------------------------------
void SurfingFloor::windowResized(ofResizeEventArgs & resize) {
	glm::vec2 sz { resize.width, resize.height };
	static glm::vec2 sz_ { -1, -1 };
	if (sz != sz_) {
		sz_ = sz;
		ofLogNotice("ofxSurfingPBR") << "windowResized() Size: " << sz_.x << "," << sz_.y;

		refreshGui();
	}
}

//--------------------------------------------------------------
void SurfingFloor::drawDebug() {

#ifdef SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS
	// Shader and displacer info
	if (bShaderToPlane || bDisplaceToMaterial) {
		// layout is responsive

		int x, y, w, h;

		float r = img.getHeight() / (float)img.getWidth();

		// info
		string s = "";
		s += " \n";
		s += "DEBUG\nSHADER\n\n";
		s += "FLOOR PLANE\n";
		s += "Size: ";
		s += ofToString(floorPlane.getWidth(), 0) + "x" + ofToString(floorPlane.getHeight(), 0) + "\n";
		s += "Resolution: ";
		s += ofToString(floorPlane.getResolution().x) + "x" + ofToString(floorPlane.getResolution().y) + "\n\n";
		s += "IMAGE\n";
		s += "Size: ";
		s += ofToString(img.getWidth()) + "x" + ofToString(img.getHeight()) + "\n\n";
		s += "WINDOW\n";
		s += ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight()) + " px\n";
		s += ofToString(ofGetFrameRate(), 1) + " FPS";

		ofxSurfing::SURFING_LAYOUT l;
		l = ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;

		//TODO
		// Set the preview box position related to help box layout.

		if (helpLayout == ofxSurfing::SURFING_LAYOUT_TOP_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_TOP_CENTER)
			l = ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_TOP_RIGHT)
			l = ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT;

		if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_TOP_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_CENTER_RIGHT)
			l = ofxSurfing::SURFING_LAYOUT_CENTER_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_CENTER_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_CENTER_RIGHT;

		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT)
			l = ofxSurfing::SURFING_LAYOUT_TOP_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER)
			l = ofxSurfing::SURFING_LAYOUT_TOP_CENTER;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT)
			l = ofxSurfing::SURFING_LAYOUT_TOP_RIGHT;

		ofxSurfing::ofDrawBitmapStringBox(s, l);

		auto bb = ofxSurfing::getBBBitmapStringBoxToLayout(s, l);

		//image preview
		w = bb.getWidth();
		h = w * r;
		x = bb.getTopLeft().x;

		//fix to avoid empty pixels on the union..
		float round = (float)SURFING__STRING_BOX__DEFAULT_ROUND;
		bool isRound = round > 0;

		// set image pos above text box
		y = bb.getTopLeft().y - h;

		//move image below the text box
		if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_LEFT)
			y = bb.getBottomLeft().y; //TOP_LEFT;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_CENTER)
			y = bb.getBottomLeft().y; //TOP_CENTER;
		else if (helpLayout == ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT)
			y = bb.getBottomLeft().y; //TOP_RIGHT;

		img.draw(x, y, w, h);

		// border rect line
		x += 1;
		w -= 2;
		ofRectangle rbb = ofRectangle(x, y, w, h);
		ofPushStyle();
		ofNoFill();
		ofSetColor(0, 255);
		ofDrawRectangle(rbb);
		ofPopStyle();
	}
#endif
}