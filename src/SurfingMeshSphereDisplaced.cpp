#include "SurfingMeshSphereDisplaced.h"

//--------------------------------------------------------------
SurfingMeshSphereDisplaced::SurfingMeshSphereDisplaced() {
	ofLogNotice("SurfingMeshSphereDisplaced") << "SurfingMeshSphereDisplaced()";
}

//--------------------------------------------------------------
SurfingMeshSphereDisplaced ::~SurfingMeshSphereDisplaced() {

	ofLogNotice("SurfingMeshSphereDisplaced") << "~SurfingMeshSphereDisplaced()";

	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingMeshSphereDisplaced::Changed);

	if (!bDoneExit) exit();
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::exit() {
	ofLogNotice("SurfingMeshSphereDisplaced") << "exit()";

	ofxSurfing::saveGroup(parameters, pathDisplace);

	bDoneExit = true;
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::Changed(ofAbstractParameter & e) {

	string name = e.getName();
	ofLogNotice("SurfingMeshSphereDisplaced") << "Changed " << name << " : " << e;

	if (name == vResetAll.getName()) {
		doResetAll();
	}
	if (name == vResetScale.getName()) {
		scaleGlobal = 0.5f;
	}
	if (name == scaleGlobal.getName()) {
		scale.set({ scaleGlobal.get(), scaleGlobal.get(), scaleGlobal.get() });
	}
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::setup(string name) {
	ofLogNotice("SurfingMeshSphereDisplaced") << "setup(" << name << ")";
	this->setName(name);
	setup();
}
//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::setName(const string & n) {
	name = n;
	ofLogNotice("SurfingMeshSphereDisplaced") << "setName(" << name << ")";
}
//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::setup() {
	ofLogNotice("SurfingMeshSphereDisplaced") << "setup()";

	sphere.setRadius(float(SURFING__OBJ_SIZE_MAG) * 0.5f);
	sphere.setResolution(50);

	//--

	noiseParams.setName("Noise");
	noiseParams.add(details.set("Details", 0.06, 0, 1));
	noiseParams.add(displace.set("Displace", 0.8, 0, 1));
	noiseParams.add(speed.set("Speed", 0.1, 0, 1));

	mainMesh = sphere.getMesh();

	depths = new float[sphere.getMesh().getNumVertices()];
	speeds = new float[sphere.getMesh().getNumVertices()];
	forces = new float[sphere.getMesh().getNumVertices()];

	for (int i = 0; i < sphere.getMesh().getNumVertices(); i++) {
		depths[i] = sphere.getMesh().getVertices()[i].length();
		speeds[i] = 0;
		forces[i] = 0;
	}

	//--

	//TODO: delete all this parameters from here
	// and use directly from the class
	string n = "MESH_SPHERE";
	if (name != "") {
		n += string("_");
		n += name;
	}
	parameters.setName(n);

	parameters.add(ampModulate.set("Modulate", 0.5f, 0.f, 1.f));
	parameters.add(scaleGlobal.set("Scale", 0.5f, 0.f, 1.f));
	parameters.add(scale.set("Scale xyz", glm::vec3(0.5f), glm::vec3(0.f), glm::vec3(1.f)));
	parameters.add(vResetScale.set("Reset Scale"));
	parameters.add(bSphere1.set("Sphere 1", true));
	parameters.add(radius1.set("Radius 1", 0.5, 0, 1));
	parameters.add(bSphere2.set("Sphere 2", true));
	parameters.add(noiseParams);
	parameters.add(vResetAll.set("Reset All"));

	ofAddListener(parameters.parameterChangedE(), this, &SurfingMeshSphereDisplaced::Changed);

	doResetAll();

	//---

	bGui.set("UI " + parameters.getName(), true);
	bDraw.set("Draw " + parameters.getName(), true);

	gui.setup(parameters);

	gui.getGroup(scale.getName()).minimize();

	if (pathGlobalFolder != "") pathDisplace = pathGlobalFolder + "\\";
	pathDisplace += "SceneDisplace";
	if (name != "") pathDisplace += "_" + name;
	pathDisplace += ".json";

	ofxSurfing::loadGroup(parameters, pathDisplace);
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::update() {
	mainMesh = sphere.getMesh();
	float time = speed * ofGetElapsedTimef();
	for (int i = 0; i < mainMesh.getNumVertices(); i++) {
		ofVec3f v = mainMesh.getVertex(i);
		float d = ofNoise(v.x * details * 0.1, v.y * details * 0.1, time) * displace * 100.0;
		v = v + mainMesh.getNormal(i) * d * ampModulate.get() * 2.f;
		mainMesh.setVertex(i, v);
	}
	setNormals(mainMesh);
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::drawSphereDisplaced(bool bwireframe, bool brotated) {
	ofPushMatrix();

	ofRotate(180, 0, 1, 0);
	if (brotated == true) ofRotate(180, 0, 1, 0);
	if (!bwireframe)
		mainMesh.drawFaces();

	//ofEnableSmoothing();
	if (bwireframe)
		mainMesh.drawWireframe();

	ofPopMatrix();
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::setNormals(ofMesh & mesh) {
	int nV = mesh.getNumVertices();
	int nT = mesh.getNumIndices() / 3;
	vector<ofPoint> norm(nV); //Array for the normals
	for (int t = 0; t < nT; t++) {
		int i1 = mesh.getIndex(3 * t);
		int i2 = mesh.getIndex(3 * t + 1);
		int i3 = mesh.getIndex(3 * t + 2);
		const ofPoint & v1 = mesh.getVertex(i1);
		const ofPoint & v2 = mesh.getVertex(i2);
		const ofPoint & v3 = mesh.getVertex(i3);
		ofPoint dir = -((v2 - v1).getCrossed(v3 - v1)).getNormalized();
		norm[i1] += dir;
		norm[i2] += dir;
		norm[i3] += dir;
	}
	for (int i = 0; i < nV; i++) {
		norm[i].normalize();
	}
	mesh.clearNormals();
	//mesh.addNormals( norm );

	vector<glm::vec3> normG(nV);
	for (int i = 0; i < nV; i++) {
		normG[i] = glm::vec3(norm[i].x, norm[i].y, norm[i].z);
	}
	mesh.addNormals(normG);
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::doResetAll() {
	ofLogNotice("SurfingMeshSphereDisplaced") << "doResetAll()";

	bSphere1 = false;
	radius1 = 0.5;
	bSphere2 = true;

	details = 0.1f;
	displace = 0.5f;
	speed = 0.5;

	ampModulate = 0.1f;

	vResetScale.trigger();
	//scaleGlobal = 0.5f;
	//scale = { 0.5f, 0.5f, 0.5f };
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::draw(bool bwire) {
	if (!bDraw) return;

	update();

	ofPushMatrix();

	//--

	float smin = 0.5f;
	float smax = 2.5f;
	float sx = ofMap(scale.get().x, scale.getMin().x, scale.getMax().x, smin, smax, true);
	float sy = ofMap(scale.get().y, scale.getMin().y, scale.getMax().y, smin, smax, true);
	float sz = ofMap(scale.get().z, scale.getMin().z, scale.getMax().z, smin, smax, true);
	ofScale(sx, sy, sz);

	if (bSphere1) {
		ofDrawSphere(0, 0, 0, radius1 * float(SURFING__OBJ_SIZE_MAG) * 0.5f * 1.5f);
	}

	if (bSphere2) drawSphereDisplaced(bwire, false);

	//--

	ofPopMatrix();
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::setPathGlobalFolder(string folder) {
	ofLogNotice("ofxSurfingScene3D") << "SurfingMeshSphereDisplaced:setPathGlobalFolder(" << folder << ")";
	pathGlobalFolder = folder;
}

//--------------------------------------------------------------
void SurfingMeshSphereDisplaced::setGuiPosition(glm::vec2 p) {
	gui.setPosition(p.x, p.y);
}