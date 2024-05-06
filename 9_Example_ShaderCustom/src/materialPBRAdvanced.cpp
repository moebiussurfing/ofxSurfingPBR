#include "materialPBRAdvanced.h"

//--------------------------------------------------------------
void materialPBRAdvanced::setup() {

	bDebug.set("debug", false);
	bWiggleVerts.set("bWiggleVerts", false);
	parameters.setName("materialPBRAdvanced");
	parameters.add(bDebug);
	parameters.add(bWiggleVerts);
	gui.setup(parameters);
	gui.setPosition(10, ofGetHeight() - 200);

	//--

	ofDisableArbTex();

	meshLogoHollow.load("models/ofLogoHollow.ply");
	meshLogoHollow.mergeDuplicateVertices();
	// we need to flip the normals for this mesh //
	for (size_t i = 0; i < meshLogoHollow.getNumNormals(); i++) {
		meshLogoHollow.getNormals()[i] *= -1.f;
	}

	// going to add a custom uniform, passing elapsed time to use for animation
	matLogo.setCustomUniform1f("iElapsedTime", 1.0);
	// set the material to PBR, default if phong
	matLogo.setPBR(true);
	matFloor.setPBR(true);

	string path = "shaders_custom/images_plywood/";
	matPlywood.setPBR(true);
	matPlywood.loadTexture(OF_MATERIAL_TEXTURE_DIFFUSE, path + "plywood_diff_2k.jpg");
	matPlywood.loadTexture(OF_MATERIAL_TEXTURE_NORMAL, path + "plywood_nor_gl_2k.png");
	matPlywood.loadTexture(OF_MATERIAL_TEXTURE_AO_ROUGHNESS_METALLIC, path + "plywood_arm_2k.jpg");
	matPlywood.setTexCoordScale(1.0, 0.3);
	matPlywood.setClearCoatEnabled(true);
	matPlywood.setClearCoatStrength(1.0);
	matPlywood.setClearCoatRoughness(0.0);

	// higher resolution sphere
	meshPlySphere = ofMesh::icosphere(1.0, 5);

	matSphere.setPBR(true);
	matSphere.setDiffuseColor(ofFloatColor(0.25, 1.0));
	matSphere.setMetallic(1.0);
	matSphere.setRoughness(0.05);

	reloadShader();
}

//--------------------------------------------------------------
void materialPBRAdvanced::draw() {
	renderScene(true); // -> with shadow

	//	ofEnableDepthTest();
	//
	//	if (light.shouldRenderShadowDepthPass()) {
	//		int numShadowPasses = light.getNumShadowDepthPasses();
	//		for (int j = 0; j < numShadowPasses; j++) {
	//			light.beginShadowDepthPass(j);
	//			renderScene(true); // -> with shadow
	//			light.endShadowDepthPass(j);
	//		}
	//	}
	//
	//	camera.begin();
	//	{
	//
	//		renderScene(false); // -> without shadow
	//
	//		if (cubeMap.hasPrefilteredMap()) {
	//			cubeMap.drawPrefilteredCube(0.2f);
	//		}
	//
	//		if (bDebug) {
	//			light.draw();
	//
	//			if (light.getIsEnabled() && light.getShadow().getIsEnabled()) {
	//				light.getShadow().drawFrustum();
	//			}
	//		}
	//	}
	//	camera.end();
	//
	//	ofDisableDepthTest();
	//
	//	ofSetColor(255);
	//	ofEnableAlphaBlending();
	//#ifdef USE_LIGHT
	//	for (auto & lp : lights) {
	//		lp->gui.draw();
	//	}
	//#endif
}

//--------------------------------------------------------------
void materialPBRAdvanced::drawGui() {

	stringstream ss;
	ss << "materialPBRAdvanced" << endl;
	ss << "Reload shader(r): make changes to shader \nin data/shaders/main.frag and \nthen press 'r' to see changes.";
	ss << endl
	   << "Wiggle verts(w): " << (bWiggleVerts ? "yes" : "no");
	ss << endl
	   << "Frame rate: " << ofGetFrameRate();

	int x = gui.getShape().getBottomLeft().x + 5;
	int y = gui.getShape().getBottomLeft().y + 15;

	ofDrawBitmapStringHighlight(ss.str(), x, y);

	gui.draw();
}

//--------------------------------------------------------------
void materialPBRAdvanced::renderScene(bool bShadowPass) {

	float zoffset = 500;
	float sz = 1500;

	// floor
	matFloor.setMetallic(0.0);
	matFloor.setReflectance(0.01);
	matFloor.setRoughness(0.8);
	matFloor.setDiffuseColor(ofFloatColor(0.97, 0.96, 0.91, 1.0));

	matFloor.begin();
	ofDrawBox(0, -275, zoffset, sz, 25, sz * 0.33);
	matFloor.end();

	// ball 1
	matPlywood.begin();
	ofPushMatrix();
	ofTranslate(0, 0, zoffset);
	float angle = ofGetElapsedTimef();
	ofTranslate(700.0 * cos(angle), -20, sin(angle) * 300.0 - 100);
	ofScale(60);
	meshPlySphere.draw();
	ofPopMatrix();
	matPlywood.end();

	// logo
	if (!matLogo.hasDepthShader() && bShadowPass && bWiggleVerts) {
		mDepthShader.begin();
		mDepthShader.setUniform1f("iElapsedTime", ofGetElapsedTimef());
		mDepthShader.setUniform1f("uWiggleVerts", bWiggleVerts ? 1.0f : 0.0f);
	}
	// setting custom uniforms on a material automatically adds it to the shader
	matLogo.setCustomUniform1f("iElapsedTime", ofGetElapsedTimef());
	matLogo.setCustomUniform1f("uWiggleVerts", bWiggleVerts ? 1.0f : 0.0f);
	matLogo.begin();
	ofPushMatrix();
	ofTranslate(-70, -250, zoffset);
	ofRotateXDeg(-90);
	ofScale(50);
	meshLogoHollow.draw();
	ofPopMatrix();
	matLogo.end();
	if (!matLogo.hasDepthShader() && bShadowPass && bWiggleVerts) {
		mDepthShader.end();
	}

	// ball 2
	matSphere.begin();
	ofPushMatrix();
	ofTranslate(0, 0, zoffset);
	ofTranslate(700.0 * cos(angle - PI), -20, sin(angle - PI) * 300.0 - 100);
	ofScale(120);
	meshPlySphere.draw();
	ofPopMatrix();
	matSphere.end();
}

//--------------------------------------------------------------
bool materialPBRAdvanced::reloadShader() {
	string path = "shaders_custom/shaders_materialPBRAdvanced/";

	// load the shader main functions //
	string vname = path + "main.vert";
	ofBuffer vbuffer = ofBufferFromFile(vname);
	string fname = path + "main.frag";
	ofBuffer fbuffer = ofBufferFromFile(fname);
	if (vbuffer.size() && fbuffer.size()) {
		matLogo.setShaderMain(vbuffer.getText(), GL_VERTEX_SHADER, path + "main.vert");
		matLogo.setShaderMain(fbuffer.getText(), GL_FRAGMENT_SHADER, path + "main.frag");
		matLogo.setDepthShaderMain(vbuffer.getText(), path + "main.glsl");
		// configure the shader to include shadow functions for passing depth
		// #define OF_SHADOW_DEPTH_PASS gets added by OF so we can use the same shader file and run different bits of code for the shadow pass
		// we add #define NON_MATERIAL_DEPTH_PASS because ofMaterial adds variables that we need
		// to add manually when not using a materil, see main.vert
		//		light.getShadow().setupShadowDepthShader(mDepthShader, "#define NON_MATERIAL_DEPTH_PASS\n"+vbuffer.getText());
		return true;
	}
	return false;
}

//--------------------------------------------------------------
void materialPBRAdvanced::keyPressed(int key) {
	cout << "key: " << key << endl;
	if (key == 'r') {
		reloadShader();
	}
	if (key == 'd') {
		bDebug = !bDebug;
	}
	if (key == 'w') {
		bWiggleVerts = !bWiggleVerts;
	}
	if (key == OF_KEY_DEL || key == 8) {
		cout << "trying to remove texture from mat plywood" << endl;
		matPlywood.removeTexture(OF_MATERIAL_TEXTURE_DIFFUSE);
		matPlywood.removeTexture(OF_MATERIAL_TEXTURE_NORMAL);
		matPlywood.removeTexture(OF_MATERIAL_TEXTURE_AO_ROUGHNESS_METALLIC);
	}
}
