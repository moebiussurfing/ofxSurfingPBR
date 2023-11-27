#include "SurfingSceneManager.h"

SurfingSceneManager::SurfingSceneManager() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:SurfingSceneManager()";

	clearColors();
	clearMaterials();
}

SurfingSceneManager ::~SurfingSceneManager() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:~SurfingSceneManager()";
}

void SurfingSceneManager::clearColors() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:clearColors()";

	colors.clear();
	parametersColors.clear();
}

void SurfingSceneManager::clearMaterials() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:clearMaterials()";

	materials.clear();
	parametersMaterials.clear();
}

void SurfingSceneManager::setFunctionRenderScene(callback_t f) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setFunctionRenderScene()";

	clearColors();
	clearMaterials();

	if (f_RenderScene == nullptr) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:setFunctionRenderScene(). Wrong callback_t";
	}
	f_RenderScene = f;

	surfingLights.setFunctionRenderScene(f_RenderScene);
}

void SurfingSceneManager::setup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup()";

	if (materials.size() == 0) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:Note that you need to add the materials before calling setup()!";
	}if (colors.size() == 0) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:Note that you need to add the colors before calling setup()!";
	}

	//--

	surfingLights.setup();

	//--

	parametersMaterials.setName("Materials");
	parametersColors.setName("Colors");

	parameters.setName("SCENE_MANAGER");
	parameters.add(surfingLights.bGui);
	parameters.add(parametersMaterials);
	parameters.add(parametersColors);

	gui.setup(parameters);

	ofxSurfing::loadSettings(parameters, path);

	//--

	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() added " << colors.size() << " colors";
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() added " << materials.size() << " materials";
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() Done";
}

void SurfingSceneManager::update() {
	surfingLights.updateLights();
}

void SurfingSceneManager::draw() {
}

void SurfingSceneManager::drawGui() {
	gui.draw();

	if (surfingLights.bGui)
		ofxSurfing::setGuiPositionRightTo(surfingLights.gui, gui);
	surfingLights.drawGui();

	for (size_t i = 0; i < materials.size(); i++) {
		materials[i]->drawGui();
	}
}

void SurfingSceneManager::addMaterial(string name) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:addMaterial(" << name << ")";

	int i = materials.size();
	string s = "Material_" + ofToString(i);
	std::unique_ptr<SurfingMaterial> m;
	m = std::make_unique<SurfingMaterial>();
	m->setup(s);
	parametersMaterials.add(m->parameters);
	materials.push_back(std::move(m));
}

void SurfingSceneManager::addColor(ofFloatColor color) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:addColor(" << color << ")";

	int i = colors.size();
	string s = "Color_" + ofToString(i);
	std::unique_ptr<ofParameter<ofFloatColor>> m;
	m = std::make_unique<ofParameter<ofFloatColor>>();
	m->set(s, color, ofFloatColor(0, 0), ofFloatColor(1, 1));
	parametersColors.add(*m);
	colors.push_back(std::move(m));
}

void SurfingSceneManager::setColor(ofFloatColor color, int index) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setColor(" << index << ")";

	if (materials.size() == 0) return;

	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->setColorGlobal(color);
}

void SurfingSceneManager::setColorMaterial(ofFloatColor color, int index) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setColorMaterial(" << color << ":" << index;

	if (materials.size() == 0) return;
	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->setColorGlobal(color);
}

void SurfingSceneManager::beginMaterial(int index) {
	if (materials.size() == 0) return;
	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->begin();
}

void SurfingSceneManager::endMaterial(int index) {
	if (materials.size() == 0) return;
	size_t i = index;
	if (i > materials.size() - 1) return;

	materials[i]->end();
}

void SurfingSceneManager::beginLights() {
	surfingLights.begin();
}

void SurfingSceneManager::endLights() {
	surfingLights.end();
}

void SurfingSceneManager::exit() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:exit()";

	ofxSurfing::saveSettings(parameters, path);
}