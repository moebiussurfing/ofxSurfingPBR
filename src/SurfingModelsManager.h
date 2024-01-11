/*
* SurfingModelsManager.h
* 
	A class to improve the other SurfingFilesBrowser.h class
	and render the browsed/named model names, to load them and draw.
	Also includes some transformations like y position, scale or y rotation.
*/

//--

#pragma once

#include "ofMain.h"

#include "SurfingFilesBrowserModels.h"

#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"

class SurfingModelsManager {
public:
	SurfingModelsManager() {
		ofLogNotice("ofxSurfingPBR") << "SurfingModelsManager:SurfingModelsManager()";
	}

	~SurfingModelsManager() {
		ofLogNotice("ofxSurfingPBR") << "SurfingModelsManager:~SurfingModelsManager()";
	}

private:
	SurfingFilesBrowserModels modelsManager;

	ofEventListener listenerLoadModel;
	ofEventListener listenerIndexModel;
	ofEventListener listenerIndexAnim;

	vector<std::unique_ptr<ofxAssimpModelLoader>> models;
	vector<vector<ofVboMesh>> meshesModels;

public:
	ofParameter<int> indexFile;
	// exposed/referenced to link a callback
	// on the parent scope to be notified
	// when index from SurfingFilesBrowser changed.

	void setup() {
		//supported formats
		vector<string> extensions = {
			"ply",
			"fbx",
			"obj"
		};

		modelsManager.setFileExtensions(extensions);
		modelsManager.setTitle("MODEL");
		modelsManager.setup("models"); //set path for files. bin/data/models

		//--

		// Callback to trig the model file loading.
		// The model path is ready on modelsManager.pathFile!
		listenerLoadModel = modelsManager.vLoad.newListener([this](void) {
			// load has been triggered

			size_t i = modelsManager.getIndexFile();

			if (models[i]->hasAnimations()) { // an animated model
				int numAnims = models[i]->getAnimationCount();

				modelsManager.indexAnimation.setWithoutEventNotifications(0);
				modelsManager.indexAnimation.setMin(0);
				modelsManager.indexAnimation.setMax(numAnims - 1);

				models[i]->setLoopStateForAllAnimations(OF_LOOP_NORMAL);
				models[i]->getAnimation(modelsManager.indexAnimation.get()).play();
			} else { // a not animated model
				modelsManager.indexAnimation.set(-1);
				modelsManager.indexAnimation.setMin(-1);
				modelsManager.indexAnimation.setMin(-1);
			}
		});

		indexFile.makeReferenceTo(modelsManager.indexFile);

		listenerIndexModel = modelsManager.indexFile.newListener([this](int & i) {
			// index model changed
		});

		listenerIndexAnim = modelsManager.indexAnimation.newListener([this](int & i) {
			// index animation changed
			doLoadAnim(i);
		});

		//--

		loadModels();
	}

private:
	void loadModels() {
		string p = modelsManager.getPathFiles();

		ofLogNotice("SurfingModelsManager") << "loadModels(" << p << ")";
		ofLogNotice("SurfingModelsManager") << "Trying to load all the models files from the folder now.";

		models.clear();
		meshesModels.clear();

		size_t sz = modelsManager.getAmountFiles();

		for (size_t i = 0; i < sz; i++) {

			// Models
			string path = modelsManager.getPathFile(i);
			if (path == "") {
				ofLogError("SurfingModelsManager") << "Model path not settled properly or unknown!";
				continue;
			}

			std::unique_ptr<ofxAssimpModelLoader> m = std::make_unique<ofxAssimpModelLoader>();
			bool b = m->load(path, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);

			if (m->hasAnimations()) {
				modelsManager.indexAnimation = 0;
				m->setLoopStateForAllAnimations(OF_LOOP_NORMAL);
				m->getAnimation(0).play();
			}

			models.push_back(std::move(m));

			// Meshes
			// Add meshes to the vector of ofVboMesh's.
			// We queue multi meshes,
			// bc some models could have multiple parts.
			vector<ofVboMesh> meshes;
			size_t sz = models.back()->getMeshCount();

			for (int j = 0; j < sz; j++) {
				ofVboMesh vm = models.back()->getMesh(j);
#if 0
				// workaround trick to fix a model mesh normals!
				// flip the normals to fix ofLogoHollow.ply
				string n = modelsManager.getFilename(i);
				if (n == "ofLogoHollow.ply") {
					//vm.mergeDuplicateVertices();
					for (size_t k = 0; k < vm.getNumNormals(); k++) {
						vm.getNormals()[k] *= -1.f;
					}
				}
#endif
				meshes.push_back(vm);
			}
			meshesModels.push_back(meshes);

			ofLogNotice("SurfingModelsManager") << "Queued " << meshes.size() << " meshes into for the model from " << path;
		}

		ofLogNotice("SurfingModelsManager") << "Queued " << meshesModels.size() << " models meshes from " << p << " into meshesModels.";
	}

private:
	void updateAnim(int i = -1) {
		if (i == -1) i = modelsManager.getIndexFile();
		if (i > meshesModels.size() - 1) return;

		models[i]->update();

		if (models[i]->hasAnimations()) {
			auto m = models[i]->getCurrentAnimatedMesh(0);
			meshesModels[i].clear(); //slow?
			meshesModels[i].push_back(m);
		}
	}

public:
	void doLoadAnim(int index) {
		if (index < 0) return;
		if (index > modelsManager.indexAnimation.getMax()) return;

		size_t i = modelsManager.getIndexFile();
		if (i > models.size() - 1) return;

		if (modelsManager.indexAnimation.get() != index)
			modelsManager.indexAnimation.setWithoutEventNotifications(index);

		if (models[i]->hasAnimations()) {
			models[i]->stopAllAnimations();

			int j = modelsManager.indexAnimation.get();
			int numAnims = models[i]->getAnimationCount();

			modelsManager.indexAnimation.setMin(0);
			modelsManager.indexAnimation.setMax(numAnims - 1);

			j %= numAnims;
			j = ofClamp(j, modelsManager.indexAnimation.getMin(), modelsManager.indexAnimation.getMax());
			modelsManager.indexAnimation.set(j);

			models[i]->getAnimation(modelsManager.indexAnimation.get()).play();
		}
	}

	void doNextAnim() {
		size_t i = modelsManager.getIndexFile();
		if (models[i]->hasAnimations()) {
			models[i]->stopAllAnimations();

			int j = modelsManager.indexAnimation.get();
			j++;
			j %= models[i]->getAnimationCount();
			j = ofClamp(j, modelsManager.indexAnimation.getMin(), modelsManager.indexAnimation.getMax());

			modelsManager.indexAnimation.set(j);
			models[i]->getAnimation(modelsManager.indexAnimation).play();
		}
	}

	//--

public:
	void draw() {
		drawModel();
	}

private:
	void drawModel() {
		if (meshesModels.size() == 0) {
			return;
		}

		//--

		// Mode A
		// Draw selected model only
		// all their queued meshes.
		if (!modelsManager.bModeAll.get()) {

			// Pick the selected model.
			size_t i = modelsManager.getIndexFile();

			modelsManager.drawOfNodeIfDebug(i);

			if (!modelsManager.isEnabled()) return;

			modelsManager.getNode(i).transformGL();
			{
				updateAnim(i);

				// draw all the model meshes
				for (size_t j = 0; j < meshesModels[i].size(); j++) {
					meshesModels[i][j].drawFaces();
				}
			}
			modelsManager.getNode(i).restoreTransformGL();
		}

		else {
			// Mode B
			// Draw all the models:
			// all their queued meshes.
			// Iterate all the models, not only the selected!
			for (size_t i = 0; i < meshesModels.size(); i++) {
				modelsManager.drawOfNodeIfDebug(i);

				if (!modelsManager.isEnabled(i)) continue;

				modelsManager.getNode(i).transformGL();
				{
					updateAnim(i);

					// draw all the model meshes
					for (size_t j = 0; j < meshesModels[i].size(); j++) {
						meshesModels[i][j].drawFaces();
					}
				}
				modelsManager.getNode(i).restoreTransformGL();
			}
		}
	}

	//--

private:
	// for animated model
	bool bAnimate;
	float animationPosition;

public:
	void refreshGui() {
		modelsManager.refreshGui();
	}
	void drawGui() {
		modelsManager.drawGui();
	}

	ofxPanel & getGui() {
		return modelsManager.gui;
	}

	ofxPanel * getGuiPtr() {
		return &modelsManager.gui;
	}

	void drawHelp() {
		modelsManager.drawHelp();
	}

	void setGuiPosition(glm::vec2 p) {
		modelsManager.setGuiPosition(p);
	}

	void keyPressed(int key) {
		if (key == OF_KEY_DOWN) {
			modelsManager.doNext();
		} else if (key == OF_KEY_UP) {
			modelsManager.doPrevious();
		}
	}

	//--

	/*
	
	//TODO:
	// A workaround trick to fix a model mesh normals,
	// or non solid/transparent faces!

private:
#define FIX_FACES 0

	GLint frontFaceMode = 0;

	// Push frontFaceMode
	void pushFixFaces() {
#if FIX_FACES
		string n = modelsManager.getFilename();
		if (n != "ofLogoHollow.ply") // exclude models from the fix
		{
			//TODO: fix  for "transparent" for model
			// head25k.obj bc normals problems..
			//glFrontFace(GL_CCW);

			glGetIntegerv(GL_FRONT_FACE, &frontFaceMode); // Save current state
			glFrontFace(GL_CCW); // Change state
		}
#endif
	}

	//--

	// Pop frontFaceMode
	void popFixFaces() {
#if FIX_FACES
		string n = modelsManager.getFilename();
		if (n != "ofLogoHollow.ply") // exclude model/s from the fix
		{
			glFrontFace(frontFaceMode); // Restore saved state
		}
#endif
	}
	*/
};
