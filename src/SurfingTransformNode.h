
/*
* SurfingTransformNode.h
* 
	A class to handle the transforms for an object.
	Position, rotation, scale etc..
	To be applied easily and to store settings
	and apply transformation when drawing.
*/

//--

#pragma once

#include "ofMain.h"

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
	// Bounding box

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