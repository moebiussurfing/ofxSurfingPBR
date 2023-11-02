## Overview

# WIP 

**openFrameworks** addon to easily test, learn, and use the new **PBR features** from the new **OF 0.12+** releases.  

(You should use the [GitHub master branch](https://github.com/openframeworks/openFrameworks)!)  

All this code is copied from and hardly based on this [OF forum topic](https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19 ).  
Original authors: [@NickHardeman](https://github.com/NickHardeman) and [@paolo-scoppola](https://github.com/paolo-scoppola).  

## Screenshot

![](2_Example_Models/Capture.PNG)

## Video

https://github.com/moebiussurfing/ofxSurfingPBR/assets/2106149/43ac19b8-da41-4dd5-ad4d-b77f885d3e24

## Features

- Two ready-to-use editable parametric materials:
  - A. For the **Plane** (floor).
  - B. For the other **Objects**.
- A positionable Ambient **light** with **Shadows**.
- **Cubemaps** (**HDR**) files with on runtime loader.
  - Alternative background color.
- Persistent settings:
  - **Plane** colors, transforms, simple material, etc...
  - **Objects** material with full specs. 
  - Camera settings.
    - Included [ofxCameraSaveLoad](https://github.com/roymacdonald/ofxCameraSaveLoad) into `/libs`.
- Easy to pass a local **camera** and your **render scene**.
  - Test camera workflow between classes. [TODO] 
- Material and scene **Randomizers**.
  - History browsing **Undo/Redo** workflow. [TODO] 
- Added **Shader** for *testing/code*.
  - **Displacement** applied to the plane and his material/mesh.
  - Mesh/vertex displacements (not just heightmaps). [TODO]
  - Shader **Debugger**. [TODO]
- **3D models** browser/loader class
  - Used into `\2_Example_Models\`.

## Usage

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

      drawGui();
}

void ofApp::renderScene()
{
      // Plane floor
      pbr.drawPlane();

      // Other objects
      pbr.beginMaterial();
      {
          ///* Draw here! *///
      }
      pbr.endMaterial();
}
```

## Dependencies

[OF 0.12+](https://github.com/openframeworks/openFrameworks)

## Requeriments 

* Copy the content of `data.zip` to the examples or to your projects into `/bin/data`.

  └ data  
      ├ cubemaps / .exr / .hdr / .jpg  
      ├ models / .obj / .ply / .fbx  
      ├ shadersGL2 / .frag / .vert  
      └ shadersGL3 / .frag / .vert  

## Tested Systems
* **Windows 11** / **VS 2022** / **OF 0.12+**  
@[GitHub master branch](https://github.com/openframeworks/openFrameworks)

## License
**MIT License**
