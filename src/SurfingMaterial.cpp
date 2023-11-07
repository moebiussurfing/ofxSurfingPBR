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

	parameters.setName("PBR_Material");

	colorParams.setName("Colors");
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

	colorParams.add(ambientColor.set("Ambient Color", ofFloatColor::white));
	colorParams.add(specularColor.set("Specular Color", ofFloatColor::white));
	colorParams.add(diffuseColor.set("Diffuse Color", ofFloatColor::white));
	colorParams.add(emissiveColor.set("Emissive Color", ofFloatColor::white));

	coatParams.add(bClearCoat.set("Clear Coat", false));
	coatParams.add(clearCoatRoughness.set("Clear Coat Roughness", 0.0001, 0.0001, 10.0));
	coatParams.add(clearCoatStrength.set("Clear Coat Strength", 0.0001, 0.0001, 10.0));

	vResetMaterial.set("Reset Material");

	//--

	randomizersParams.add(vRandomMaterial.set("Random Material"));
	randomizersParams.add(vRandomSettings.set("Random Settings"));
	randomizersParams.add(vRandomColorsGlobal.set("Random Global Color"));
	randomizersParams.add(vRandomColors.set("Random Colors"));
	randomizersParams.add(vRandomColorsAlpha.set("Random ColorsAlpha"));
	randomizersParams.add(vRandomAlphas.set("Random Alphas"));
	helpersParams.add(randomizersParams);

	//TODO
	// History undo/redo
	setupHistoryManager();

	//--

	parameters.add(globalColor.set("Global Color", ofFloatColor::white));
	parameters.add(globalAlpha.set("Global Alpha", 1.0f, 0.0f, 1.0f));

	parameters.add(settingsParams);

	parameters.add(colorParams);
	parameters.add(coatParams);

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	//parameters.add(autoSaver.bEnable);
	autoSaver.bEnable.setSerializable(false);
#endif

	parameters.add(vResetMaterial);

	//workflow
	//parameters.add(indexHistory);

	bGuiHelpers.set("Helpers", true);

	bGuiHelpers.setSerializable(false);
	//workflow: to exclude from states. trouble bc we would like to save & include into gui..

	parameters.add(bGuiHelpers);

	//--

	ofAddListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
	ofAddListener(helpersParams.parameterChangedE(), this, &SurfingMaterial::ChangedHelpers);
}

//--------------------------------------------------------------
void SurfingMaterial::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setupGui()";

	gui.setup(parameters);
	
	guiHelpers.setup(helpersParams);

	//--

	// minimize panels

	gui.getGroup(coatParams.getName()).minimize();
	gui.getGroup(colorParams.getName()).minimize();

	//guiHelpers.minimizeAll();
	guiHelpers.getGroup(randomizersParams.getName()).minimize();
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
		guiHelpers.setPosition(p + glm::vec2(0, SURFING__PAD_OFXGUI_BETWEEN_PANELS));
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
		if(bAutoStoreAfterRandoms) doStoreNewState();
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
void SurfingMaterial::Changed(ofAbstractParameter & e) {
	//if (bDisableCallbacks) return;

	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Changed: " << name << ": " << e;

#ifdef SURFING__USE_AUTOSAVE_SETTINGS_ENGINE
	autoSaver.saveSoon();
#endif

	//--

	if (name == globalColor.getName()) {
		ofFloatColor gc = globalColor.get();
		ofFloatColor c;
		float a;

		a = ambientColor.get().a;
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		ambientColor.set(c);

		a = specularColor.get().a;
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		specularColor.set(c);

		a = diffuseColor.get().a;
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		diffuseColor.set(c);

		a = emissiveColor.get().a;
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		emissiveColor.set(c);
	}

	else if (name == globalAlpha.getName()) {
		float a = globalAlpha.get();
		ofFloatColor c;
		ofFloatColor gc;

		gc = ambientColor.get();
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		ambientColor.set(c);

		gc = specularColor.get();
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		specularColor.set(c);

		gc = diffuseColor.get();
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		diffuseColor.set(c);

		gc = emissiveColor.get();
		c = ofFloatColor(gc.r, gc.g, gc.b, a);
		emissiveColor.set(c);
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

	else if (name == diffuseColor.getName()) {
		material.setDiffuseColor(diffuseColor);
	} else if (name == ambientColor.getName()) {
		material.setAmbientColor(ambientColor);
	} else if (name == emissiveColor.getName()) {
		material.setEmissiveColor(emissiveColor);
	} else if (name == specularColor.getName()) {
		material.setSpecularColor(specularColor);
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
	//randomize settings.

	shininess.set(ofRandom(1));
	roughness.set(ofRandom(1));
	metallic.set(ofRandom(1));
	reflectance.set(ofRandom(1));
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColorsAlpha() {
	//randomizes the colors and their alphas too.

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
	//randomizes the global color but do not touches the alpha/s.

	ofFloatColor c;
	float a;

	a = globalColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	globalColor.set(c);
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColors() {
	//randomizes the colors but do not touches the alphas.

	ofFloatColor c;
	float a;

	a = ambientColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	ambientColor.set(c);

	a = specularColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	specularColor.set(c);

	a = diffuseColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	diffuseColor.set(c);

	a = emissiveColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
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
	ambientColor.set(c);

	a = ofRandom(1);
	c = specularColor.get();
	c = ofFloatColor(c.r, c.g, c.b, a);
	specularColor.set(c);

	a = ofRandom(1);
	c = diffuseColor.get();
	c = ofFloatColor(c.r, c.g, c.b, a);
	diffuseColor.set(c);

	a = ofRandom(1);
	c = emissiveColor.get();
	c = ofFloatColor(c.r, c.g, c.b, a);
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
const string SurfingMaterial::getFilepathHistoryState(const int & i) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:getFilepathHistoryState() " << i;

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
			string p = getFilepathHistoryState(i);
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
		p = getFilepathHistoryState(indexHistory);
	} else {
		p = getFilepathHistoryState(i);
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
	string p = getFilepathHistoryState(i);

	ofxSurfing::saveSettings(parameters, p);
}

//--------------------------------------------------------------
void SurfingMaterial::restoreGlobal() {
#if 0
	//TODO: need fix: 
	// if we want to call after recalling a state, 
	// bc gui is not being updated..
	globalColor.setWithoutEventNotifications(ofFloatColor(1, 1));
	globalAlpha.setWithoutEventNotifications(1);
#else
	globalColor.set(ofFloatColor(1, 1));
	globalAlpha.set(1);
#endif
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
		string p = getFilepathHistoryState(i);

		restoreGlobal();
		ofxSurfing::loadSettings(parameters, p);
		//restoreGlobal();
	} else
		ofLogError("ofxSurfingPBR") << "Out of range of history for index: " << i;
}

//--------------------------------------------------------------
void SurfingMaterial::doRemoveState(int i) {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:doRemoveState(" << i << ")";

	if (i >= 0 && i < sizeHistory) {
		//remove file
		int i = indexHistory;
		string p = getFilepathHistoryState(i);

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
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Save: " << path;

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
void SurfingMaterial::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Load: " << path;

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
}

//--------------------------------------------------------------
void SurfingMaterial::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:exit()";

	save();
}

//--------------------------------------------------------------
ofParameter<int> & SurfingMaterial::getIndexStateParam() {
	return indexHistory;
}