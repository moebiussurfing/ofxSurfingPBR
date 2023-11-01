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


├───cubemaps
│       hdrvfx_chanc_1_n1_v3_3k_Ref.hdr
│       kloppenheim_06_puresky_1k.exr
│       modern_buildings_2_1k.exr
│       WinterForest_8k.jpg
│
├───models
│       basic_form.ply
│       FlatCap.fbx
│       head25k.obj
│       ofLogoHollow.ply
│       Skull.obj
│       SkullLowPoly.fbx
│
├───shadersGL2
│       shader.frag
│       shader.vert
│
└───shadersGL3
        shader.frag
        shader.vert


        ª   ofxSurfingPBR_Material.json
ª   ofxSurfingPBR_Scene.json
ª   r.md
ª   
+---cubemaps
ª       hdrvfx_chanc_1_n1_v3_3k_Ref.hdr
ª       kloppenheim_06_puresky_1k.exr
ª       modern_buildings_2_1k.exr
ª       WinterForest_8k.jpg
ª       
+---models
ª       basic_form.ply
ª       FlatCap.fbx
ª       head25k.obj
ª       ofLogoHollow.ply
ª       Skull.obj
ª       SkullLowPoly.fbx
ª       
+---shadersGL2
ª       shader.frag
ª       shader.vert
ª       
+---shadersGL3
        shader.frag
        shader.vert

        
## Tested Systems
* **Windows 11** / **VS 2022** / **OF 0.12+ GH Master branch**

## License
**MIT License**
