# WIP 

## Overview

**openFrameworks** addon to easily test, learn, and use the new **PBR features** from the new **OF 0.12+** releases.  

(You should use the [GitHub master branch](https://github.com/openframeworks/openFrameworks)!)  

All this code is copied from and hardly based on this [OF forum topic](https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19 ).  
Original authors: [@NickHardeman](https://github.com/NickHardeman) and [@paolo-scoppola](https://github.com/paolo-scoppola).  

## Screenshots

### 1_Example_Basic
![](1_Example_Basic/Capture.PNG)

### 2_Example_Models
![](2_Example_Models/Capture.PNG)

## Video

https://github.com/moebiussurfing/ofxSurfingPBR/assets/2106149/43ac19b8-da41-4dd5-ad4d-b77f885d3e24

## Features

- Two ready-to-use editable parametric materials:
  - **Material A**. For the **Plane** (floor).
  - **Material B**. For the **other Objects**.
- A positionable Ambient **light** with **Shadows**.
- **Cubemaps** (aka **HDR**) files.
  - On runtime loader.
  - Alternative background color.
- Added **Shader** for *testing/code*.
  - **Displacement** applied to the plane and his material/mesh.
- Persistent settings:
  - **Plane** colors, transforms, simple material, snapshots, etc...
  - **Objects** material with full specs. 
  - Camera settings.
    - Included [ofxCameraSaveLoad](https://github.com/roymacdonald/ofxCameraSaveLoad) into `/libs`.
- Material **Randomizers**.
- **History** browsing workflow.
    - **Presets**/**Snapshots** (Store/Recall) explorer system.
    - Kind of **Undo/Redo**.
- **3D models** browser/loader class
  - Used in the example `\2_Example_Models\`.
 - Included [ofxSurfingHelpersLite](https://github.com/moebiussurfing/ofxSurfingHelpersLite) into `/libs`.

### TODO
  - Add more **Shaders** related stuff. / _HELP IS WELCOME!_ 
    - _Mesh/vertex_ displacements (not just heightmaps). / _TODO_ 
    - Shader **Debugger**. / _TODO_ 

<details>
  <summary>Usage</summary>
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

#include <functional>
using callback_t = std::function<void()>;

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
    pbr.setup();

    // Pass the local camera
    pbr.setCameraPtr(&cam);

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
        ///*   DRAW HERE !   *///
    }
    pbr.endMaterial();
}
```

  </p>
</details>

## Dependencies

* [OF 0.12+](https://github.com/openframeworks/openFrameworks).
* **ofxAssimpModelLoader** / _OF core_
    * Only for the example `2_Example_Models`. 

## Requeriments 

* Copy the content of `data.zip` to the examples or to your projects into `/bin/data`:

bin  
$~$ └ data  
$~~~~~~$ ├ cubemaps $~~~~~~$ exr, hdr, jpg  
$~~~~~~$ ├ models $~~~~~~~~$ obj, ply, fbx  
$~~~~~~$ ├ shadersGL2 $~~~~$ frag, vert  
$~~~~~~$ └ shadersGL3 $~~~~$ frag, vert  

## Tested Systems
* **Windows 11** / **VS 2022** / **OF 0.12+** @[GitHub master branch](https://github.com/openframeworks/openFrameworks)

## License
**MIT License**
