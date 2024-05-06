#include "vboMeshDrawInstanced.h"

/**
 * tig: This example shows how to draw instanced geometry using 
 * ofVboMesh.drawInstanced() and matching GLSL shader code.
 * The most interesting things will happen in the shader files, 
 * and in the draw() method, where everything is set up properly 
 * to allow instanced rendering.
 *
 * Note that you can feed any ofVboMesh to the instanced draw method.
 *
 */

//--------------------------------------------------------------
vboMeshDrawInstanced::vboMeshDrawInstanced() {
}

//--------------------------------------------------------------
vboMeshDrawInstanced::~vboMeshDrawInstanced() {
	ofxSurfing::saveGroup(parameters);
}

//--------------------------------------------------------------
void vboMeshDrawInstanced::setup() {
	bDraw.set("bDraw", true);
	isShaderDirty.set("isShaderDirty", false);
	float sz = 1000;
	position.set("position", glm::vec3(0, 0, 0), glm::vec3(-sz, -sz, -sz), glm::vec3(sz, sz, sz));
	rotation.set("rotation", glm::vec3(0, 0, 0), glm::vec3(-180, -180, -180), glm::vec3(180, 180, 180));
	parameters.setName("vboMeshDrawInstanced");
	parameters.add(bDraw);
	parameters.add(position);
	parameters.add(rotation);
	parameters.add(isShaderDirty);
	ofxSurfing::loadGroup(parameters);
	gui.setup(parameters);
	gui.setPosition(ofGetWidth() - 300, ofGetHeight() - 300);

	//--

	// initialize variables:

	isShaderDirty = true; // this flag will tell us whether to reload our shader from disk.
	// this allows you to change your shaders without having to restart
	// your app. we'll set it up so that pressing the SPACE key on your
	// keyboard will reload the shader.

	// generate a box vboMesh from a primitive.

	ofBoxPrimitive tmpBox;
	// set the size to be 2 units.
	tmpBox.set(2);

	mVboBox = tmpBox.getMesh();

	// load depth image
	ofDisableArbTex();
	// note that we disable arb tex, meaning we will use normalized texture coordinates,
	// where a texture's x and y coordinate are each expressed as a normalized float.
	// this makes things slightly easier in the shader.

	// load the depth image into our texture
	ofLoadImage(mTexDepth, "shaders_custom/images/depth_image.png");
	ofEnableArbTex();
}

//--------------------------------------------------------------
void vboMeshDrawInstanced::update() {

	if (isShaderDirty) {

		// only reload the shader if it is 'dirty', i.e. the user has either requested reloading
		// or the 'isShaderDirty' flag has been initialized to true in setup()

		// Since we are using a shared_ptr around the shader, the old shader will get destroyed
		// automatically as soon as we assign a new shader object to our mShdInstanced.
		string path = "shaders_custom/shaders_vboMeshDrawInstanced/";

		ofLogNotice() << "Reloading Shader.";
		mShdInstanced = shared_ptr<ofShader>(new ofShader());
		// most of the instanced drawing magic happens in the shaders:
#ifdef USE_PROGRAMMABLE_GL
		// if we are using programmable GL, we load the GLSL version 150 shader pair.
		mShdInstanced->load(path + "instanced.vert", path + "instanced.frag");
#else
		// if we are using fixed function GL, we load the GLSL version 120 shader pair.
		mShdInstanced->load(path + "instanced_120.vert", path + "instanced_120.frag");
#endif
		GLint err = glGetError();
		if (err != GL_NO_ERROR) {
			ofLogError() << "Load Shader came back with GL error:	" << err;
		}

		isShaderDirty = false;
	}
}

//--------------------------------------------------------------
void vboMeshDrawInstanced::draw() {
	if (!bDraw) return;

	update();

	//--

	ofPushMatrix();
	{
		ofTranslate(position);
		ofRotateDeg(rotation.get().x, 1, 0, 0);
		ofRotateDeg(rotation.get().y, 0, 1, 0);
		ofRotateDeg(rotation.get().z, 0, 0, 1);

		//--

		//ofEnableDepthTest();
		//// we don't care about alpha blending in this example, and by default alpha blending is on in openFrameworks > 0.8.0
		//// so we de-activate it for now.
		//ofDisableAlphaBlending();

		//--

		ofPushStyle();
		ofSetColor(ofColor::white);

		// bind the shader
		mShdInstanced->begin();
		// give the shader access to our texture
		mShdInstanced->setUniformTexture("tex0", mTexDepth, 0);
		// feed the shader a normalized float value that changes over time, to animate things a little
		mShdInstanced->setUniform1f("timeValue", (ofGetElapsedTimeMillis() % 30000) / 30000.0f);
		// we only want to see triangles facing the camera.
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// let's draw 128 * 128 == 16384 boxes !
		mVboBox.drawInstanced(OF_MESH_FILL, 128 * 128);

		glDisable(GL_CULL_FACE);
		mShdInstanced->end();

		ofPopStyle();

		//--

		//ofEnableAlphaBlending();
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void vboMeshDrawInstanced::drawGui() {

	int x = gui.getShape().getBottomLeft().x + 5;
	int y = gui.getShape().getBottomLeft().y + 20;

	ofSetColor(ofColor::white);
	stringstream ss;
	ss << "vboMeshDrawInstanced" << endl;
	ss << "Use mouse to move camera.\nPress 'f' to toggle fullscreen;\nSPACEBAR to reload shader.";
	ofDrawBitmapStringHighlight(ss.str(), x, y);

	gui.draw();
}

//--------------------------------------------------------------
void vboMeshDrawInstanced::keyReleased(int key) {

	switch (key) {
	case ' ':
		isShaderDirty = true;
		// mark the shader as dirty - this will reload the shader.
		break;
	default:
		break;
	}
}
