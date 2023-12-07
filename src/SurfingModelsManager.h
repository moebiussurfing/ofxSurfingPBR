/*
* SurfingModelsManager.h
* 
	A class to improve the other SurfingModelsFilesBrowser.h class
	and render the browsed/named model names, to load them and draw.
	Also includes some transformations like y position, scale or y rotation.
*/

//--

#pragma once

#include "ofMain.h"

#include "SurfingModelsFilesBrowser.h"
#include "ofVboMesh.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSurfingPBRConstants.h"

class SurfingModelsManager {
public:
	SurfingModelsManager() {
		ofLogNotice("ofxSurfingPBR") << "SurfingModelsManager:SurfingModelsManager()";
	}

	~SurfingModelsManager() {
		ofLogNotice("ofxSurfingPBR") << "SurfingModelsManager:~SurfingModelsManager()";
	}

private:
	SurfingModelsFilesBrowser filesBrowser;

	ofEventListener listenerLoadModel;
	ofEventListener listenerIndexModel;
	ofEventListener listenerIndexAnim;

	vector<std::unique_ptr<ofxAssimpModelLoader>> models;
	vector<vector<ofVboMesh>> meshesModels;

public:
	ofParameter<int> indexFile;
	// exposed/referenced to link a callback
	// on the parent scope to be notified
	// when index from SurfingModelsFilesBrowser changed.

	void setup() {

		filesBrowser.setup();

		//--

		// Callback to trig the model file loading.
		// The model path is ready on filesBrowser.pathModel!
		listenerLoadModel = filesBrowser.vLoad.newListener([this](void) {
			// load has been triggered

			size_t i = filesBrowser.getIndexFile();

			if (models[i]->hasAnimations()) { // an animated model
				int numAnims = models[i]->getAnimationCount();

				filesBrowser.indexAnimation.setWithoutEventNotifications(0);
				filesBrowser.indexAnimation.setMin(0);
				filesBrowser.indexAnimation.setMax(numAnims - 1);

				models[i]->setLoopStateForAllAnimations(OF_LOOP_NORMAL);
				models[i]->getAnimation(filesBrowser.indexAnimation.get()).play();
			} else { // a not animated model
				filesBrowser.indexAnimation.set(-1);
				filesBrowser.indexAnimation.setMin(-1);
				filesBrowser.indexAnimation.setMin(-1);
			}
		});

		indexFile.makeReferenceTo(filesBrowser.indexFile);

		listenerIndexModel = filesBrowser.indexFile.newListener([this](int & i) {
			// index model changed
		});

		listenerIndexAnim = filesBrowser.indexAnimation.newListener([this](int & i) {
			// index animation changed
			doLoadAnim(i);
		});

		//--

		loadModels();
	}

private:
	void loadModels() {
		string p = filesBrowser.getPathModels();

		ofLogNotice("SurfingModelsManager") << "loadModels(" << p << ")";
		ofLogNotice("SurfingModelsManager") << "Trying to load all the models files from the folder now.";

		models.clear();
		meshesModels.clear();

		size_t sz = filesBrowser.getAmountFiles();

		for (size_t i = 0; i < sz; i++) {

			// Models
			string path = filesBrowser.getPathModel(i);
			if (path == "") {
				ofLogError("SurfingModelsManager") << "Model path not settled properly or unknown!";
				continue;
			}

			std::unique_ptr<ofxAssimpModelLoader> m = std::make_unique<ofxAssimpModelLoader>();
			bool b = m->load(path, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);

			if (m->hasAnimations()) {
				filesBrowser.indexAnimation = 0;
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
				string n = filesBrowser.getFilename(i);
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
	void updateAnim() {
		size_t i = filesBrowser.getIndexFile();
		if (i < meshesModels.size()) {

			models[i]->update();

			if (models[i]->hasAnimations()) {
				auto m = models[i]->getCurrentAnimatedMesh(0);
				meshesModels[i].clear(); //slow?
				meshesModels[i].push_back(m);
			}
		}

		filesBrowser.updateAutoSwitch();
	}

public:
	void doLoadAnim(int index) {
		if (index < 0) return;
		if (index > filesBrowser.indexAnimation.getMax()) return;

		size_t i = filesBrowser.getIndexFile();
		if (i > models.size() - 1) return;

		if (filesBrowser.indexAnimation.get() != index)
			filesBrowser.indexAnimation.setWithoutEventNotifications(index);

		if (models[i]->hasAnimations()) {
			models[i]->stopAllAnimations();

			int j = filesBrowser.indexAnimation.get();
			int numAnims = models[i]->getAnimationCount();

			filesBrowser.indexAnimation.setMin(0);
			filesBrowser.indexAnimation.setMax(numAnims - 1);

			j %= numAnims;
			j = ofClamp(j, filesBrowser.indexAnimation.getMin(), filesBrowser.indexAnimation.getMax());
			filesBrowser.indexAnimation.set(j);

			models[i]->getAnimation(filesBrowser.indexAnimation.get()).play();
		}
	}

	void doNextAnim() {
		size_t i = filesBrowser.getIndexFile();
		if (models[i]->hasAnimations()) {
			models[i]->stopAllAnimations();

			int j = filesBrowser.indexAnimation.get();
			j++;
			j %= models[i]->getAnimationCount();
			j = ofClamp(j, filesBrowser.indexAnimation.getMin(), filesBrowser.indexAnimation.getMax());

			filesBrowser.indexAnimation.set(j);
			models[i]->getAnimation(filesBrowser.indexAnimation).play();
		}
	}

private:
	void updateTransform() {
		const float yUnit = 500;
		const float scalePow = filesBrowser.getTransformScalePow();
		float scaleUnit = 1000;

		if (scalePow == 0) {
		} else if (scalePow < 1) {
			scaleUnit = scaleUnit / (float)abs(scalePow - 1);
		} else if (scalePow > 1) {
			scaleUnit = scaleUnit * (float)abs(scalePow + 1);
		}

		float y = ofMap(filesBrowser.getTransformPos(), -1, 1, -yUnit, yUnit, true);
		float s = ofMap(filesBrowser.getTransformScale(), -1, 1, 1.f / scaleUnit, scaleUnit, true);
		float r = ofMap(filesBrowser.getTransformRotY(), -1, 1, -180, 180, true);

		ofTranslate(0, y, 0);
		ofScale(s, s, s);
		ofRotateYDeg(r);
	}

public:
	void draw() {
		updateAnim();

		ofPushMatrix();
		{
			updateTransform();
			drawModel();
		}
		ofPopMatrix();
	}

private:
	void drawModel() {
		if (meshesModels.size() == 0) return;

#if 1
		// workaround trick to fix a model mesh normals!
		string n = filesBrowser.getFilename();
		if (n != "ofLogoHollow.ply") // exclude models from the fix
		{
			//TODO: fix  for "transparent" for model
			// head25k.obj bc normals problems..

			glFrontFace(GL_CCW);
		}
#endif

		if (!filesBrowser.bModeAll.get()) {
			// Draw selected model only
			// all their queued meshes.
			// Pick the selected model.
			size_t i = filesBrowser.getIndexFile();
			{
				for (size_t j = 0; j < meshesModels[i].size(); j++) {
					meshesModels[i][j].drawFaces();
				}
			}
		} else {
			// Draw all the models:
			// all their queued meshes.
			for (size_t i = 0; i < meshesModels.size(); i++) {
				for (size_t j = 0; j < meshesModels[i].size(); j++) {
					meshesModels[i][j].drawFaces();
				}
			}
		}
	}

private:
	// for animated model
	bool bAnimate;
	float animationPosition;

public:
	void drawGui() {
		filesBrowser.drawGui();
	}

	ofxPanel & getGui() {
		return filesBrowser.gui;
	}

	void drawHelp() {
		filesBrowser.drawHelp();
	}

	void setGuiPosition(glm::vec2 p) {
		filesBrowser.setGuiPosition(p);
	}

	void keyPressed(int key) {
		if (key == OF_KEY_DOWN) {
			filesBrowser.next();
		} else if (key == OF_KEY_UP) {
			filesBrowser.previous();
		}
	}
};
