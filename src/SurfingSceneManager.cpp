#include "SurfingSceneManager.h"

//--------------------------------------------------------------
SurfingSceneManager::SurfingSceneManager() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:SurfingSceneManager()";

	clearColors();
	clearMaterials();
}

//--------------------------------------------------------------
SurfingSceneManager ::~SurfingSceneManager() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:~SurfingSceneManager()";
}

//--------------------------------------------------------------
void SurfingSceneManager::clearColors() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:clearColors()";

	colors.clear();
	colorsParams.clear();
}

//--------------------------------------------------------------
void SurfingSceneManager::clearMaterials() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:clearMaterials()";

	materials.clear();
	materialsParams.clear();
}

//--------------------------------------------------------------
void SurfingSceneManager::setFunctionRenderScene(callback_t f) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setFunctionRenderScene()";

	clearColors();
	clearMaterials();

	if (f_RenderScene == nullptr) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:setFunctionRenderScene(). Wrong callback_t";
	}
	f_RenderScene = f;

	lights.setFunctionRenderScene(f_RenderScene);
}

//--------------------------------------------------------------
void SurfingSceneManager::setupBuild() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setupBuild()";

	if (materials.size() == 0) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:Note that you need to add the materials before calling setup()!";
	}
	if (colors.size() == 0) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:Note that you need to add the colors before calling setup()!";
	}

	//--

	lights.setup();

	//--

	setupParams();

	setupGui();

	//--

	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() added " << colors.size() << " colors";
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() added " << materials.size() << " materials";
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() Done";

	//--

	startup();
}

//--------------------------------------------------------------
void SurfingSceneManager::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setupParams()";

	//--

	bGui_Materials.set("Materials", true);
	bGui_Colors.set("Colors", false);

	nameIndexMaterial.set("M Name", "");
	nameIndexColor.set("C Name", "");
	nameIndexMaterial.setSerializable(false);
	nameIndexColor.setSerializable(false);

	indexMaterial.set("Select Material", -1, -1, -1);
	if (materials.size() > 0) {
		indexMaterial.setMax(materials.size() - 1);
		indexMaterial.setMin(0);
		indexMaterial.set(0);
	}

	indexColor.set("Select Color", -1, -1, -1);
	if (colors.size() > 0) {
		indexColor.setMax(colors.size() - 1);
		indexColor.setMin(0);
		indexColor.set(0);
	}

	parameters.setName("SCENE_MANAGER");
	parameters.add(lights.bGui);
	parameters.add(bGui_Materials);
	parameters.add(indexMaterial);
	parameters.add(nameIndexMaterial);
	parameters.add(bGui_Colors);
	parameters.add(indexColor);
	parameters.add(nameIndexColor);

	materialsParams.setName("Materials");
	listenerIndexMaterial = indexMaterial.newListener([this](int & i) {
		if (i < materials.size()) {
			string n = materials[i]->getName();
			nameIndexMaterial.set(n);
		}

		refreshGui();
	});

	colorsParams.setName("Colors");
	listenerIndexColor = indexColor.newListener([this](int & i) {
		if (i < colors.size()) {
			string n = colors[i]->getName();
			nameIndexColor.set(n);
		}
		
		refreshGui();
	});
}

//--------------------------------------------------------------
void SurfingSceneManager::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:startup()";

	indexMaterial = indexMaterial;
	indexColor = indexColor;

	ofxSurfing::loadSettings(parameters, path);
}

//--------------------------------------------------------------
void SurfingSceneManager::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setupGui()";

	gui.setup(parameters);
	guiMaterials.setup(materialsParams);
	guiColors.setup(colorsParams);

	refreshGui();
}

//--------------------------------------------------------------
void SurfingSceneManager::refreshGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:refreshGui()";

	// maximize selected index only for materials and colors

	for (size_t i = 0; i < materials.size(); i++) {
		auto & g = guiMaterials.getGroup(materials[i]->getName());

		if (i == indexMaterial.get()) {
			g.maximize();

			g.getGroup(materials[i]->settingsParams.getName())
				.getGroup(materials[i]->coatParams.getName())
				.minimize();
			g.getGroup(materials[i]->colorParams.getName()).minimize();
			g.getGroup(materials[i]->moreParams.getName()).minimize();
			g.getGroup(materials[i]->globalParams.getName())
				.getGroup(materials[i]->globalLinksParams.getName())
				.minimize();
		} else {
			g.minimize();
		}
	}

	for (size_t i = 0; i < colors.size(); i++) {
		auto & g = guiColors.getGroup(colors[i]->getName());

		if (i == indexColor.get()) {
			g.maximize();
		} else {
			g.minimize();
		}
	}
}

//--------------------------------------------------------------
void SurfingSceneManager::updateLights() {
	lights.updateLights();
}

//--------------------------------------------------------------
void SurfingSceneManager::draw() {
}

//--------------------------------------------------------------
void SurfingSceneManager::drawGui() {
	gui.draw();

	if (lights.bGui) {
		ofxSurfing::setGuiPositionRightTo(lights.gui, gui);
		lights.drawGui();
	}

	if (bGui_Materials) {
		ofxSurfing::setGuiPositionRightTo(guiMaterials, lights.bGui ? lights.gui : gui);
		guiMaterials.draw();
	}

	if (bGui_Colors) {
		ofxSurfing::setGuiPositionRightTo(guiColors, guiMaterials);
		guiColors.draw();
	}
}

//--------------------------------------------------------------
void SurfingSceneManager::addMaterial(string name) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:addMaterial(" << name << ")";

	int i = materials.size();
	string s = "";
	if (name == "")
		s += "M_" + ofToString(i);
	else
		s += name;
	std::unique_ptr<SurfingMaterial> m;
	m = std::make_unique<SurfingMaterial>();
	m->setup(s);
	materialsParams.add(m->parameters);
	materials.push_back(std::move(m));
}

//--------------------------------------------------------------
void SurfingSceneManager::addColor(ofFloatColor color, string name) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:addColor(" << color << ")";

	int i = colors.size();
	string s = "";
	if (name == "")
		s += "C_" + ofToString(i);
	else
		s += name;
	std::unique_ptr<ofParameter<ofFloatColor>> m;
	m = std::make_unique<ofParameter<ofFloatColor>>();
	m->set(s, color, ofFloatColor(0, 0), ofFloatColor(1, 1));
	colorsParams.add(*m);
	colors.push_back(std::move(m));
}

//--------------------------------------------------------------
void SurfingSceneManager::setColor(ofFloatColor color, int index) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setColor(" << index << ")";

	if (materials.size() == 0) return;

	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->setColorGlobal(color);
}

//--------------------------------------------------------------
void SurfingSceneManager::setColorMaterial(ofFloatColor color, int index) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setColorMaterial(" << color << ":" << index;

	if (materials.size() == 0) return;
	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->setColorGlobal(color);
}

//--------------------------------------------------------------
void SurfingSceneManager::beginMaterial(int index) {
	if (materials.size() == 0) return;
	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->begin();
}

//--------------------------------------------------------------
void SurfingSceneManager::endMaterial(int index) {
	if (materials.size() == 0) return;
	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->end();
}

//--------------------------------------------------------------
void SurfingSceneManager::beginLights() {
	lights.begin();
}

//--------------------------------------------------------------
void SurfingSceneManager::endLights() {
	lights.end();
}

//--------------------------------------------------------------
void SurfingSceneManager::drawDebugLights() {
	lights.drawDebugLights();
}

//--------------------------------------------------------------
void SurfingSceneManager::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:exit()";

	ofxSurfing::saveSettings(parameters, path);
}

//--------------------------------------------------------------
void SurfingSceneManager::toggleDebug() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:toggleDebug()";

	lights.bDebug = !lights.bDebug;
}