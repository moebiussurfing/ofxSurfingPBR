#include "SurfingMaterial.h"
//--------------------------------------------------------------
SurfingMaterial::SurfingMaterial() {
	ofAddListener(ofEvents().update, this, &SurfingMaterial::update);
	ofAddListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
}

//--------------------------------------------------------------
SurfingMaterial::~SurfingMaterial() {
	ofRemoveListener(ofEvents().update, this, &SurfingMaterial::update);
	ofRemoveListener(parameters.parameterChangedE(), this, &SurfingMaterial::Changed);
}

//--------------------------------------------------------------
void SurfingMaterial::setupGui() {
	gui.setup(parameters);
	gui.loadFromFile(path);
}

//--------------------------------------------------------------
void SurfingMaterial::setup() {

	parameters.setName("PBR_Material");

#ifdef SURFING__USE_DISPLACE
	planeParams.add(useMaterial.set("Use Material", true));
	planeParams.add(noiseAmplitude.set("Displacement", 0.0f, 0.0f, 255.0f));
	planeParams.add(noiseScale.set("Noise Scale", 0.0f, 0.0f, 0.1f));
	planeParams.add(noiseSpeed.set("Noise Speed", 0.0f, 0.0f, 1.0f));
	parameters.add(planeParams);

	parameters.add(displacementStrength.set("Displacement Strength", 0.0, 0.0, 500.0));
	parameters.add(displacementNormalsStrength.set("Displacement Normals Strength", 0.0, 0.0, 1.0));
	parameters.add(normalGeomToNormalMapMix.set("Normal Geom To Normal Map Mix", 0.0, 0.0, 1.0));
#endif

	parameters.add(shininess.set("Shininess", 0.0, 0.0, 1.0));
	parameters.add(roughness.set("Roughness", 0.0, 0.0, 1.0));
	parameters.add(metallic.set("Metallic", 0.0, 0.0, 1.0));
	parameters.add(reflectance.set("Reflectance", 0.0, 0.0, 1.0));

	parameters.add(ambientColor.set("Ambient Color", ofColor::white));
	parameters.add(specularColor.set("Specular Color", ofColor::white));
	parameters.add(diffuseColor.set("Diffuse Color", ofColor::white));
	parameters.add(emissiveColor.set("Emissive Color", ofColor::white));

	parameters.add(clearCoat.set("Clear Coat", false));
	parameters.add(clearCoatRoughness.set("Clear Coat Roughness", 0.0001, 0.0001, 10.0));
	parameters.add(clearCoatStrength.set("Clear Coat Strength", 0.0001, 0.0001, 10.0));

	parameters.add(resetMaterial);

	//--

#ifdef SURFING__USE_DISPLACE
	ofDisableArbTex();

	shader.load("shadersGL3/shader");
	#ifdef TARGET_OPENGLES
	OPENGL ES supports GL_RGBA32F but not GL_R32F
		img.allocate(80, 60, OF_IMAGE_COLOR_ALPHA);
	#else
	img.allocate(800, 600, OF_IMAGE_GRAYSCALE);
	#endif
#endif

	//--

#ifdef SURFING__USE_DISPLACE
	plane.set(800, 600, 80, 60);
	plane.mapTexCoordsFromTexture(img.getTexture());
#endif

	//--

	doResetMaterial();

	setupGui();
}

//--------------------------------------------------------------
void SurfingMaterial::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void SurfingMaterial::update() {

	material.setDiffuseColor(diffuseColor);
	material.setAmbientColor(ambientColor);
	material.setEmissiveColor(emissiveColor);
	material.setSpecularColor(specularColor);

	material.setRoughness(roughness);
	material.setMetallic(metallic);
	material.setReflectance(reflectance);
	material.setShininess(shininess);

	//material.setTexCoordScale(scaleX, scaleY);
	material.setClearCoatEnabled(clearCoat);
	material.setClearCoatRoughness(clearCoatRoughness);
	material.setClearCoatStrength(clearCoatStrength);

#ifdef SURFING__USE_DISPLACE
	material.setDisplacementTexture(img.getTexture());
	material.setDisplacementStrength(displacementStrength);
	material.setDisplacementNormalsStrength(displacementNormalsStrength);

	material.setNormalGeomToNormalMapMix(normalGeomToNormalMapMix);

	//--

	// compute displace/shader
	float n = noiseSpeed * ofGetElapsedTimef();

	auto & pixels = img.getPixels();
	int numChannels = pixels.getNumChannels();
	int w = img.getWidth();
	int h = img.getHeight();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int i = y * w + x;
			float value = noiseAmplitude * ofNoise(x * noiseScale, y * noiseScale, n);
			for (int k = 0; k < numChannels; k++) {
				pixels[i * numChannels + k] = value;
			}
		}
	}

	img.update();
#endif
}

//--------------------------------------------------------------
void SurfingMaterial::begin() {
#ifdef SURFING__USE_DISPLACE
	if (useMaterial) {
		material.begin();
	} else {
		shader.begin();
		shader.setUniformTexture("displacement", img.getTexture(), 1);
	}
#else
	material.begin();
#endif
}
//--------------------------------------------------------------
void SurfingMaterial::end() {
#ifdef SURFING__USE_DISPLACE
	if (useMaterial) {
		material.end();
	} else {
		shader.end();
	}
#else
	material.end();
#endif
}

//--------------------------------------------------------------
void SurfingMaterial::drawGui() {

	gui.draw();
}

//--------------------------------------------------------------
void SurfingMaterial::Changed(ofAbstractParameter & e) {
	std::string name = e.getName();

	ofLogNotice(__FUNCTION__) << name << " : " << e;

	if (name == resetMaterial.getName()) {
		doResetMaterial();
	}
}

//--------------------------------------------------------------
void SurfingMaterial::doResetMaterial() {
	shininess.set(0);
	roughness.set(0);
	metallic.set(0);
	reflectance.set(0);

	ambientColor.set(ofColor::white);
	specularColor.set(ofColor::white);
	diffuseColor.set(ofColor::white);
	emissiveColor.set(ofColor::white);

	clearCoat.set(false);
	clearCoatRoughness.set(0.0001);
	clearCoatStrength.set(0.0001);
}

//--------------------------------------------------------------
void SurfingMaterial::exit() {
	gui.saveToFile(path);
}