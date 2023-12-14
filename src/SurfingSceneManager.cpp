#include "SurfingSceneManager.h"

//--------------------------------------------------------------
SurfingSceneManager::SurfingSceneManager() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:SurfingSceneManager()";

	clearMaterials();

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	clearColors();
#endif
}

//--------------------------------------------------------------
SurfingSceneManager ::~SurfingSceneManager() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:~SurfingSceneManager()";

	if (!bDoneExit) {
		ofLogWarning("ofxSurfingPBR") << "SurfingSceneManager:destructor() Force calling exit() bc has not been called until now!";
		exit();
	}
}

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
//--------------------------------------------------------------
void SurfingSceneManager::clearColors() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:clearColors()";

	colors.clear();
	colorsParams.clear();
}
#endif

//--------------------------------------------------------------
void SurfingSceneManager::clearMaterials() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:clearMaterials()";

	materials.clear();
	materialsParams.clear();
}

//--------------------------------------------------------------
void SurfingSceneManager::setFunctionRenderScene(callback_t f) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setFunctionRenderScene()";

	clearMaterials();

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	clearColors();
#endif

	if (f_RenderScene == nullptr) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:setFunctionRenderScene(). Wrong callback_t nullptr";
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

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	if (colors.size() == 0) {
		ofLogError("ofxSurfingPBR") << "SurfingSceneManager:Note that you need to add the colors before calling setup()!";
	}
#endif

	//--

	lights.setup();

	//--

	setupParams();

	setupGui();

	//--

	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() added " << materials.size() << " materials";
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() Done";

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setup() added " << colors.size() << " colors";
#endif

	//--

	startup();
}

//--------------------------------------------------------------
void SurfingSceneManager::setupParams() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setupParams()";

	//--

	bGui_Materials.set("UI Materials", true);

	nameIndexMaterial.set("M Name", "");
	nameIndexMaterial.setSerializable(false);

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	bGui_Colors.set("UI Colors", false);
	nameIndexColor.set("C Name", "");
	nameIndexColor.setSerializable(false);
#endif

	indexMaterial.set("Select Material", -1, -1, -1);
	if (materials.size() > 0) {
		indexMaterial.setMax(materials.size() - 1);
		indexMaterial.setMin(0);
		indexMaterial.set(0);
	}

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	indexColor.set("Select Color", -1, -1, -1);
	if (colors.size() > 0) {
		indexColor.setMax(colors.size() - 1);
		indexColor.setMin(0);
		indexColor.set(0);
	}
#endif

	materialsControlParams.setName("Materials");
	materialsControlParams.add(indexMaterial);
	materialsControlParams.add(nameIndexMaterial);

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	colorsControlParams.setName("Colors");
	colorsControlParams.add(indexColor);
	colorsControlParams.add(nameIndexColor);
#endif

	parameters.setName("SCENE_MANAGER");
	parameters.add(lights.bGui);
	parameters.add(bGui_Materials);

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	parameters.add(bGui_Colors);
#endif
	parameters.add(materialsControlParams);

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	parameters.add(colorsControlParams);
#endif

	materialsParams.setName("Materials");
	listenerIndexMaterial = indexMaterial.newListener([this](int & i) {
		if (i < materials.size()) {
			string n = materials[i]->getName();
			nameIndexMaterial.set(n);
		}

		refreshGui();
	});

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	colorsParams.setName("Colors");
	listenerIndexColor = indexColor.newListener([this](int & i) {
		if (i < colors.size()) {
			string n = colors[i]->getName();
			nameIndexColor.set(n);
		}

		refreshGui();
	});
#endif

	listenerbGui_Materials = bGui_Materials.newListener([this](bool & b) {
		refreshGuiMaterials();
	});
}

//--------------------------------------------------------------
void SurfingSceneManager::startup() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:startup()";

	indexMaterial.set(indexMaterial.get());

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	indexColor.set(indexColor.get());
#endif

	ofxSurfing::loadSettings(parameters, path);
}

//--------------------------------------------------------------
void SurfingSceneManager::setupGui() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:setupGui()";

	gui.setup(parameters);
	guiMaterials.setup(materialsParams);

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	guiColors.setup(colorsParams);
#endif

	refreshGui(true);
}

//--------------------------------------------------------------
void SurfingSceneManager::refreshGuiMaterials() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:refreshGuiMaterials()";
	if (bGui_Materials)
		gui.getGroup(materialsControlParams.getName()).maximize();
	else
		gui.getGroup(materialsControlParams.getName()).minimize();
}

//--------------------------------------------------------------
void SurfingSceneManager::refreshGui(bool bHard) {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:refreshGui(" << bHard << ")";

	if (bHard) {
		gui.getGroup(materialsControlParams.getName()).minimize();

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
		gui.getGroup(colorsControlParams.getName()).minimize();
#endif
	}

	//--

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

	//TODO: implement usage
#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	for (size_t i = 0; i < colors.size(); i++) {
		auto & g = guiColors.getGroup(colors[i]->getName());

		if (i == indexColor.get()) {
			g.maximize();
		} else {
			g.minimize();
		}
	}
#endif
}

//--------------------------------------------------------------
void SurfingSceneManager::updateLights() {
	lights.updateLights();
}

//--------------------------------------------------------------
void SurfingSceneManager::draw() {
}

////--------------------------------------------------------------
//void SurfingSceneManager::drawGui(ofxPanel * guiPtr) {
//	if (guiPtr != nullptr) {
//		glm::vec3 p;
//		p = gui.getShape().getTopRight() + glm::vec2(SURFING__PAD_OFXGUI_BETWEEN_PANELS, 0);
//	}
//}

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

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
	if (bGui_Colors) {
		ofxSurfing::setGuiPositionRightTo(guiColors, guiMaterials);
		guiColors.draw();
	}
#endif
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

#ifndef SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
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
#endif

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

	bDoneExit = true;
}

//--------------------------------------------------------------
void SurfingSceneManager::toggleDebug() {
	ofLogNotice("ofxSurfingPBR") << "SurfingSceneManager:toggleDebug()";

	lights.bDebug = !lights.bDebug;
}