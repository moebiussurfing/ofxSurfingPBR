#include "SurfingMaterial.h"
//--------------------------------------------------------------
SurfingMaterial::SurfingMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:constructor()";

	ofAddListener(ofEvents().update, this, &SurfingMaterial::update);
	ofAddListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
}

//--------------------------------------------------------------
SurfingMaterial::~SurfingMaterial() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:destructor()";

	ofRemoveListener(ofEvents().update, this, &SurfingMaterial::update);
	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
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

	parameters.setName("PBR_Material");

	colorParams.setName("Colors");
	settingsParams.setName("Settings");
	coatParams.setName("Coat");
	helpersParams.setName("Helpers");

	globalColor.setSerializable(false);

	settingsParams.add(shininess.set("Shininess", 0.0, 0.0, 1.0));
	settingsParams.add(roughness.set("Roughness", 0.0, 0.0, 1.0));
	settingsParams.add(metallic.set("Metallic", 0.0, 0.0, 1.0));
	settingsParams.add(reflectance.set("Reflectance", 0.0, 0.0, 1.0));
	parameters.add(settingsParams);

	colorParams.add(globalColor.set("Global Color", ofColor::white));
	colorParams.add(ambientColor.set("Ambient Color", ofColor::white));
	colorParams.add(specularColor.set("Specular Color", ofColor::white));
	colorParams.add(diffuseColor.set("Diffuse Color", ofColor::white));
	colorParams.add(emissiveColor.set("Emissive Color", ofColor::white));
	parameters.add(colorParams);

	coatParams.add(clearCoat.set("Clear Coat", false));
	coatParams.add(clearCoatRoughness.set("Clear Coat Roughness", 0.0001, 0.0001, 10.0));
	coatParams.add(clearCoatStrength.set("Clear Coat Strength", 0.0001, 0.0001, 10.0));
	parameters.add(coatParams);

	helpersParams.add(randomSettings.set("Random Settings"));
	helpersParams.add(randomColorsGlobal.set("Random Global Color"));
	helpersParams.add(randomColors.set("Random Colors"));
	helpersParams.add(randomColorsAlpha.set("Random ColorsAlpha"));
	helpersParams.add(randomMaterial.set("Random Material"));
	helpersParams.add(resetMaterial.set("Reset Material"));
	parameters.add(helpersParams);

	bAutoSave.set("Autosave", true);
	parameters.add(bAutoSave);
}

//--------------------------------------------------------------
void SurfingMaterial::setupGui() {
	gui.setup(parameters);

	//minimize
	gui.getGroup(coatParams.getName()).minimize();
	gui.getGroup(helpersParams.getName()).minimize();
}

//--------------------------------------------------------------
void SurfingMaterial::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void SurfingMaterial::update() {

	// autosave workflow
	auto t = ofGetElapsedTimeMillis() - timeLastChange;
	if (bAutoSave && bFlagSave && t > timeSaveGap) {
		bFlagSave = false;
		save();
	}
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
}

//--------------------------------------------------------------
void SurfingMaterial::Changed(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Changed: " << name << ": " << e;

	//for every modified param, flag to save on the next frame but after a time gap.
	if (bAutoSave) {
		bFlagSave = true;
		timeLastChange = ofGetElapsedTimeMillis();
	}

	if (name == resetMaterial.getName()) {
		doResetMaterial();
	} else if (name == randomMaterial.getName()) {
		doRandomMaterial();
	} else if (name == randomColors.getName()) {
		doRandomColors();
	} else if (name == randomColorsGlobal.getName()) {
		doRandomColorGlobal();
	} else if (name == randomColorsAlpha.getName()) {
		doRandomColorsAlpha();
	} else if (name == randomSettings.getName()) {
		doRandomSettings();
	}

	else if (name == globalColor.getName()) {
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

	//--

	//TODO improve avoiding update() call even when not required..

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

	else if (name == clearCoat.getName()) {
		material.setClearCoatEnabled(clearCoat);
	} else if (name == clearCoatRoughness.getName()) {
		material.setClearCoatRoughness(clearCoatRoughness);
	} else if (name == clearCoatStrength.getName()) {
		material.setClearCoatStrength(clearCoatStrength);
	}
}

//--------------------------------------------------------------
void SurfingMaterial::doRandomMaterial() {
	doRandomSettings();
	doRandomColors();
}
//--------------------------------------------------------------
void SurfingMaterial::doResetMaterial() {
	//reset all the material, params and colors to full alpha.

	shininess.set(0);
	roughness.set(0);
	metallic.set(0);
	reflectance.set(0);

	globalColor.set(ofColor::white);
	// will set all color except the alphas...

	// force again to overwrite the alphas too
	ambientColor.set(ofColor::white);
	specularColor.set(ofColor::white);
	diffuseColor.set(ofColor::white);
	emissiveColor.set(ofColor::white);

	clearCoat.set(false);
	clearCoatRoughness.set(0.0001);
	clearCoatStrength.set(0.0001);
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomSettings() {
	//randomize params

	shininess.set(ofRandom(1));
	roughness.set(ofRandom(1));
	metallic.set(ofRandom(1));
	reflectance.set(ofRandom(1));
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColorsAlpha() {
	//randomizes the alphas too

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
	//do not touches the alphas

	ofFloatColor c;
	float a;
	a = globalColor.get().a;
	c = ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1), a);
	globalColor.set(c);
}
//--------------------------------------------------------------
void SurfingMaterial::doRandomColors() {
	//do not touches the alphas

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
void SurfingMaterial::exit() {
	ofLogNotice("ofxSurfingPBR") << "exit()";

	save();
}

//--------------------------------------------------------------
void SurfingMaterial::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Save: " << path;

	gui.saveToFile(path);
}
//--------------------------------------------------------------
void SurfingMaterial::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Load: " << path;

	gui.loadFromFile(path);
}