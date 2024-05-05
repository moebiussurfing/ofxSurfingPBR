#pragma once

#include "ofMain.h"

class materialPBRAdvanced{

	public:
		void setup();
		void draw();
		void drawGui();
	
		void renderScene(bool bShadowPass);
	
		bool reloadShader();

		void keyPressed(int key);
	
		ofEasyCam camera;
		//ofCubeMap cubeMap;
		ofVboMesh meshLogoHollow;
		ofMaterial matLogo;
		ofMaterial matFloor;
		ofMaterial matPlywood;
		ofMaterial matSphere;
	
		ofVboMesh meshPlySphere;
	
		ofShader mDepthShader;
	
		int mode = 0;
	
		ofLight light;
		bool bDebug = false;
		bool bWiggleVerts = false;
};
