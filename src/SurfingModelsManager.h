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
	SurfingFilesBrowserModels filesBrowserModels;

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

		filesBrowserModels.setFileExtensions(extensions);
		filesBrowserModels.setTitle("MODEL");
		filesBrowserModels.setup("models"); //set path for files

		//--

		// Callback to trig the model file loading.
		// The model path is ready on filesBrowserModels.pathFile!
		listenerLoadModel = filesBrowserModels.vLoad.newListener([this](void) {
			// load has been triggered

			size_t i = filesBrowserModels.getIndexFile();

			if (models[i]->hasAnimations()) { // an animated model
				int numAnims = models[i]->getAnimationCount();

				filesBrowserModels.indexAnimation.setWithoutEventNotifications(0);
				filesBrowserModels.indexAnimation.setMin(0);
				filesBrowserModels.indexAnimation.setMax(numAnims - 1);

				models[i]->setLoopStateForAllAnimations(OF_LOOP_NORMAL);
				models[i]->getAnimation(filesBrowserModels.indexAnimation.get()).play();
			} else { // a not animated model
				filesBrowserModels.indexAnimation.set(-1);
				filesBrowserModels.indexAnimation.setMin(-1);
				filesBrowserModels.indexAnimation.setMin(-1);
			}
		});

		indexFile.makeReferenceTo(filesBrowserModels.indexFile);

		listenerIndexModel = filesBrowserModels.indexFile.newListener([this](int & i) {
			// index model changed
		});

		listenerIndexAnim = filesBrowserModels.indexAnimation.newListener([this](int & i) {
			// index animation changed
			doLoadAnim(i);
		});

		//--

		loadModels();
	}

private:
	void loadModels() {
		string p = filesBrowserModels.getPathModels();

		ofLogNotice("SurfingModelsManager") << "loadModels(" << p << ")";
		ofLogNotice("SurfingModelsManager") << "Trying to load all the models files from the folder now.";

		models.clear();
		meshesModels.clear();

		size_t sz = filesBrowserModels.getAmountFiles();

		for (size_t i = 0; i < sz; i++) {

			// Models
			string path = filesBrowserModels.getPathModel(i);
			if (path == "") {
				ofLogError("SurfingModelsManager") << "Model path not settled properly or unknown!";
				continue;
			}

			std::unique_ptr<ofxAssimpModelLoader> m = std::make_unique<ofxAssimpModelLoader>();
			bool b = m->load(path, ofxAssimpModelLoader::OPTIMIZE_DEFAULT);

			if (m->hasAnimations()) {
				filesBrowserModels.indexAnimation = 0;
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
				string n = filesBrowserModels.getFilename(i);
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
		size_t i = filesBrowserModels.getIndexFile();
		if (i < meshesModels.size()) {

			models[i]->update();

			if (models[i]->hasAnimations()) {
				auto m = models[i]->getCurrentAnimatedMesh(0);
				meshesModels[i].clear(); //slow?
				meshesModels[i].push_back(m);
			}
		}

		filesBrowserModels.updateAutoSwitch();
	}

public:
	void doLoadAnim(int index) {
		if (index < 0) return;
		if (index > filesBrowserModels.indexAnimation.getMax()) return;

		size_t i = filesBrowserModels.getIndexFile();
		if (i > models.size() - 1) return;

		if (filesBrowserModels.indexAnimation.get() != index)
			filesBrowserModels.indexAnimation.setWithoutEventNotifications(index);

		if (models[i]->hasAnimations()) {
			models[i]->stopAllAnimations();

			int j = filesBrowserModels.indexAnimation.get();
			int numAnims = models[i]->getAnimationCount();

			filesBrowserModels.indexAnimation.setMin(0);
			filesBrowserModels.indexAnimation.setMax(numAnims - 1);

			j %= numAnims;
			j = ofClamp(j, filesBrowserModels.indexAnimation.getMin(), filesBrowserModels.indexAnimation.getMax());
			filesBrowserModels.indexAnimation.set(j);

			models[i]->getAnimation(filesBrowserModels.indexAnimation.get()).play();
		}
	}

	void doNextAnim() {
		size_t i = filesBrowserModels.getIndexFile();
		if (models[i]->hasAnimations()) {
			models[i]->stopAllAnimations();

			int j = filesBrowserModels.indexAnimation.get();
			j++;
			j %= models[i]->getAnimationCount();
			j = ofClamp(j, filesBrowserModels.indexAnimation.getMin(), filesBrowserModels.indexAnimation.getMax());

			filesBrowserModels.indexAnimation.set(j);
			models[i]->getAnimation(filesBrowserModels.indexAnimation).play();
		}
	}

private:
	void updateTransform() {
		const float yUnit = 500;
		const float scalePow = filesBrowserModels.getTransformScalePow();
		float scaleUnit = 1000;

		if (scalePow == 0) {
		} else if (scalePow < 1) {
			scaleUnit = scaleUnit / (float)abs(scalePow - 1);
		} else if (scalePow > 1) {
			scaleUnit = scaleUnit * (float)abs(scalePow + 1);
		}

		//const float dmax = 180;
		const float dmax = 360;
		float y = ofMap(filesBrowserModels.getTransformPosY(), -1, 1, -yUnit, yUnit, true);
		float s = ofMap(filesBrowserModels.getTransformScale(), -1, 1, 1.f / scaleUnit, scaleUnit, true);
		float r = ofMap(filesBrowserModels.getTransformRotY(), -1, 1, -dmax, dmax, true);

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

		//--

//TODO:
#define FIX_FACES 1
#if FIX_FACES
		// Push
		GLint frontFaceMode;
		// workaround trick to fix a model mesh normals!
		string n = filesBrowserModels.getFilename();
		if (n != "ofLogoHollow.ply") // exclude models from the fix
		{
			//TODO: fix  for "transparent" for model
			// head25k.obj bc normals problems..
			//glFrontFace(GL_CCW);

			glGetIntegerv(GL_FRONT_FACE, &frontFaceMode); // Save current state
			glFrontFace(GL_CCW); // Change state
		}
#endif

		if (!filesBrowserModels.bModeAll.get()) {
			// Draw selected model only
			// all their queued meshes.
			// Pick the selected model.
			size_t i = filesBrowserModels.getIndexFile();
			{
				for (size_t j = 0; j < meshesModels[i].size(); j++) {
					meshesModels[i][j].drawFaces();
				}
			}
		}
		
		else {
			// Draw all the models:
			// all their queued meshes.
			for (size_t i = 0; i < meshesModels.size(); i++) {
				for (size_t j = 0; j < meshesModels[i].size(); j++) {
					meshesModels[i][j].drawFaces();
				}
			}
		}

		//--

#if FIX_FACES
		// Pop
		if (n != "ofLogoHollow.ply") // exclude models from the fix
			glFrontFace(frontFaceMode); // Restore saved state
#endif
	}

private:
	// for animated model
	bool bAnimate;
	float animationPosition;

public:
	void drawGui() {
		filesBrowserModels.drawGui();
	}

	ofxPanel & getGui() {
		return filesBrowserModels.gui;
	}

	void drawHelp() {
		filesBrowserModels.drawHelp();
	}

	void setGuiPosition(glm::vec2 p) {
		filesBrowserModels.setGuiPosition(p);
	}

	void keyPressed(int key) {
		if (key == OF_KEY_DOWN) {
			filesBrowserModels.next();
		} else if (key == OF_KEY_UP) {
			filesBrowserModels.previous();
		}
	}
};
