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
	helpersParams.setName("Helpers");

	//--

	helpersParams.setSerializable(false);

	globalColor.setSerializable(false);
	globalAlpha.setSerializable(false);

	// autosaver
	callback_t f = std::bind(&SurfingMaterial::save, this);
	autoSaver.setFunctionSaver(f);

	//--

	settingsParams.add(shininess.set("Shininess", 0.0, 0.0, 1.0));
	settingsParams.add(roughness.set("Roughness", 0.0, 0.0, 1.0));
	settingsParams.add(metallic.set("Metallic", 0.0, 0.0, 1.0));
	settingsParams.add(reflectance.set("Reflectance", 0.0, 0.0, 1.0));

	colorParams.add(ambientColor.set("Ambient Color", ofColor::white));
	colorParams.add(specularColor.set("Specular Color", ofColor::white));
	colorParams.add(diffuseColor.set("Diffuse Color", ofColor::white));
	colorParams.add(emissiveColor.set("Emissive Color", ofColor::white));

	coatParams.add(bClearCoat.set("Clear Coat", false));
	coatParams.add(clearCoatRoughness.set("Clear Coat Roughness", 0.0001, 0.0001, 10.0));
	coatParams.add(clearCoatStrength.set("Clear Coat Strength", 0.0001, 0.0001, 10.0));

	helpersParams.add(resetMaterial.set("Reset Material"));
	helpersParams.add(randomMaterial.set("Random Material"));
	helpersParams.add(randomSettings.set("Random Settings"));
	helpersParams.add(randomColorsGlobal.set("Random Global Color"));
	helpersParams.add(randomColors.set("Random Colors"));
	helpersParams.add(randomColorsAlpha.set("Random ColorsAlpha"));
	helpersParams.add(randomAlphas.set("Random Alphas"));

	//--

	parameters.add(settingsParams);

	parameters.add(globalAlpha.set("Global Alpha", 1.0f, 0.0f, 1.0f));
	parameters.add(globalColor.set("Global Color", ofColor::white));

	parameters.add(colorParams);
	parameters.add(coatParams);
	parameters.add(helpersParams);

	parameters.add(autoSaver.bAutoSave);

	ofAddListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
}

//--------------------------------------------------------------
void SurfingMaterial::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:setupGui()";

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

	autoSaver.saveSoon();

	//--

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
	} else if (name == randomAlphas.getName()) {
		doRandomAlphas();
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

	// moved here from update()
	// improve avoiding call on each frame
	// even when not required..

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
void SurfingMaterial::doRandomAlphas() {
	//only touches the alphas

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

//--------------------------------------------------------------
void SurfingMaterial::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:exit()";

	save();
}

//--------------------------------------------------------------
void SurfingMaterial::save() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Save: " << path;

	// Save
	{
		//gui.saveToFile(path);
		ofxSurfing::saveSettings(parameters, path);
	}
}

//--------------------------------------------------------------
void SurfingMaterial::load() {
	ofLogNotice("ofxSurfingPBR") << "SurfingMaterial:Load: " << path;

	autoSaver.pause();

	// Load
	{
		//gui.loadFromFile(path);
		ofxSurfing::loadSettings(parameters, path);
	}

	autoSaver.start();
}