/*
	TODO

	- make aux transform params with makeRef
		to link to loaded object,
		then avoiding to expose all the object 
		transforms folders to the gui

	- add bool draw to each object to allow all models mode arrange.
*/

/*
* SurfingFilesBrowserModels.h
* 
	A class to help loading the many model files
	from a path directory (default bin/data/models/)
	Then allows browsing them and apply some basic transforms
	that are persistent.

	Transforms are stored as JSON paired to the model file name with a suffix.
	So we can add and remove more files to the path 
	without breaking persistent settings!

	Another parent (scope) class would be able to load files easily.
	It has help info and displays to debug selected browser,
	get the path to be passed to the model/mesh loaders..etc
*/

//--

#pragma once

#include "ofMain.h"

#include "SurfingFilesBrowser.h"

#define SURFING__PBR__USE_MODELS_TRANSFORM_NODES

//--

#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
class TransformNode {
public:
	ofParameter<bool> bEnable{ "Enable", true };

	ofParameter<int> scalePow { "ScalePow", 0, -100, 100 };
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };

	ofParameter<glm::vec3> position { "Position", glm::vec3(0), 
		glm::vec3(-1), glm::vec3(1) };

	ofParameter<glm::vec3> rotation { "Rotation", glm::vec3(0), 
		glm::vec3(-180), glm::vec3(180) };

	ofParameterGroup parameters {
		"Transform",
		bEnable, scalePow, scale, position, rotation
	};

	TransformNode() {
	}

	~TransformNode() {
	}

	//--

	//TODO:

	//#define SURFING__WIP_BOUNDING_BOX
	// bounding box

	#ifdef SURFING__WIP_BOUNDING_BOX
	//vector<MeshNode> meshNodes;

	glm::vec3 meshMin;
	glm::vec3 meshMax;
	glm::vec3 meshCentroid = { 0, 0, 0 }; // can be used in place of glm::vec3(0,0,0);

	void calculateVertexBounds(ofMesh * mesh) {
		min.x = std::numeric_limits<float>::max();
		max.x = -min.x;

		min.y = min.x;
		max.y = max.x;

		min.z = min.x;
		max.z = max.x;

		const auto & verts = mesh->getVertices();
		for (const auto & v : verts) {
			if (v.x > max.x) {
				max.x = v.x;
			}
			if (v.x < min.x) {
				min.x = v.x;
			}

			if (v.y > max.y) {
				max.y = v.y;
			}
			if (v.y < min.y) {
				min.y = v.y;
			}

			if (v.z > max.z) {
				max.z = v.z;
			}
			if (v.z < min.z) {
				min.z = v.z;
			}
		}
	}

	glm::vec3 min;
	glm::vec3 max;

	void drawBounds() {
		//if (!bDebug) return;
		//if (!bDrawBounds && !bDrawBBox) return;

		bool bDrawBBox = true;
		bool bDrawBounds = true;

		ofColor c;

		int a = 150;
		//int a = ofMap(glm::sin(ofGetElapsedTimef()), -1, 1, 0, 200);

		c = ofColor(255, a); //white
		//c = ofColor(0, a); //black

		ofPushStyle();
		{
			if (bDrawBBox) {
				ofPushMatrix();
				{
					ofTranslate(-meshCentroid);

					ofSetColor(c);

					drawBounds(meshMin, meshMax, 100);
				}
				ofPopMatrix();
			}

			//if (bDrawBounds) {
			//	for (auto & meshNode : meshNodes) {
			//		meshNode.node.transformGL();
			//		ofSetColor(c);
			//		drawBounds(meshNode.min, meshNode.max, 30);
			//		meshNode.node.restoreTransformGL();
			//	}
			//}
		}
		ofPopStyle();
	}

	void drawBounds(glm::vec3 min, glm::vec3 max, float size) {
		glm::vec3 up(0, size, 0);
		glm::vec3 right(size, 0, 0);
		glm::vec3 forward(0, 0, size);

		ofMesh drawMesh;
		drawMesh.setMode(OF_PRIMITIVE_LINES);

		glm::vec3 cornerPt = min;
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + forward);

		cornerPt = glm::vec3(min.x, min.y, max.z);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - forward);

		cornerPt = glm::vec3(min.x, max.y, min.z);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + forward);

		cornerPt = glm::vec3(min.x, max.y, max.z);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - forward);

		cornerPt = glm::vec3(max.x, min.y, min.z);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + forward);

		cornerPt = glm::vec3(max.x, min.y, max.z);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - forward);

		cornerPt = glm::vec3(max.x, max.y, min.z);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt + forward);

		cornerPt = glm::vec3(max.x, max.y, max.z);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - right);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - up);
		drawMesh.addVertex(cornerPt);
		drawMesh.addVertex(cornerPt - forward);

		drawMesh.draw();
	}

	glm::vec3 getBoundBoxShape() const {
		glm::vec3 p;
		p = (meshMax - meshMin);
		return p;
	}
	#endif
};

//--

#else
// simpler mode using only y position and rotation.
class TransformSimple {
public:
	ofParameter<int> scalePow { "ScalePow", 0, -100, 100 };
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<float> yPos { "y Pos", 0, -1.f, 1.f };
	ofParameter<float> yRot { "y Rot", 0, -1.f, 1.f };

	ofParameterGroup parameters {
		"Transform",
		scalePow, scale, yPos, yRot
	};

	TransformSimple() {
	}

	~TransformSimple() {
	}
};
#endif

//--

class SurfingFilesBrowserModels : public SurfingFilesBrowser {
public:
	SurfingFilesBrowserModels() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:SurfingFilesBrowserModels()";
	}

	~SurfingFilesBrowserModels() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:~SurfingFilesBrowserModels()";
	}

private:
	void setupGui() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:setupGui()";

		setupParamsTransforms();

		parameters.add(extraParams);
		parameters.add(bHelp);

		gui.setup(parameters);

		//gui.getGroup(transformParams.getName()).minimize();
		gui.getGroup(extraParams.getName()).minimize();
	}

	void setupParamsTransforms() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:setupParamsTransforms()";

		vReset.setName("Reset Model");

		transformParams.setName("Transforms");
		transformParams.add(vReset);

		transforms.clear();
		for (size_t i = 0; i < dir.size(); i++) {

#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
			TransformNode t = TransformNode();
#else
			TransformSimple t = TransformSimple();
#endif
			t.parameters.setName(getFilename(i));
			transforms.emplace_back(t);

			transformParams.add(t.parameters);
		}

		transformParams.setSerializable(false);

		parameters.add(transformParams);
	}

public:
	void doReset() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:doReset()";

		resetTransform();
	}

public:
	string extSuffixTransform = "__Transform.json";

	void saveTransforms() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:saveTransforms()";

		for (size_t i = 0; i < transforms.size(); i++) {
			ofJson j;
			string p = pathFiles + "\\" + getName(i) + extSuffixTransform;

			ofJson settings;
			ofSerialize(settings, transforms[i].parameters);
			bool b = ofSavePrettyJson(p, settings);
		}
	}

	void loadTransforms() {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:loadTransforms()";

		for (size_t i = 0; i < transforms.size(); i++) {
			string p = pathFiles + "\\" + getName(i) + extSuffixTransform;

			ofJson settings;
			settings = ofLoadJson(p);
			ofDeserialize(settings, transforms[i].parameters);
		}
	}

	void save() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:save()";

		ofxSurfing::saveSettings(parameters, pathSettings);

		saveTransforms();
	}

	void load() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:load()";

		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, pathSettings);
		autoSaver.start();

		loadTransforms();
	}

	//--

public:
	void refreshGui() override {
		ofLogNotice("ofxSurfingPBR") << "SurfingFilesBrowserModels:refreshGui()";

		for (size_t i = 0; i < transforms.size(); i++) {

#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
			TransformNode t = transforms[i];
#else
			TransformSimple t = transforms[i];
#endif
			string n = getFilename(i);
			bool b = (i == indexFile); //selected
			auto & g = gui.getGroup(transformParams.getName()).getGroup(n);
			b ? g.maximize() : g.minimize();

#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
			//g.getGroup(t.position.getName()).minimize();
			//g.getGroup(t.rotation.getName()).minimize();
			g.getGroup(t.position.getName()).maximize();
			g.getGroup(t.rotation.getName()).maximize();
#endif
		}
	}

	//--

	// Store each model transforms for gizmo.

private:
#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
	vector<TransformNode> transforms;
#else
	vector<TransformSimple> transforms;
#endif

	//--

	// Get the transforms for each model
	// passing the model index as argument:

public:
	bool isEnabled(int i = -1) const {
		bool b;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			b = transforms[i].bEnable;
		return b;
	}

	float getTransformScale(int i = -1) const {
		float v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].scale;
		return v;
	}

	float getTransformScalePow(int i = -1) const {
		int v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].scalePow;
		return v;
	}

#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
	void resetTransform(int i = -1) {
		if (i == -1) i = indexFile;
		if (i < transforms.size()) {
			transforms[i].scalePow = 0;
			transforms[i].scale = 0;
			transforms[i].position = glm::vec3(0);
			transforms[i].rotation = glm::vec3(0);
		}
	}

	glm::vec3 getTransformPosition(int i = -1) const {
		glm::vec3 v = glm::vec3(0);
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].position;
		return v;
	}

	glm::vec3 getTransformRotation(int i = -1) const {
		glm::vec3 v = glm::vec3(0);
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].rotation;
		return v;
	}

#else
	float getTransformPosY(int i = -1) const {
		float v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].yPos;
		return v;
	}

	float getTransformRotY(int i = -1) const {
		float v = 0;
		if (i == -1) i = indexFile;
		if (i < transforms.size())
			v = transforms[i].yRot;
		return v;
	}

	void resetTransform(int i = -1) {
		if (i == -1) i = indexFile;
		if (i < transforms.size()) {
			transforms[i].scalePow = 0;
			transforms[i].scale = 0;
			transforms[i].yPos = 0;
			transforms[i].yRot = 0;
		}
	}
#endif

	ofParameterGroup transformParams;
};