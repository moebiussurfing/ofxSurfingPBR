#include "SurfingMaterial.h"

//--------------------------------------------------------------
SurfingMaterial::SurfingMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:constructor() " << name;

	ofAddListener(ofEvents().update, this, &SurfingMaterial::update);
}

//--------------------------------------------------------------
SurfingMaterial::~SurfingMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:destructor() " << name;

	ofRemoveListener(ofEvents().update, this, &SurfingMaterial::update);
	ofRemoveListener(settingsParams.parameterChangedE(), this, &SurfingMaterial::ChangedSettings);
	ofRemoveListener(colorParams.parameterChangedE(), this, &SurfingMaterial::ChangedColors);
	ofRemoveListener(helpersParams.parameterChangedE(), this, &SurfingMaterial::ChangedHelpers);
	ofRemoveListener(globalParams.parameterChangedE(), this, &SurfingMaterial::ChangedGlobals);

#ifdef SURFING__PBR__USE_AUTO_CALL_EXIT_ON_DESTRUCTOR_IF_REQUIRED
	if (!bDoneExit) {
		ofLogWarning("ofxSurfingPBR") << "SurfingMaterial:destructor() Force calling exit() bc has not been called until now!";

		exit();
	}
#endif
}

//--------------------------------------------------------------
void SurfingMaterial::setup(string name) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setup(" << name << ")";
	this->setName(name);
	setup();
}

//--------------------------------------------------------------
void SurfingMaterial::setup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setup()";

	setupParams();

	doResetMaterial();

	setupGui();

	load();

	bDoneSetup = true;
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setup() Done!";
}

//--------------------------------------------------------------
void SurfingMaterial::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:SurfingMaterial:setupParams()";

	//--

	// This is required when using multiple material instances:

	string nameParams;

	if (name == "") { // default workflow. when using one single instance
		path = pathRoot + ext;

		nameParams = "PBR_MATERIAL";
	}

	else { // to allow multiple instances. to be used for gui naming and settings path!
		path = pathRoot + "_" + name + ext;

		pathHistory = pathHistory + name;

		nameParams = name;
	}

	//--

	parameters.setName(nameParams);

	colorParams.setName("Colors");
	globalParams.setName("Globals");
	globalLinksParams.setName("Link");
	settingsParams.setName("Settings");
	coatParams.setName("Coat");
	randomizersParams.setName("Randomizers");
	helpersParams.setName("Helpers");
	moreParams.setName("More");

	//--

	globalColor.setSerializable(false);
	globalAlpha.setSerializable(false);

	moreParams.setSerializable(false);

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	// auto saver
	callback_t f = std::bind(&SurfingMaterial::save, this);
	autoSaver.setFunctionSaver(f);
#endif

	//--

	settingsParams.add(shininess.set("Shininess", 0.0, 0.0, SURFING__PBR__MAX_SHININESS));
	settingsParams.add(roughness.set("Roughness", 0.0, 0.0, 1.0));
	settingsParams.add(metallic.set("Metallic", 0.0, 0.0, 1.0));
	settingsParams.add(reflectance.set("Reflectance", 0.0, 0.0, 1.0));

	colorParams.add(ambientColor.set("Ambient", ofFloatColor::white));
	colorParams.add(specularColor.set("Specular", ofFloatColor::white));
	colorParams.add(diffuseColor.set("Diffuse", ofFloatColor::white));
	colorParams.add(emissiveColor.set("Emissive", ofFloatColor::white));

	coatParams.add(bClearCoat.set("Clear Coat", false));
	coatParams.add(clearCoatRoughness.set("CC Roughness", 0.0001, 0.0001, 10.0));
	coatParams.add(clearCoatStrength.set("CC Strength", 0.0001, 0.0001, 10.0));

	vResetMaterial.set("Mat Reset");
	vRandomMaterialFull.set("Mat Rand Full");
	vRandomSettings.set("Mat Rand Settings");

	//--

	randomizersParams.add(vResetMaterial);
	randomizersParams.add(vRandomMaterialFull);
	randomizersParams.add(vRandomSettings);
	randomizersParams.add(vRandomColorsGlobal.set("Random Global Color"));
	randomizersParams.add(vRandomColors.set("Random Colors"));
	randomizersParams.add(vRandomColorsAlpha.set("Random ColorsAlpha"));
	randomizersParams.add(vRandomAlphas.set("Random Alphas"));
	helpersParams.add(randomizersParams);

	//--

	//TODO
	// History undo/redo
	setupHistoryManager();

	//--

	globalParams.add(globalColor.set("Global Color", ofFloatColor::white));
	globalParams.add(globalAlpha.set("Global Alpha", 1.0f, 0.0f, 1.0f));
	globalLinksParams.add(indexFromColorToGlobal.set("Get G Color", 0, 0, 3));
	globalLinksParams.add(nameSourceGlobal.set("from Source", "NONE"));
	//globalLinksParams.add(vFromColorToGlobal.set("toGlobal"));
	globalParams.add(globalLinksParams);
	parameters.add(globalParams);
	nameSourceGlobal.setSerializable(false);

	bKeys.set("Keys", false); //TODO: not stored
	bGuiHelpers.set("UI HELPERS", false);
	bGuiHelpers.setSerializable(false);
	// workflow: required to exclude from states/snapshots.
	// trouble bc we would like to save & include into gui..

	//--

	parameters.add(colorParams);

	settingsParams.add(coatParams);
	parameters.add(settingsParams);

	//vLoad.set("Load");
	//vSave.set("Save");
	//moreParams.add(vLoad);
	//moreParams.add(vSave);
	//listenerSave = vSave.newListener([this](void) {
	//	save();
	//});
	//listenerLoad = vLoad.newListener([this](void) {
	//	load();
	//});

	moreParams.add(vRandomMaterialFull);
	moreParams.add(vRandomSettings);
	moreParams.add(vResetMaterial);
	moreParams.add(indexHistory);
	moreParams.add(bGuiHelpers);
	parameters.add(moreParams);

	if (name == "")
		bGui.set("UI MATERIAL", true);
	else
		bGui.set("UI " + name, true);

	//--

	ofAddListener(colorParams.parameterChangedE(), this, &SurfingMaterial::ChangedColors);
	ofAddListener(settingsParams.parameterChangedE(), this, &SurfingMaterial::ChangedSettings);
	ofAddListener(helpersParams.parameterChangedE(), this, &SurfingMaterial::ChangedHelpers);
	ofAddListener(globalParams.parameterChangedE(), this, &SurfingMaterial::ChangedGlobals);

	////refresh
	//indexFromColorToGlobal = indexFromColorToGlobal;
}

//--------------------------------------------------------------
void SurfingMaterial::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setupGui()";

	gui.setup(parameters);

#if 1
	eSaveOfxGui = gui.savePressedE.newListener([this] {
		save();
	});
	eLoadOfxGui = gui.loadPressedE.newListener([this] {
		load();
	});
#endif

	guiHelpers.setup(helpersParams);

	//--

	// minimize panels
	refreshGui();
}

//--------------------------------------------------------------
void SurfingMaterial::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:refreshGui()";

	gui.getGroup(settingsParams.getName()).getGroup(coatParams.getName()).minimize();
	gui.getGroup(colorParams.getName()).minimize();
	gui.getGroup(globalParams.getName()).getGroup(globalLinksParams.getName()).minimize();
	gui.getGroup(moreParams.getName()).minimize();

	guiHelpers.getGroup(randomizersParams.getName()).minimize();
}

//--------------------------------------------------------------
ofRectangle SurfingMaterial::getGuiShape() const {
	ofRectangle bb = gui.getShape();
	return bb;
}

//--------------------------------------------------------------
void SurfingMaterial::setGuiPosition(glm::vec2 pos) {
	gui.setPosition(pos.x, pos.y);
}

//--------------------------------------------------------------
void SurfingMaterial::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void SurfingMaterial::update() {

	//--

	if (bFlagGlobalColor) {
		bFlagGlobalColor = false;

		doGlobalColor();
	}

	if (bFlagGlobalAlpha) {
		bFlagGlobalAlpha = false;

		doGlobalAlpha();
	}

	if (bFlagFromColorIndexToGlobals) {
		bFlagFromColorIndexToGlobals = false;

		doGetGlobalColorFromIndexColor();
	}

	if (bFlagIndexFromColorToGlobal) {
		bFlagIndexFromColorToGlobal = false;

		doIndexFromColorToGlobal();
	}

	//--

	if (bFlagDoRefreshIndexHistory) {
		bFlagDoRefreshIndexHistory = false;

		doRefreshIndexHistory();
	}

	if (bFlagDoResetMaterial) {
		bFlagDoResetMaterial = false;

		doResetMaterial();
	}

	if (bFlagDoRandomMaterialFull) {
		bFlagDoRandomMaterialFull = false;

		doRandomMaterial();
	}

	if (bFlagDoRandomColors) {
		bFlagDoRandomColors = false;

		doRandomColors();
	}

	if (bFlagDoRandomColorsAlpha) {
		bFlagDoRandomColorsAlpha = false;

		doRandomColorsAlpha();
	}

	if (bFlagDoRandomAlphas) {
		bFlagDoRandomAlphas = false;

		doRandomAlphas();
	}

	if (bFlagDoRandomColorGlobal) {
		bFlagDoRandomColorGlobal = false;

		doRandomColorGlobal();
	}

	if (bFlagDoRandomSettings) {
		bFlagDoRandomSettings = false;

		doRandomSettings();
	}

	//--

	if (bFlagDoPrevHistory) {
		bFlagDoPrevHistory = false;

		doPrevHistory();
	}

	if (bFlagDoNextHistory) {
		bFlagDoNextHistory = false;

		doNextHistory();
	}

	if (bFlagDoSaveState) {
		bFlagDoSaveState = false;

		doSaveState();
	}

	if (bFlagDoStoreNewState) {
		bFlagDoStoreNewState = false;

		doStoreNewState();
	}

	if (bFlagDoRecallState) {
		bFlagDoRecallState = false;

		doRecallState(indexHistory);
	}

	if (bFlagDoRemoveState) {
		bFlagDoRemoveState = false;

		doRemoveState(indexHistory);
	}

	if (bFlagDoRefeshHistory) {
		bFlagDoRefeshHistory = false;

		refreshHistoryFolder();
	}

	if (bFlagDoClearHistory) {
		bFlagDoClearHistory = false;

		doClearHistory();
	}

	//--

	// App flow controls
	{
		// On the 1st frame
		int f = (int)(ofGetFrameNum());
		if (f >= 0) {
			if (!bAppRunning) {
				bAppRunning = true;
				ofLogNotice("ofxSurfingPBR") << "Starting app at/and ending frame number: " << ofGetFrameNum();
			}
		}
	}
}

//--------------------------------------------------------------
void SurfingMaterial::begin() {
	ofSetColor(255);

	material.begin();
}
//--------------------------------------------------------------
void SurfingMaterial::end() {
	material.end();
}

//--------------------------------------------------------------
void SurfingMaterial::drawGui() {
	if (!bGui) return;

	gui.draw();

	if (bGuiHelpers) {
		ofxSurfing::setGuiPositionBelowTo(guiHelpers, gui);
		guiHelpers.draw();
	}
}

//--

//--------------------------------------------------------------
void SurfingMaterial::ChangedHelpers(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:ChangedHelpers: " << name << ": " << e;

	//--

	if (name == vResetMaterial.getName()) {
		bFlagDoResetMaterial = true;
	}

	// Randomizers

	else if (name == vRandomMaterialFull.getName()) {
		bFlagDoRandomMaterialFull = true;
	}

	else if (name == vRandomColors.getName()) {
		bFlagDoRandomColors = true;
	}

	else if (name == vRandomColorsAlpha.getName()) {
		bFlagDoRandomColorsAlpha = true;
	}

	else if (name == vRandomAlphas.getName()) {
		bFlagDoRandomAlphas = true;
	}

	else if (name == vRandomColorsGlobal.getName()) {
		bFlagDoRandomColorGlobal = true;
	}

	else if (name == vRandomSettings.getName()) {
		bFlagDoRandomSettings = true;
	}

	//--

	// History

	else if (name == vPrevHistory.getName()) {
		bFlagDoPrevHistory = true;
	}

	else if (name == vNextHistory.getName()) {
		bFlagDoNextHistory = true;
	}

	else if (name == vSaveState.getName()) {
		bFlagDoSaveState = true;
	}

	else if (name == vStoreNewState.getName()) {
		bFlagDoStoreNewState = true;
	}

	else if (name == vRecallState.getName()) {
		bFlagDoRecallState = true;
	}

	else if (name == vRemoveState.getName()) {
		bFlagDoRemoveState = true;
	}

	else if (name == vRefeshHistory.getName()) {
		bFlagDoRefeshHistory = true;
	}

	else if (name == vClearHistory.getName()) {
		bFlagDoClearHistory = true;
	}

	else if (name == indexHistory.getName()) {
		if (!bAutoLoadHistory && !bAppRunning) return; //workaround: skip to overwrite loaded settings!
		bFlagDoRefreshIndexHistory = true;
	}
}

//--------------------------------------------------------------
void SurfingMaterial::ChangedGlobals(ofAbstractParameter & e) {

	//if (!bAppRunning) {;
	if (!bDoneSetup) {
		ofLogWarning("ofxSurfingPBR") << "Skipped bc !bDoneSetup / SurfingMaterial:ChangedGlobals: " << name << ": " << e;

		return;
	}

	string name = e.getName();
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:ChangedGlobals: " << name << ": " << e;

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	// This flag patterns helps reducing exception crashes when some callbacks happens.
	// we moved the functions out of here/the callbacks.
	// also limited many possible callbacks by calling maximum once per frame!

	if (name == globalColor.getName()) {
		if (bAttendingColors) return;
		bFlagGlobalColor = true;
	}

	else if (name == globalAlpha.getName()) {
		if (bAttendingColors) return;
		bFlagGlobalAlpha = true;
	}

	else if (name == indexFromColorToGlobal.getName()) {
		bFlagIndexFromColorToGlobal = true;
	}

	//else if (name == vFromColorToGlobal.getName()) {
	//	bFlagFromColorIndexToGlobals = true;
	//}
}

//--------------------------------------------------------------
void SurfingMaterial::ChangedColors(ofAbstractParameter & e) {
	if (bAttendingColors) return;

	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:ChangedColors: " << name << ": " << e;

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	if (name == diffuseColor.getName()) {
		if (diffuseColor.get() != material.getDiffuseColor())
			material.setDiffuseColor(diffuseColor.get());
	}

	else if (name == ambientColor.getName()) {
		if (ambientColor.get() != material.getAmbientColor())
			material.setAmbientColor(ambientColor.get());
	}

	else if (name == emissiveColor.getName()) {
		if (emissiveColor.get() != material.getEmissiveColor())
			material.setEmissiveColor(emissiveColor.get());
	}

	else if (name == specularColor.getName()) {
		if (specularColor.get() != material.getSpecularColor())
			material.setSpecularColor(specularColor.get());
	}
}

//--------------------------------------------------------------
void SurfingMaterial::ChangedSettings(ofAbstractParameter & e) {

	string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:ChangedSettings: " << name << ": " << e;

	//--

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	if (e.isSerializable()) {
		autoSaver.saveSoon();
	}
#endif

	//--

	if (name == roughness.getName()) {
		material.setRoughness(roughness);
	}

	else if (name == metallic.getName()) {
		material.setMetallic(metallic);
	}

	else if (name == reflectance.getName()) {
		material.setReflectance(reflectance);
	}

	else if (name == shininess.getName()) {
		material.setShininess(shininess);
	}

	else if (name == bClearCoat.getName()) {
		material.setClearCoatEnabled(bClearCoat);
	}

	else if (name == clearCoatRoughness.getName()) {
		material.setClearCoatRoughness(clearCoatRoughness);
	}

	else if (name == clearCoatStrength.getName()) {
		material.setClearCoatStrength(clearCoatStrength);
	}
}

//--

//--------------------------------------------------------------
void SurfingMaterial::doGlobalColor() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doGlobalColor()";

	// global color control applies to the other colors
	// without touching their original alpha's.

	ofFloatColor gc = globalColor.get();
	ofFloatColor c;
	float a;

	a = ambientColor.get().a;
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (ambientColor.get() != c)
		ambientColor.set(c);

	a = specularColor.get().a;
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (specularColor.get() != c)
		specularColor.set(c);

	a = diffuseColor.get().a;
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (diffuseColor.get() != c)
		diffuseColor.set(c);

	a = emissiveColor.get().a;
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (emissiveColor.get() != c)
		emissiveColor.set(c);
}

//--------------------------------------------------------------
void SurfingMaterial::doGlobalAlpha() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doGlobalAlpha()";

	// global alpha control applies to the other colors alpha
	// without touching their original color's.

	float a = globalAlpha.get();
	ofFloatColor c;
	ofFloatColor gc;

	gc = ambientColor.get();
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (ambientColor.get() != c)
		ambientColor.set(c);

	gc = specularColor.get();
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (specularColor.get() != c)
		specularColor.set(c);

	gc = diffuseColor.get();
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (diffuseColor.get() != c)
		diffuseColor.set(c);

	gc = emissiveColor.get();
	c = ofFloatColor(gc.r, gc.g, gc.b, a);
	if (emissiveColor.get() != c)
		emissiveColor.set(c);
}

//--------------------------------------------------------------
void SurfingMaterial::doIndexFromColorToGlobal() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doIndexFromColorToGlobal()";

	string s = "NONE";

	const int i = indexFromColorToGlobal.get();

	if (i == 0)
		s = "Ambient";
	else if (i == 1)
		s = "Specular";
	else if (i == 2)
		s = "Diffuse";
	else if (i == 3)
		s = "Emissive";

	nameSourceGlobal.set(s);

	//workflow. auto get
	doGetGlobalColorFromIndexColor();
}

//--------------------------------------------------------------
void SurfingMaterial::doGetGlobalColorFromIndexColor() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doGetGlobalColorFromIndexColor()";

	bAttendingColors = true;
	// avoid affecting the colors when touching global color!
	// this disables colors callbacks!

	ofFloatColor c = ofFloatColor(1.f, 1.f);

	const int i = indexFromColorToGlobal.get();

	if (i == 0)
		c = ambientColor.get();
	else if (i == 1)
		c = specularColor.get();
	else if (i == 2)
		c = diffuseColor.get();
	else if (i == 3)
		c = emissiveColor.get();

	globalAlpha.set(c.a);
	globalColor.set(c);

	bAttendingColors = false;
}

//--

//--------------------------------------------------------------
void SurfingMaterial::doRefreshIndexHistory() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRefreshIndexHistory()";

	if (indexHistory != indexHistory_) { //changed
		/*
		//TODO?
		//fix clamp bc name could not be correlated!
		//this happens when deleting states manually or on runtime!
		if (indexHistory.get() > indexHistory.getMax()) {
			indexHistory = ofClamp(indexHistory.get(), indexHistory.getMin(), indexHistory.getMax());
		}
		*/

		if (indexHistory >= 0 && indexHistory < sizeHistory) {

			//workflow
			///update/save current before load another.
			if (bAutoSaveBeforeChangeIndex && indexHistory_ != -1) {
				doSaveState(indexHistory_);
			}

			doRecallState();
		}

		indexHistory_ = indexHistory;
	}
}

//--

//--------------------------------------------------------------
void SurfingMaterial::doRandomMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRandomMaterial()";

	doRandomSettings();
	doRandomColors();

	if (bAutoStoreAfterRandoms) bFlagDoStoreNewState = true;
}

//--------------------------------------------------------------
void SurfingMaterial::doResetMaterialOfMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doResetMaterialOfMaterial()";

	// resets raw/vanilla OF ofMaterial
	material = ofMaterial();
#if 0
	// log defaults
	ofLog() << "material.getShininess() " << material.getShininess();
	ofLog() << "material.getRoughness() " << material.getRoughness();
	ofLog() << "material.getMetallic() " << material.getMetallic();
	ofLog() << "material.getReflectance() " << material.getReflectance();
	ofLog() << "material.getAmbientColor() " << material.getAmbientColor();
	ofLog() << "material.getSpecularColor() " << material.getSpecularColor();
	ofLog() << "material.getDiffuseColor() " << material.getDiffuseColor();
	ofLog() << "material.getEmissiveColor() " << material.getEmissiveColor();
	ofLog() << "material.isClearCoatEnabled() " << material.isClearCoatEnabled();
	ofLog() << "material.getClearCoatRoughness() " << material.getClearCoatRoughness();
	ofLog() << "material.getClearCoatStrength() " << material.getClearCoatStrength();
#endif

	// Refresh the addon to mirror/reflect the same OF internal PBR settings!
	ambientColor.set(material.getAmbientColor());
	specularColor.set(material.getSpecularColor());
	diffuseColor.set(material.getDiffuseColor());
	emissiveColor.set(material.getEmissiveColor());

	shininess.set(material.getShininess());
	roughness.set(material.getRoughness());
	metallic.set(material.getMetallic());
	reflectance.set(material.getReflectance());

	bClearCoat.set(material.isClearCoatEnabled());
	clearCoatRoughness.set(material.getClearCoatRoughness());
	clearCoatStrength.set(material.getClearCoatStrength());

	material = ofMaterial(); //workaround fix?
}

//--------------------------------------------------------------
void SurfingMaterial::doResetAll(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doResetMaterial(" << bHard << ")";
	doResetMaterial(bHard);
}

//--------------------------------------------------------------
void SurfingMaterial::doResetMaterial(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doResetMaterial()";

	if (bHard) {
		doResetMaterialOfMaterial();
	}

	//--

	// resets all the material parameters and colors

	// Addon defaults
	//ofFloatColor c = ofFloatColor(0.5f, 1.f);
	//ambientColor.set(c);
	//specularColor.set(c);
	//diffuseColor.set(c);
	//emissiveColor.set(c);
	//bClearCoat.set(false);
	//clearCoatRoughness.set(0.0001);
	//clearCoatStrength.set(0.0001);

	// OF ofMaterial defaults
	shininess.set(shininess.getMax() * 0.25f);
	roughness.set(0.5);
	metallic.set(0);
	reflectance.set(0.5);

	bClearCoat.set(false);
	clearCoatRoughness.set(0.1);
	clearCoatStrength.set(1);

	ambientColor.set(ofFloatColor(0.2, 0.2, 0.2, 1));
	specularColor.set(ofFloatColor(0, 0, 0, 1));
	diffuseColor.set(ofFloatColor(0.8, 0.8, 0.8, 1));
	emissiveColor.set(ofFloatColor(0, 0, 0, 1));

	refreshGlobals();
}

//--------------------------------------------------------------
void SurfingMaterial::doRandomSettings() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRandomSettings()";

	// randomizes the PBR settings without touching the colors.

	shininess.set(ofRandom(shininess.getMax()));
	roughness.set(ofRandom(roughness.getMax()));
	metallic.set(ofRandom(metallic.getMax()));
	reflectance.set(ofRandom(reflectance.getMax()));

	clearCoatRoughness.set(ofRandom(clearCoatRoughness.getMax()));
	clearCoatStrength.set(ofRandom(clearCoatStrength.getMax()));

	refreshGlobals();

	if (bAutoStoreAfterRandoms) bFlagDoStoreNewState = true;
}

//--------------------------------------------------------------
void SurfingMaterial::doRandomColorsAlpha() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRandomColorsAlpha()";

	// randomizes the colors and their alphas too.

	ofFloatColor c;

	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), ofRandom(1));
	ambientColor.set(c);

	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), ofRandom(1));
	specularColor.set(c);

	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), ofRandom(1));
	diffuseColor.set(c);

	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), ofRandom(1));
	emissiveColor.set(c);

	refreshGlobals();

	if (bAutoStoreAfterRandoms) bFlagDoStoreNewState = true;
}

//--------------------------------------------------------------
void SurfingMaterial::doRandomColorGlobal() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRandomColorGlobal()";

	// randomizes the global color but do not touches the alpha/s.

	ofFloatColor c;
	float a;

	a = globalColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	globalColor.set(c);

	if (bAutoStoreAfterRandoms) bFlagDoStoreNewState = true;
}

//--------------------------------------------------------------
void SurfingMaterial::doRandomColors() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRandomColors()";

	// randomizes the colors but do not touches the alphas.

	ofFloatColor c;
	float a;

	a = ambientColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	if (ambientColor.get() != c)
		ambientColor.set(c);

	a = specularColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	if (specularColor.get() != c)
		specularColor.set(c);

	a = diffuseColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	if (diffuseColor.get() != c)
		diffuseColor.set(c);

	a = emissiveColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	if (emissiveColor.get() != c)
		emissiveColor.set(c);

	refreshGlobals();

	if (bAutoStoreAfterRandoms) bFlagDoStoreNewState = true;
}

//--------------------------------------------------------------
void SurfingMaterial::doRandomAlphas() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRandomAlphas()";

	// randomizes the alpha's only of each color. Do not touches the rgb of the colors.

	ofFloatColor c;
	float a;

	a = ofRandom(1);
	c = ambientColor.get();
	c = ofFloatColor(c.r, c.g, c.b, a);
	if (ambientColor.get() != c)
		ambientColor.set(c);

	a = ofRandom(1);
	c = specularColor.get();
	c = ofFloatColor(c.r, c.g, c.b, a);
	if (specularColor.get() != c)
		specularColor.set(c);

	a = ofRandom(1);
	c = diffuseColor.get();
	c = ofFloatColor(c.r, c.g, c.b, a);
	if (diffuseColor.get() != c)
		diffuseColor.set(c);

	a = ofRandom(1);
	c = emissiveColor.get();
	c = ofFloatColor(c.r, c.g, c.b, a);
	if (emissiveColor.get() != c)
		emissiveColor.set(c);

	refreshGlobals();

	if (bAutoStoreAfterRandoms) bFlagDoStoreNewState = true;
}

//--

//--------------------------------------------------------------
void SurfingMaterial::setupHistoryManager() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setupHistoryManager()";

	indexHistory.set("State", -1, -1, -1);

	vPrevHistory.set("Prev");
	vNextHistory.set("Next");
	vRecallState.set("State Recall");
	vStoreNewState.set("State Store");
	vSaveState.set("State Save");
	vRemoveState.set("State Remove");
	vRefeshHistory.set("History Refresh");
	vClearHistory.set("History Clear");
	bAutoStoreAfterRandoms.set("Auto Store Randoms", true);
	bAutoLoadHistory.set("Auto Load History", false);

	historyParams.setName("History Browser");
	historyParams.add(vNextHistory);
	historyParams.add(vPrevHistory);
	historyParams.add(indexHistory);
	historyParams.add(bAutoLoadHistory);
	historyParams.add(bAutoStoreAfterRandoms);
	historyParams.add(vRecallState);
	historyParams.add(vStoreNewState);
	//historyParams.add(vSaveState);//disable bc is forced to auto save
	historyParams.add(vRemoveState);
	historyParams.add(vRefeshHistory);
	historyParams.add(vClearHistory);
	historyParams.add(bKeys);

	helpersParams.add(historyParams);

	sizeHistory = 0;

	//--

	reorganizeHistoryFolder();
}

//--------------------------------------------------------------
void SurfingMaterial::removeHistoryFolder() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:removeHistoryFolder() " << pathHistory;
	string p = ofToDataPath(pathHistory);
	std::filesystem::remove_all(p);
}

//--------------------------------------------------------------
const string SurfingMaterial::getFilePathHistoryState(const int & i) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:getFilePathHistoryState() " << i;

	// Check for valid index
	if (i < 0 || i > sizeHistory - 1) {
		ofLogError("ofxSurfingPBR") << "Invalid history state index: " << i;
		return "";
	}

	string p = pathHistory;
	p += "\\" + pathHistoryNameRoot + ofToString(i);
	p += ".json";
	return p;
}

//--------------------------------------------------------------
void SurfingMaterial::refreshHistoryFolder() {
	// List directory with state files.

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:refreshHistoryFolder()";
	ofLogNotice("ofxSurfingPBR") << "Path: " << pathHistory;

	ofxSurfing::checkFolderOrCreate(pathHistory);

	ofDirectory dir;

	//TODO: should ignore other formats correctly..
	dir.allowExt("json");

	dir.listDir(pathHistory);
	dir.sort();

	sizeHistory = dir.size();

	if (indexHistory.getMin() == -1 && sizeHistory > 0) indexHistory.setMin(0);

	size_t last = sizeHistory - 1;
	indexHistory.setMax(last);
	indexHistory.setWithoutEventNotifications(last);
}

//--------------------------------------------------------------
void SurfingMaterial::reorganizeHistoryFolder() {
#if 1
	//TODO:
	//Maybe we could fix below code bc we can't rename files using existing files...
	//so we must copy the folder to another place,
	//rename the files and the bring the files back to the correct/original path!
	//for the moment we disable this feature bc is not problematic,
	//only the numerated naming could miss some numbers if we delete states manually!
	refreshHistoryFolder();
#else
	// List directory with state files and rename the files if required to correlate correct order:
	// 0, 1, 2...etc without any empty "numered names"
	// Will be used when deleting files manually or on runtime!

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:reorganizeHistoryFolder()";
	ofLogNotice("ofxSurfingPBR") << pathHistory;

	ofDirectory dir(pathHistory);

	if (dir.exists()) {

		//TODO: should ignore other formats correctly..
		dir.allowExt("json");

		dir.listDir();

		sizeHistory = dir.size();

		ofLogNotice("ofxSurfingPBR") << "Renaming all files";

		for (std::size_t i = 0; i < dir.size(); ++i) {
			string p_ = dir.getPath(i);
			ofFile file(p_);
			string p = getFilePathHistoryState(i);
			file.renameTo(p, true, true);

			ofLogNotice("ofxSurfingPBR") << p;
		}

		//--

		refreshHistoryFolder();
	} else {
		ofLogError("ofxSurfingPBR") << "Directory does not exist: " << pathHistory;
	}
#endif
}

//--------------------------------------------------------------
void SurfingMaterial::doClearHistory() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doClearHistory()";

	sizeHistory = 0;

	removeHistoryFolder();

	indexHistory.setWithoutEventNotifications(-1);
	indexHistory.setMin(-1);
	indexHistory.setMax(-1);
}

//--------------------------------------------------------------
void SurfingMaterial::doPrevHistory() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doPrevHistory()";

	if (sizeHistory == 0) {
		ofLogWarning("ofxSurfingPBR") << "Empty history";
		return;
	}

	if (indexHistory > 0)
		indexHistory = indexHistory - 1;
	else
		indexHistory = indexHistory.getMax(); //cycled
	//indexHistory = indexHistory.getMin(); //clamped
}

//--------------------------------------------------------------
void SurfingMaterial::doNextHistory() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doNextHistory()";

	if (sizeHistory == 0) {
		ofLogWarning("ofxSurfingPBR") << "Empty history";
		return;
	}

	if (indexHistory < indexHistory.getMax())
		indexHistory = indexHistory + 1;
	else
		indexHistory = indexHistory.getMin(); //cycled
	//indexHistory = indexHistory.getMax(); //clamped
}

//--------------------------------------------------------------
void SurfingMaterial::doSaveState(int i) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doSaveState(" << i << ")";

	// save over loaded or passed state

	string p;

	// save file
	if (i == -1) {
		p = getFilePathHistoryState(indexHistory);
	} else {
		p = getFilePathHistoryState(i);
	}

	ofxSurfing::saveSettings(parameters, p);
}

//--------------------------------------------------------------
void SurfingMaterial::doStoreNewState() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doStoreNewState()";

	// save a new state at the end

	sizeHistory++;

	if (indexHistory.getMin() == -1) indexHistory.setMin(0);
	size_t last = sizeHistory - 1;
	indexHistory.setMax(last);
	indexHistory.setWithoutEventNotifications(last);

	//save file
	int i = indexHistory;
	string p = getFilePathHistoryState(i);

	ofxSurfing::saveSettings(parameters, p);
}

//--------------------------------------------------------------
void SurfingMaterial::refreshGlobals() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:refreshGlobals()";

	// workflow
	// Auto get global from a source color.

	//vFromColorToGlobal.trigger();
	//will not work during setup bc callbacks are bypassed until setup is done!
	// So we flag to be called on next frame/update.
	//bFlagFromColorIndexToGlobals = true;
	doGetGlobalColorFromIndexColor();
}

//--------------------------------------------------------------
void SurfingMaterial::doRecallState() {
	doRecallState(indexHistory);
}

//--------------------------------------------------------------
void SurfingMaterial::doRecallState(int i) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRecallState(" << i << ")";

	if (i >= 0 && i < sizeHistory) {
		//load file
		int i = indexHistory;
		string p = getFilePathHistoryState(i);

		ofxSurfing::loadSettings(parameters, p);

		refreshGlobals();
	} else
		ofLogError("ofxSurfingPBR") << "Out of range of history for index: " << i;
}

//--------------------------------------------------------------
void SurfingMaterial::doRemoveState(int i) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRemoveState(" << i << ")";

	if (i >= 0 && i < sizeHistory) {
		//remove file
		int i = indexHistory;
		string p = getFilePathHistoryState(i);

		ofFile file(p);
		if (file.remove()) {
			ofLogNotice("ofxSurfingPBR") << "File removed successfully: " << p;

			// We need to rename all the files to correlate numeration!
			reorganizeHistoryFolder();

			// re set index
			//indexHistory = ofClamp(i, 0, sizeHistory - 1);
		} else {
			ofLogError("ofxSurfingPBR") << "Failed to remove file: " << p;
		}
	} else
		ofLogError("ofxSurfingPBR") << "Out of range of history for index: " << i;
}

//--

//--------------------------------------------------------------
void SurfingMaterial::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:save -> " << path;

	if (path == "") {
		ofLogWarning("ofxSurfingPBR") << "Settings path is empty!";
	}

	// Save
	{
		bool b = ofxSurfing::saveSettings(parameters, path);
		if (b) {
			ofLogNotice("ofxSurfingPBR") << "Successful saving " << path;
		}

		//TODO
		////workflow
		//if (bAutoSaveBeforeChangeIndex) {
		//	doSaveState(indexHistory);
		//}
	}
}

//--------------------------------------------------------------
string SurfingMaterial::getName() {
	return name;
}

//--------------------------------------------------------------
void SurfingMaterial::setName(const string & n) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setName(" << n << ")";
	name = n;
	//bGui.setName("UI " + name);
}

//--------------------------------------------------------------
void SurfingMaterial::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:load -> " << path;

	if (path == "") {
		ofLogWarning("ofxSurfingPBR") << "Settings path is empty!";
	}

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	// Load
	{
		//doResetMaterialOfMaterial();//TODO fix coherence with OF vanilla.. it looks darker..
		bool b = ofxSurfing::loadSettings(parameters, path);
		if (b) {
			ofLogNotice("ofxSurfingPBR") << "Successful loading " << path;
		}
	}

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	refreshGlobals();

	// fix
	bFlagIndexFromColorToGlobal = true;
}

//--------------------------------------------------------------
void SurfingMaterial::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:exit()";
	// Not required to be called bc it's using the auto saver!

#ifdef SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE
	save();
#endif

	bDoneExit = true;
}

//--------------------------------------------------------------
ofParameter<int> & SurfingMaterial::getIndexStateParam() {
	return indexHistory;
}

//--------------------------------------------------------------
void SurfingMaterial::keyPressed(int key) {
	if (!bKeys) return;

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::keyPressed(" << key << ") " << name;

	// Randomizers
	if (key == OF_KEY_F1) doResetMaterial();
	if (key == OF_KEY_F2) doRandomMaterial();
	if (key == OF_KEY_F3) doRandomSettingsUser();
	if (key == OF_KEY_F4) doRandomColorGlobalUser();
	if (key == OF_KEY_F5) doRandomColorsUser();
	if (key == OF_KEY_F6) doRandomColorsAlphaUser();
	if (key == OF_KEY_F7) doRandomAlphasUser();

	// History
	if (key == 'z') doPrevHistory();
	if (key == 'x') doNextHistory();
	if (key == 'r') doRecallState();
	if (key == 's') doStoreNewState();
	if (key == 'S') doSaveState();
}

//--

//--------------------------------------------------------------
void SurfingMaterial::doResetMaterialUser() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::doResetMaterialUser()";

	doResetMaterial();
	doStoreNewState();
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomUser() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::doRandomUser()";

	doRandomMaterial();
	doStoreNewState();
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomSettingsUser() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::doRandomSettingsUser()";

	doRandomSettings();
	doStoreNewState();
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColorGlobalUser() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::doRandomColorGlobalUser()";

	doRandomColorGlobal();
	doStoreNewState();
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColorsUser() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::doRandomColorsUser()";

	doRandomColors();
	doStoreNewState();
};
//--------------------------------------------------------------
void SurfingMaterial::doRandomColorsAlphaUser() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::doRandomColorsAlphaUser()";

	doRandomColorsAlpha();
	doStoreNewState();
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomAlphasUser() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial::doRandomAlphasUser()";

	doRandomAlphas();
	doStoreNewState();
}