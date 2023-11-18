#include "SurfingMaterial.h"
//--------------------------------------------------------------
SurfingMaterial::SurfingMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:constructor()";

	ofAddListener(ofEvents().update, this, &SurfingMaterial::update);
}

//--------------------------------------------------------------
SurfingMaterial::~SurfingMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:destructor()";

	ofRemoveListener(ofEvents().update, this, &SurfingMaterial::update);
	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
	ofRemoveListener(helpersParams.parameterChangedE(), this, &SurfingMaterial::ChangedHelpers);
	ofRemoveListener(globalParams.parameterChangedE(), this, &SurfingMaterial::ChangedGlobals);
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
}

//--------------------------------------------------------------
void SurfingMaterial::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setupParams()";

	//--

	string nameParams;

	if (name == "") { // default workflow. when using one single material instance
		path = pathRoot + ext;

		nameParams = "PBR_MATERIAL";
	}

	else { // to allow multiple instances. to be used for gui naming and settings path!
		//string n = "PBR_MATERIAL";
		//n += string("_");
		//n += name;

		path = pathRoot + "_" + name + ext;

		pathHistory = pathHistory + "material_" + name;

		nameParams = name;
	}

	//--

	parameters.setName(nameParams);

	colorParams.setName("Colors");
	globalParams.setName("Globals");
	settingsParams.setName("Settings");
	coatParams.setName("Coat");
	randomizersParams.setName("Randomizers");
	helpersParams.setName("Helpers");

	//--

	globalColor.setSerializable(false);
	globalAlpha.setSerializable(false);

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	// auto saver
	callback_t f = std::bind(&SurfingMaterial::save, this);
	autoSaver.setFunctionSaver(f);
#endif

	//--

	settingsParams.add(shininess.set("Shininess", 0.0, 0.0, 1.0));
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

	//--

	randomizersParams.add(vRandomMaterial.set("Mat Random Full"));
	randomizersParams.add(vRandomSettings.set("Mat Random Settings"));
	randomizersParams.add(vRandomColorsGlobal.set("Random Global Color"));
	randomizersParams.add(vRandomColors.set("Random Colors"));
	randomizersParams.add(vRandomColorsAlpha.set("Random ColorsAlpha"));
	randomizersParams.add(vRandomAlphas.set("Random Alphas"));
	helpersParams.add(randomizersParams);

	//TODO
	// History undo/redo
	setupHistoryManager();

	//--

	globalParams.add(globalColor.set("Global Color", ofFloatColor::white));
	globalParams.add(globalAlpha.set("Global Alpha", 1.0f, 0.0f, 1.0f));
	globalParams.add(nameSourceGlobal.set("Source", "NONE"));
	globalParams.add(fromColorToGlobal.set("fromColor", 0, 0, 3));
	globalParams.add(vToGlobal.set("toGlobal"));
	parameters.add(globalParams);
	nameSourceGlobal.setSerializable(false);

	//--

	parameters.add(colorParams);

	settingsParams.add(coatParams);
	parameters.add(settingsParams);

	parameters.add(vRandomMaterial);
	parameters.add(vRandomSettings);
	parameters.add(vResetMaterial);

	//workflow
	//parameters.add(indexHistory);

	bGui.set("UI MATERIAL", true);

	bGuiHelpers.set("UI HELPERS", false);
	bGuiHelpers.setSerializable(false);
	// workflow: required to exclude from states/psnapshots. 
	// trouble bc we would like to save & include into gui..

	parameters.add(bGuiHelpers);

	//--

	ofAddListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
	ofAddListener(helpersParams.parameterChangedE(), this, &SurfingMaterial::ChangedHelpers);
	ofAddListener(globalParams.parameterChangedE(), this, &SurfingMaterial::ChangedGlobals);
}

//--------------------------------------------------------------
void SurfingMaterial::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setupGui()";

	gui.setup(parameters);

#if 1
	static ofEventListener listenerSave;
	static ofEventListener listenerLoad;
	listenerSave = gui.savePressedE.newListener([this] {
		save();
	});
	listenerLoad = gui.loadPressedE.newListener([this] {
		load();
	});
#endif

	guiHelpers.setup(helpersParams);

	//--

	// minimize panels

	gui.getGroup(settingsParams.getName()).getGroup(coatParams.getName()).minimize();
	gui.getGroup(colorParams.getName()).minimize();

	//guiHelpers.minimizeAll();
	guiHelpers.getGroup(randomizersParams.getName()).minimize();
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
	gui.draw();

	if (bGuiHelpers) {
		auto p = gui.getShape().getBottomLeft();
		guiHelpers.setPosition(p + glm::vec2(0, SURFING__PAD_OFXGUI_BETWEEN_PANELS * 2));
		guiHelpers.draw();
	}
}

//--------------------------------------------------------------
void SurfingMaterial::ChangedHelpers(ofAbstractParameter & e) {
	//if (bDisableCallbacks) return;

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:ChangedHelpers: " << name << ": " << e;

	//--

	// Randomizers
	if (name == vRandomMaterial.getName()) {
		doRandomMaterial();
		if (bAutoStoreAfterRandoms) doStoreNewState();
	} else if (name == vRandomColors.getName()) {
		doRandomColors();
		if (bAutoStoreAfterRandoms) doStoreNewState();
	} else if (name == vRandomColorsGlobal.getName()) {
		doRandomColorGlobal();
		if (bAutoStoreAfterRandoms) doStoreNewState();
	} else if (name == vRandomColorsAlpha.getName()) {
		doRandomColorsAlpha();
		if (bAutoStoreAfterRandoms) doStoreNewState();
	} else if (name == vRandomAlphas.getName()) {
		doRandomAlphas();
		if (bAutoStoreAfterRandoms) doStoreNewState();
	} else if (name == vRandomSettings.getName()) {
		doRandomSettings();
		if (bAutoStoreAfterRandoms) doStoreNewState();
	}

	//--

	// History
	else if (name == vPrevHistory.getName()) {
		//bDisableCallbacks = true;
		doPrevHistory();
		//bDisableCallbacks = false;
	} else if (name == vNextHistory.getName()) {
		doNextHistory();
	} else if (name == vSaveState.getName()) {
		doSaveState();
	} else if (name == vStoreNewState.getName()) {
		doStoreNewState();
	} else if (name == vRecallState.getName()) {
		doRecallState(indexHistory);
	} else if (name == vRemoveState.getName()) {
		doRemoveState(indexHistory);
	} else if (name == vRefeshHistory.getName()) {
		refreshHistoryFolder();
	} else if (name == vClearHistory.getName()) {
		doClearHistory();
	}

	else if (name == indexHistory.getName()) {

		/*
		//TODO?
		//fix clamp bc name could not be correlated!
		//this happens when deleting states manually or on runtime!
		if (indexHistory.get() > indexHistory.getMax()) {
			indexHistory = ofClamp(indexHistory.get(), indexHistory.getMin(), indexHistory.getMax());
		}
		*/

		if (indexHistory != indexHistory_) { //changed

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
}

//--------------------------------------------------------------
void SurfingMaterial::ChangedGlobals(ofAbstractParameter & e) {

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:ChangedGlobals: " << name << ": " << e;

	// exclude
	if (name == nameSourceGlobal.getName()) {
		return;
	}

	//--
	
#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (name == globalColor.getName()) {
		if (bAttendingGlobal) return;
		
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

	else if (name == globalAlpha.getName()) {
		if (bAttendingGlobal) return;

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

	else if (name == fromColorToGlobal.getName()) {
		string s = "NONE";

		const int i = fromColorToGlobal.get();

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
		vToGlobal.trigger();
	}

	else if (name == vToGlobal.getName()) {
		ofFloatColor c = ofFloatColor(1.f, 1.f);

		const int i = fromColorToGlobal.get();

		if (i == 0)
			c = ambientColor.get();
		else if (i == 1)
			c = specularColor.get();
		else if (i == 2)
			c = diffuseColor.get();
		else if (i == 3)
			c = emissiveColor.get();

		bAttendingGlobal = true;
		{
			globalColor.set(c);
			globalAlpha.set(c.a);
		}
		bAttendingGlobal = false;
	}
}

//--------------------------------------------------------------
void SurfingMaterial::Changed(ofAbstractParameter & e) {
	//if (bDisableCallbacks) return;

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Changed: " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	 if (name == diffuseColor.getName()) {
		if (diffuseColor.get() != material.getDiffuseColor())
			material.setDiffuseColor(diffuseColor.get());
	} else if (name == ambientColor.getName()) {
		if (ambientColor.get() != material.getAmbientColor())
			material.setAmbientColor(ambientColor.get());
	} else if (name == emissiveColor.getName()) {
		if (emissiveColor.get() != material.getEmissiveColor())
			material.setEmissiveColor(emissiveColor.get());
	} else if (name == specularColor.getName()) {
		if (specularColor.get() != material.getSpecularColor())
			material.setSpecularColor(specularColor.get());
	}

	//--

	else if (name == roughness.getName()) {
		material.setRoughness(roughness);
	} else if (name == metallic.getName()) {
		material.setMetallic(metallic);
	} else if (name == reflectance.getName()) {
		material.setReflectance(reflectance);
	} else if (name == shininess.getName()) {
		material.setShininess(shininess);
	}

	else if (name == bClearCoat.getName()) {
		material.setClearCoatEnabled(bClearCoat);
	} else if (name == clearCoatRoughness.getName()) {
		material.setClearCoatRoughness(clearCoatRoughness);
	} else if (name == clearCoatStrength.getName()) {
		material.setClearCoatStrength(clearCoatStrength);
	}

	else if (name == vResetMaterial.getName()) {
		doResetMaterial();
	}
}

//--------------------------------------------------------------
void SurfingMaterial::doRandomMaterial() {
	doRandomSettings();
	doRandomColors();
}

//--------------------------------------------------------------
void SurfingMaterial::doResetMaterial() {
	//reset all the material params and colors with alpha settled to full.

	shininess.set(0);
	roughness.set(0);
	metallic.set(0);
	reflectance.set(0);

	ofFloatColor c = ofFloatColor(0.5f, 1.f);

	globalColor.set(c);
	globalAlpha.set(1.f);

	// force again to overwrite the alphas too
	ambientColor.set(c);
	specularColor.set(c);
	diffuseColor.set(c);
	emissiveColor.set(c);

	bClearCoat.set(false);
	clearCoatRoughness.set(0.0001);
	clearCoatStrength.set(0.0001);
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomSettings() {
	// randomizes the PBR settings without touching the colors.

	shininess.set(ofRandom(1));
	roughness.set(ofRandom(1));
	metallic.set(ofRandom(1));
	reflectance.set(ofRandom(1));
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColorsAlpha() {
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
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColorGlobal() {
	// randomizes the global color but do not touches the alpha/s.

	ofFloatColor c;
	float a;

	a = globalColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	globalColor.set(c);
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColors() {
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
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomAlphas() {
	//randomizes the alpha's only of each color. Do not touches the rgb of the colors.

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

	//workflow
	//indexHistory.setSerializable(false);

	historyParams.setName("History Browser");
	historyParams.add(vNextHistory);
	historyParams.add(vPrevHistory);
	historyParams.add(indexHistory);
	historyParams.add(vRecallState);
	historyParams.add(vStoreNewState);
	//historyParams.add(vSaveState);//forced autosave
	historyParams.add(vRemoveState);
	historyParams.add(bAutoStoreAfterRandoms);
	historyParams.add(vRefeshHistory);
	historyParams.add(vClearHistory);

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
	ofLogNotice("ofxSurfingPBR") << pathHistory;

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
		indexHistory = indexHistory.getMax();
	//indexHistory = indexHistory.getMin();
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
		indexHistory = indexHistory.getMin();
	//indexHistory = indexHistory.getMax();
}

//--------------------------------------------------------------
void SurfingMaterial::doSaveState(int i) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doSaveState(" << i << ")";
	//save over loaded or passed state

	string p;

	//save file
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
	//save a new state at the end

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
void SurfingMaterial::restoreGlobal() {
	//workflow. auto get
	vToGlobal.trigger();

//#if 0
//	//TODO: need fix: 
//	// if we want to call after recalling a state, 
//	// bc gui is not being updated..
//	globalColor.setWithoutEventNotifications(ofFloatColor(1, 1));
//	globalAlpha.setWithoutEventNotifications(1);
//#else
//	globalColor.set(ofFloatColor(1, 1));
//	globalAlpha.set(1);
//#endif
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
		
		restoreGlobal();
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
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Save(" << path << ")";

	if (path == "") {
		ofLogWarning("ofxSurfingPBR") << "Settings path is empty!";
	}

	// Save
	{
		ofxSurfing::saveSettings(parameters, path);

		//workflow
		if (bAutoSaveBeforeChangeIndex) {
			doSaveState(indexHistory);
		}
	}
}

//--------------------------------------------------------------
void SurfingMaterial::setName(const string & n) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setName(" << n << ")";
	name = n;
}

//--------------------------------------------------------------
void SurfingMaterial::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Load(" << path << ")";

	if (path == "") {
		ofLogWarning("ofxSurfingPBR") << "Settings path is empty!";
	}

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.pause();
#endif

	// Load
	{
		ofxSurfing::loadSettings(parameters, path);
	}

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.start();
#endif

	//workflow. auto get
	restoreGlobal();
}

//--------------------------------------------------------------
void SurfingMaterial::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:exit()";
	// Not required to be called bc it's using the auto saver!

#if defined(SURFING__USE_AUTOSAVE_FORCE_ON_EXIT) || !defined(SURFING__USE_AUTOSAVE_SETTINGS_ENGINE)
	save();
#endif
}

//--------------------------------------------------------------
ofParameter<int> & SurfingMaterial::getIndexStateParam() {
	return indexHistory;
}