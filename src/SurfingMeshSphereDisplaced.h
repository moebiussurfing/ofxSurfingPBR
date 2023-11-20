#pragma once

//--

/*

	A simple class to draw a sphere mesh
	animated by a noise displacement.

*/

//--

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxSurfingHelpersLite.h"

#define SURFING__OBJ_SIZE_MAG 100

class SurfingMeshSphereDisplaced {

public:
	SurfingMeshSphereDisplaced();
	~SurfingMeshSphereDisplaced();

public:
	void setup();
	void setup(string name);
	void setName(const string & n);
	void exit();

private:
	bool bDoneExit = false;
	string name = "";

public:
	void setGuiPosition(glm::vec2 pos);
	void draw(bool bwire = false);

private:
	void update();
	void drawSphereDisplaced(bool bwire = false, bool brotated = false);

private:
	string pathGlobalFolder = ""; //top parent folder for all other subfolders

public:

	ofParameterGroup noiseParams;
	ofParameter<float> details;
	ofParameter<float> displace;
	ofParameter<float> speed;

	ofParameterGroup parameters;

	ofParameter<bool> bSphere1;//sphere with fixed radius
	ofParameter<float> radius1;
	ofParameter<bool> bSphere2;//sphere with modulated radius by noise
	ofParameter<float> ampModulate;// main mod controller
	ofParameter<glm::vec3> scale;
	ofParameter<float> scaleGlobal;
	ofParameter<void> vResetScale;
	
	ofParameter<void> vResetAll;
	void doResetAll();

	//--

	ofxPanel gui;
	ofParameter<bool> bGui;
	ofParameter<bool> bDraw;

	void drawGui() {
		if (!bGui) return;

		gui.draw();
	}

	void setGuiPosition(const glm::vec3 & p) {
		gui.setPosition(p.x, p.y);
	}

private:
	void Changed(ofAbstractParameter & e);

	string pathDisplace = "";

public:
	void setPathGlobalFolder(string folder);

private:
	// Mesh
	ofLight light;
	ofEasyCam cam;
	ofVboMesh mainMesh;

	float * depths;
	float * speeds;
	float * forces;

	ofSpherePrimitive sphere;
	//ofBoxPrimitive sphere;
	//ofIcoSpherePrimitive sphere;

	ofMesh mesh;

private:
	void setNormals(ofMesh & mesh);
};
