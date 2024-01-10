
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
// with y position and rotation only.

//--

#pragma once

#include "ofMain.h"

//OPTIONAL: comment the below line
// if you are using this class standalone,
// without the whole ofxSurfingPBR
#include "ofxSurfingPBRConstants.h"

#ifndef SURFING__PBR__SCENE_SIZE_UNIT
	#define SURFING__PBR__SCENE_SIZE_UNIT 1000.f
#endif

//--

#ifdef SURFING__PBR__USE_MODELS_TRANSFORM_NODES
class TransformNode : public ofNode {
public:
	ofParameter<bool> bEnable { "Enable", true };

	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<int> scalePow { "ScalePow", 0, -100, 100 };

private:
	float powRatio = 1.f;

public:
	void setPowRatio(float r) {
		powRatio = r;
	}

	ofParameter<glm::vec3> position { "Position", glm::vec3(0),
		glm::vec3(-1), glm::vec3(1) };

	ofParameter<glm::vec3> rotation { "Rotation", glm::vec3(0),
		glm::vec3(-180), glm::vec3(180) };

	ofParameter<void> vReset { "Reset" };
	ofParameter<void> vResetScale { "Reset Scale" };
	ofParameter<void> vResetPosition { "Reset Position" };
	ofParameter<void> vResetRotation { "Reset Rotation" };

	ofParameterGroup parameters;
	//ofParameterGroup parameters {
	//	"Transform",
	//	bEnable, scalePow, scale, position, rotation, vReset, vResetScale, vResetPosition, vResetRotation
	//};

	std::unique_ptr<ofEventListener> e_vReset;
	std::unique_ptr<ofEventListener> e_vResetScale;
	std::unique_ptr<ofEventListener> e_vResetRotation;
	std::unique_ptr<ofEventListener> e_vResetPosition;

	std::unique_ptr<ofEventListener> e_positionChanged;
	std::unique_ptr<ofEventListener> e_orientationChanged;
	std::unique_ptr<ofEventListener> e_scaleChanged;

	TransformNode() {
		setup();
	}

	TransformNode(const TransformNode & other)
		: bEnable(other.bEnable)
		, scalePow(other.scalePow)
		, powRatio(other.powRatio)
		, scale(other.scale)
		, position(other.position)
		, rotation(other.rotation)
		, vReset(other.vReset)
		, vResetScale(other.vResetScale)
		, vResetPosition(other.vResetPosition)
		, vResetRotation(other.vResetRotation)
		, parameters(other.parameters) {
		setup();
	}

	~TransformNode() { }

	void setup() {
		parameters.setName("Transform");
		parameters.add(bEnable);
		parameters.add(scalePow);
		parameters.add(scale);
		parameters.add(position);
		parameters.add(rotation);
		parameters.add(vReset);
		parameters.add(vResetScale);
		parameters.add(vResetPosition);
		parameters.add(vResetRotation);
		
		e_positionChanged = std::make_unique<ofEventListener>(position.newListener([this](glm::vec3) {
			const float dUnit = SURFING__PBR__SCENE_SIZE_UNIT; //distance unit
			float x = ofMap(position.get().x, -1, 1, -dUnit, dUnit, true);
			float y = ofMap(position.get().y, -1, 1, -dUnit, dUnit, true);
			float z = ofMap(position.get().z, -1, 1, -dUnit, dUnit, true);
			setPosition(glm::vec3(x,y,z));
		}));

		#if 0
		e_orientationChanged = std::make_unique<ofEventListener>(rotation.newListener([this](glm::vec3) {
			glm::vec3 rotationInRadians = glm::radians(rotation);
			glm::quat rotationQuatX = glm::angleAxis(rotationInRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat rotationQuatY = glm::angleAxis(rotationInRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat rotationQuatZ = glm::angleAxis(rotationInRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::quat finalRotationQuat = rotationQuatX * rotationQuatY * rotationQuatZ;
			setOrientation(finalRotationQuat);
		}));
		#endif

		//e_scaleChanged = std::make_unique<ofEventListener>(scale.newListener([this](glm::vec3) {
		//	//setScale(scale);
		//}));

		e_vReset = std::make_unique<ofEventListener>(vReset.newListener([this](void) {
			reset();
		}));

		e_vResetScale = std::make_unique<ofEventListener>(vResetScale.newListener([this](void) {
			resetScale();
		}));

		e_vResetPosition = std::make_unique<ofEventListener>(vResetPosition.newListener([this](void) {
			resetPosition();
		}));

		e_vResetRotation = std::make_unique<ofEventListener>(vResetRotation.newListener([this](void) {
			resetRotation();
		}));
	}

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
		resetScale();
		resetPosition();
		resetRotation();
	}
	void resetScale() {
		scalePow = 0;
		scale = 0;
	}
	void resetPosition() {
		position = glm::vec3(0);
		//position = glm::vec3(0, 0.1, 0);
	}
	void resetRotation() {
		rotation = glm::vec3(0);
	}

	//--

public:
	void push() {
		ofPushMatrix();
		update();
	}
	void pop() {
		ofPopMatrix();
	}

	void update() {
		// define min/max or de-normalized ranges unit
		//float szUnit = SURFING__PBR__SCENE_SIZE_UNIT; //size unit
		const float dUnit = SURFING__PBR__SCENE_SIZE_UNIT; //distance unit
		const float sPow = scalePow /** powRatio*/; //scale power
		const float rMax = 360; //rotation max/min. could be 180...

		float x = ofMap(position.get().x, -1, 1, -dUnit, dUnit, true);
		float y = ofMap(position.get().y, -1, 1, -dUnit, dUnit, true);
		float z = ofMap(position.get().z, -1, 1, -dUnit, dUnit, true);

		float rx = rotation.get().x;
		float ry = rotation.get().y;
		float rz = rotation.get().z;

		//TODO: fine tweak
		//float sUnit; //scale unit
		//const float r = 0.005;
		//sUnit = ofMap(scalePow, -100, 100, r * szUnit, 0.5 * r * szUnit, true);
		//float s = ofMap(scale, -1, 1, 1.f / sUnit, sUnit, true);
		float s;
		if (scale == 0) {
			s = scalePow;
		} else if (scale > 0) {
			s = ofMap(scale, 0, 1, scalePow, scalePow * 10, true);
		} else if (scale < 0) {
			s = ofMap(scale, 0, -1, scalePow, scalePow / 10.f, true);
		}

		ofTranslate(x, y, z);
		ofScale(s, s, s);
		ofRotateXDeg(rx);
		ofRotateYDeg(ry);
		ofRotateZDeg(rz);
	}

	/// \brief Classes extending ofNode can override this method to get
	///        notified when the position changed.
	void onPositionChanged() override {
	}

	/// \brief Classes extending ofNode can override this methods to get notified
	///        when the orientation changed.
	void onOrientationChanged() override { 
	}

	/// \brief Classes extending ofNode can override this methods to get notified 
	///        when the scale changed.
	void onScaleChanged() override {
	}

};

//----

#else
// simpler mode using only y position and rotation.
class TransformNode {
public:
	ofParameter<bool> bEnable { "Enable", true };

	ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	ofParameter<int> scalePow { "ScalePow", 0, -100, 100 };

private:
	float powRatio = 1.f;

public:
	void setPowRatio(float r) {
		powRatio = r;
	}

	ofParameter<float> yPos { "y Pos", 0, -1.f, 1.f };
	ofParameter<float> yRot { "y Rot", 0, -1.f, 1.f };

	ofParameterGroup parameters {
		"Transform",
		scalePow, scale, yPos, yRot
	};

	TransformNode() {
	}

	~TransformNode() {
	}

	//--

	float getPositionY() const {
		return yPos;
	}

	float getRotationY() const {
		return yRot;
	}

	void reset() {
		scalePow = 0;
		scale = 0;
		yPos = 0;
		yRot = 0;
	}

	bool isEnabled() const {
		return bEnable;
	}

	float getScale() const {
		return scale;
	}

	float getScalePow() const {
		return scalePow;
	}

	//--

	void update() {
		// define min/max or de-normalized ranges
		float sUnit = SURFING__PBR__SCENE_SIZE_UNIT; //size unit
		const float dUnit = sUnit; //distance unit
		const float sPow = scalePow * powRatio; //scale power
		const float rMax = 360; //rotation max/min

		sUnit = ofMap(scalePow, -100, 100, 0.02 * szUnit, 0.2 * szUnit, true);
		float s = ofMap(scale, -1, 1, 1.f / sUnit, sUnit, true);

		float y = ofMap(yPos, -1, 1, -dUnit, dUnit, true);
		float r = ofMap(yRot, -1, 1, -rMax, rMax, true);

		ofTranslate(0, y, 0);
		ofScale(s, s, s);
		ofRotateYDeg(r);
	}
};
#endif