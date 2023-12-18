
/*
* SurfingTransformNode.h
* 
	A class to handle the transforms for an object.
	Position, rotation, scale etc..
	To be applied easily and to store settings
	and apply transformation when drawing.
*/

// OPTIONAL:
#define SURFING__PBR__USE_MODELS_TRANSFORM_NODES
// Set commented to use a simpler transform mode,
// with y postion and rotation only.

//--

#pragma once

#include "ofMain.h"

#include "ofxSurfingPBRConstants.h"

//--

#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
class TransformNode {
public:
	ofParameter<bool> bEnable { "Enable", true };
	ofParameter<int> scalePow { "ScalePow", 0, -100, 100 };
	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<glm::vec3> position { "Position", glm::vec3(0),
		glm::vec3(-1), glm::vec3(1) };
	ofParameter<glm::vec3> rotation { "Rotation", glm::vec3(0),
		glm::vec3(-180), glm::vec3(180) };
	ofParameter<void> vReset { "Reset" };

	ofParameterGroup parameters {
		"Transform",
		bEnable, scalePow, scale, position, rotation, vReset
	};

	#if 0
	ofEventListener e_vReset;
	//TODO: this make fails some instances.. 
	// make needs emplace_back or unique_ptr?  
	//Severity	Code	Description	Project	File	Line	Suppression State	Details
	//Error C2280 'TransformNode::TransformNode(const TransformNode &)' : attempting to reference a deleted function 2_Example_Models K :\Documents\of_12\openFrameworks\addons\ofxSurfingPBR\src\SurfingFilesBrowserModels.h 156
	#endif

	TransformNode() {
	#if 0
		e_vReset = vReset.newListener([this](void) {
			reset();
		});
	#endif
	}

	~TransformNode() { }

	//----

	// TODO:
	// Bounding box.
	//
	//#define SURFING__WIP_BOUNDING_BOX
	//
	// Receive a passed ptr of the mesh or vector mesh
	// that we are "associating" to this node
	// and help calculate and draw the bounding box corners.

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

	//--

	// Get the transforms for each model
	// passing the model index as argument:

public:
	bool isEnabled() const {
		return bEnable;
	}

	float getScale() const {
		return scale;
	}

	float getScalePow() const {
		return scalePow;
	}

	glm::vec3 getPosition() const {
		return position;
	}

	glm::vec3 getRotation() const {
		return rotation;
	}

	void reset() {
		scalePow = 0;
		scale = 0;
		position = glm::vec3(0);
		rotation = glm::vec3(0);
	}

	//--

public:
	//#ifndef SURFING__PBR__SCENE_SIZE_UNIT
	//	#define SURFING__PBR__SCENE_SIZE_UNIT 1000.f
	//#endif

	void update() {
		// define min/max or de-normalized ranges unit
		float szUnit = SURFING__PBR__SCENE_SIZE_UNIT; //size unit
		float sUnit = szUnit; //scale unit
		const float dUnit = szUnit / 2.f; //distance unit
		const float sPow = scalePow; //scale power
		const float rMax = 360; //rotation max/min. could be 180...

		if (sPow == 0) {
		} else if (sPow < 1) {
			sUnit = sUnit / (float)abs(sPow - 1);
		} else if (sPow > 1) {
			sUnit = sUnit * (float)abs(sPow + 1);
		}

		float s = ofMap(scale, -1, 1, 1.f / sUnit, sUnit, true);

		float x = ofMap(position.get().x, -1, 1, -dUnit, dUnit, true);
		float y = ofMap(position.get().y, -1, 1, -dUnit, dUnit, true);
		float z = ofMap(position.get().z, -1, 1, -dUnit, dUnit, true);

		float rx = rotation.get().x;
		float ry = rotation.get().y;
		float rz = rotation.get().z;

		ofTranslate(x, y, z);
		ofScale(s, s, s);
		ofRotateXDeg(rx);
		ofRotateYDeg(ry);
		ofRotateZDeg(rz);
	}
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

	//--

	float getPositionY() const {
		return yPos;
	}

	float getRotationY() const {
		return = yRot;
	}

	void reset() {
		scalePow = 0;
		scale = 0;
		yPos = 0;
		yRot = 0;
	}

	//--

	void update() {
		// define min/max or de-normalized ranges
		float sUnit = SURFING__PBR__SCENE_SIZE_UNIT; //size unit
		const float dUnit = sUnit / 2.f; //distance unit
		const float sPow = scalePow; //scale power
		const float rMax = 360; //rotation max/min

		if (sPow == 0) {
		} else if (sPow < 1) {
			sUnit = sUnit / (float)abs(sPow - 1);
		} else if (sPow > 1) {
			sUnit = sUnit * (float)abs(sPow + 1);
		}

		float s = ofMap(scale, -1, 1, 1.f / sUnit, sUnit, true);
		float y = ofMap(yPos, -1, 1, -dUnit, dUnit, true);
		float r = ofMap(yRot, -1, 1, -rMax, rMax, true);

		ofTranslate(0, y, 0);
		ofScale(s, s, s);
		ofRotateYDeg(r);
	}
};
#endif