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
	}

	//--

	surfingLights.setup();

	//--

	parameters.setName("SCENE_MANAGER");
	parameters.add(surfingLights.bGui);

	parametersMaterials.setName("Materials");
	parametersColors.setName("Colors");

	parameters.add(parametersMaterials);
	parameters.add(parametersColors);

	gui.setup(parameters);
}

void SurfingSceneManager::update() {
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
}