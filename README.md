## Overview

### WIP 

**openFrameworks** addon to easily test, learn, and use the new **PBR features** from the new **OF 0.12+** releases. (You should use the GitHub master branch!)  

All this code is copied from and hardly based on the OF forum topic:  
https://forum.openframeworks.cc/t/ofshadow-and-ofshader-issue-on-of-0-12/42600/19  
Original authors: **@NickHardeman** and **@Paolo**.  

## Video
https://github.com/moebiussurfing/ofxSurfingPBR/assets/2106149/62c00037-f810-4fc4-8656-1dd61cdf06db

## Features
- Two ready-to-use styled materials:
  - A. For the floor/plane. (where the shadows are drawn)
  - B. For other objects.
- A positionable ambient light.
- Cubemaps with loader on runtime.
- Persistent settings.
- Easy to pass a local camera and draw your scene.
- Material and scene randomizers.
- Added shader testing. Applied to floor plane and his material/mesh too.

## Usage
Look at the example.

## Dependencies
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers) / Only for the example. Trivial to remove.  

## Tested Systems
* **Windows 11** / **VS 2022** / **OF 0.12+**

## License
**MIT License**
