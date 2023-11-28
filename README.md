# WIP 

## OVERVIEW

**openFrameworks** addon to easily test, learn, and use the new **PBR features** from the new **OF 0.12+** releases.  

(You should use the [GitHub master branch](https://github.com/openframeworks/openFrameworks)!)  

All this code is copied from and hardly based on this [OF forum topic](https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19 ).  
Original authors: [@NickHardeman](https://github.com/NickHardeman) and [@paolo-scoppola](https://github.com/paolo-scoppola).  

## VIDEO

https://github.com/moebiussurfing/ofxSurfingPBR/assets/2106149/43ac19b8-da41-4dd5-ad4d-b77f885d3e24

<details>
  <summary>SCREENSHOTS</summary>
  <p>

### 1_Example_Basic
![](1_Example_Basic/Capture.PNG)

### 2_Example_Models
![](2_Example_Models/Capture.PNG)

### 3_Example_Lights
![](3_Example_Lights/Capture.PNG)

### 4_Example_Materials
![](4_Example_Materials/Capture.PNG)

### 5_Example_Shadows
![](5_Example_Shadows/Capture.PNG)
 
### 6_Example-3D_Parts
![](6_Example-3D_Parts/Capture.PNG)

  </p>
</details>

## FEATURES

- Main class `ofxSurfingPBR.h` is a **Scene Manager** with default common elements:
  - Two ready-to-use editable parametric materials:
    - **Material A**. For the **Plane** (floor).
    - **Material B**. For the **other Objects**.
  - Default **Lights combo**: 
    - Flexible **Point**, **Directional** and **Spot**.
  - **Cubemaps** (**HDR**) files loader.
  - **Background color** and **Sphere/Box container** with material.

- Class `SurfingMaterial.h`: **Standalone Materials** can be used independently of the main class.

- Class `SurfingLights.h`: Four **bundled lights**: `Point`, `Direct`, `Spot` and `Area` with shadows. 

- Class `SurfingSceneManager.h`: Bundles the lights from `SurfingLights.h` and allows **queuing materials** dynamically.

- Persistent settings:
  - Optimized **auto save** on any change.
  - **Plane** simple material, colors, transforms, snapshots, etc...
  - All materials with full **PBR** specs. 
  - Camera settings.
    - Included [ofxCameraSaveLoad](https://github.com/roymacdonald/ofxCameraSaveLoad) into `/libs`.

- Material **Randomizers**.
  - **History** browsing workflow.
    - **Presets**/**Snapshots** (Store/Recall) explorer system.
- Included **3D models** browser/loader class.
- Included [ofxSurfingHelpersLite](https://github.com/moebiussurfing/ofxSurfingHelpersLite) into `/libs`.
    - **Helper classes** for settings serializers, auto saver, ofxGui customizer, layout helpers, ofDrawBitmapStringBox...etc.  

- Added **Shader** for *testing/code*.
  - **Displacement** applied to the plane and his material/mesh.

### TODO
  - Add more **Shaders** related stuff. / HELP IS WELCOME! 
    - _Mesh/vertex_ displacements (not just heightmaps). / TODO 
    - Shader **Debugger**. / TODO 

<details>
  <summary>USAGE</summary>
  <p>

## EXAMPLE

### main.cpp
```.cpp
#include "ofApp.h"
#include "ofMain.h"

int main() {

    ofGLWindowSettings settings;
    settings.setGLVersion(3, 2);

    auto window = ofCreateWindow(settings);

    ofRunApp(window, make_shared<ofApp>());
    ofRunMainLoop();
}
```

### ofApp.h
```.cpp
#pragma once
#include "ofMain.h"

#include "ofxSurfingPBR.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void draw();

    ofEasyCam cam;

    ofxSurfingPBR pbr;
    void renderScene();
};
```

### ofApp.cpp
```.cpp
void ofApp::setup() {

    // Pass the local camera
    pbr.setup(cam);

    // Pass the render scene function
    callback_t f = std::bind(&ofApp::renderScene, this);
    pbr.setFunctionRenderScene(f);
}

void ofApp::draw() {
    pbr.draw();
    pbr.drawGui();
}

void ofApp::renderScene()
{
    // Plane floor
    pbr.drawPlane();

    // Other objects
    pbr.beginMaterial();
    {
        /*   DRAW HERE !   */
    }
    pbr.endMaterial();
}
```

  </p>
</details>

## DEPENDENCIES

* [OF 0.12+](https://github.com/openframeworks/openFrameworks).
* **ofxAssimpModelLoader** / _OF core_
    * Only for the example `2_Example_Models`. 

## REQUERIMENTS 

* Copy the content of `data.zip` to the examples or to your projects into `/bin/data`:  
[bin/data.zip](https://mega.nz/file/cHkylJZY#sVdQpSF8IkvdmKdpMIbA4767kvBvfGL7R7GnU9g20ow)  

bin  
$~$ └ data  
$~~~~~~$ ├ cubemaps $~~~~$ exr, hdr, jpg  
$~~~~~~$ ├ models $~~~~$ obj, ply, fbx  
$~~~~~~$ ├ shadersGL2 $~~~~$ frag, vert  
$~~~~~~$ └ shadersGL3 $~~~~$ frag, vert  

## TESTED SYSTEMS
* **Windows 11** / **VS 2022** / **OF 0.12+** @[GitHub master branch](https://github.com/openframeworks/openFrameworks)

## LICENSE
**MIT License**
