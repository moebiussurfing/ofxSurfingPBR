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
	SurfingModelsManager::SurfingModelsManager() {
	}

	SurfingModelsManager::~SurfingModelsManager() {
	}

private:
	SurfingModelsFilesBrowser filesBrowser;

	ofEventListener listenerLoadModel;
	ofEventListener listenerIndexModel;

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
			// load is triggered
			
			////string path = filesBrowser.pathModel;
			////this->loadModel(path);
		});

		indexFile.makeReferenceTo(filesBrowser.indexFile);

		listenerIndexModel = filesBrowser.indexFile.newListener([this](int & i) {
			// index changed
		});

		//--

		loadModels();
	}

private:
	void loadModels() {
		string p = filesBrowser.getPathModels();

		ofLogNotice("SurfingModelsManager") << "loadModels(" << p << ")";
		ofLogNotice("SurfingModelsManager") << "Trying to load models files from the folder.";

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
			models.push_back(std::move(m));

			// Meshes
			// Add meshes to the vector of ofVboMesh's.
			// We queue multi meshes,
			// bc some models could have multiple parts.
			vector<ofVboMesh> meshes;
			size_t sz = models.back()->getMeshCount();
			for (int j = 0; j < sz; j++) {
				ofVboMesh vm = models.back()->getMesh(j);
				meshes.push_back(vm);
			}
			meshesModels.push_back(meshes);

			ofLogNotice("SurfingModelsManager") << "Queued " << meshes.size() << " meshes into for the model from " << path;
		}

		ofLogNotice("SurfingModelsManager") << "Queued " << meshesModels.size() << " models meshes from " << p << " into meshesModels.";
	}

public:
	void draw() {
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
		float r = ofMap(filesBrowser.getTransformRot(), -1, 1, -180, 180, true);

		ofPushMatrix();
		ofTranslate(0, y, 0);
		ofScale(s, s, s);
		ofRotateYDeg(r);
		{
			drawModel();
		}
		ofPopMatrix();
	}

private:
	void drawModel() {
		if (meshesModels.size() == 0) return;

		glFrontFace(GL_CCW);
		//TODO: fix for "transparent" for model head25k.obj bc normals problems..

		// Draw all the models: their queued meshes.
		size_t i = filesBrowser.getIndexFile();
		{
			for (size_t j = 0; j < meshesModels[i].size(); j++) {
				meshesModels[i][j].drawFaces();
			}
		}
	}

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
