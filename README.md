## Overview

### WIP 

**openFrameworks** addon to easily test, learn, and use the new **PBR features** from the new **OF 0.12+** releases.  
(You should use the [GitHub master branch](https://github.com/openframeworks/openFrameworks)!)  

All this code is copied from and hardly based on the OF forum topic:  
https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19  
Original authors: **@NickHardeman** and **@Paolo**.  

## Video

https://github.com/moebiussurfing/ofxSurfingPBR/assets/2106149/62c00037-f810-4fc4-8656-1dd61cdf06db

## Features

- Two ready-to-use editable parametric materials:
  - A. For the plane floor.
  - B. For other objects.
- A positionable Ambient light.
- Shadows.
- Cubemaps with loader on runtime.
  - Alternative background color.
- Persistent settings.
- Easy to pass a local camera and draw your scene.
- Material and scene randomizers.
- Added shader for testing.
  - Applied to plane (floor) and his material/mesh...

## Usage

Look at the examples.

## Dependencies

[OF 0.12+](https://github.com/openframeworks/openFrameworks)

## Requeriments 

* Copy the content of 'data.zip' to the examples or your projects **/bin/data**. 

    .
    ├── data                   
    │   ├── cubemaps           # exr, hdr, jpg
    │   ├── models             # obj, ply, fbx
    │   ├── shadersGL2         # .frag/.vert
    │   └── shadersGL3         # .frag/.vert
    └── ...

    .
    ├── ...
    ├── test                    # Test files (alternatively `spec` or `tests`)
    │   ├── benchmarks          # Load and stress tests
    │   ├── integration         # End-to-end, integration tests (alternatively `e2e`)
    │   └── unit                # Unit tests
    └── ...




├───a/
│   ├───aa/
│   ├───ab/
│   └───ac/
├───a1/
├───b/
│   ├───ba/
│   │   ├───bafile1.txt
│   │   └───bafile2.txt
│   ├───bb/
│   ├───bc/
│   │   └───bca/
│   │       └───bca-file1.txt
│   ├───bd/
│   └───bfile1.txt
├───c/
├───c1/
└───d/
    ├───d1/
    └───d2/


my-app/
├─ node_modules/
├─ public/
│  ├─ favicon.ico
│  ├─ index.html
│  ├─ robots.txt
├─ src/
│  ├─ index.css
│  ├─ index.js
├─ .gitignore
├─ package.json
├─ README.md




## Tested Systems
* **Windows 11** / **VS 2022** / **OF 0.12+ GH Master branch**

## License
**MIT License**
