#pragma once

//--

// OPTIONAL

#define SURFING__USE__PLANE_SHADER_AND_DISPLACERS // TODO: WIP: add another shader/material.

//#define SURFING__USE__FILE_BROWSER // allow browse model files on the folder.

//--

// DEBUG

//#define SURFING__SHADER_DEBUG // TODO: testing to improve performance.

//--

// MODULES

#define SURFING__USE_CUBE_MAP

//--

// CONSTANTS

#define SURFING__SCENE_SIZE_UNIT 1000.f
// size order magnitude for objects, lights and camera position, distances, etc...

#define SURFING__PLANE_SIZE_MULTIPLIER 10.f
#define SURFING__PLANE_SIZE_INFINITE_MODE 200000.f

#define SURFING__PLANE_RESOLUTION_MIN 2
#define SURFING__PLANE_RESOLUTION_MAX 1024

#define SURFING__SHADER_LIMIT_IMAGE_MIN 128
#define SURFING__SHADER_LIMIT_IMAGE_MAX 1024

#define SURFING__SCENE_CAMERA_FAR 1000000.f

#define SURFING__SCENE_TEST_UNIT_SCALE 5.f

#define SURFING__USE_AUTOSAVE_SETTINGS_ENGINE

//--

// PBR CONSTANTS

#define SURFING__PBR__MAX_SHININESS 100
#define SURFING__PBR__PLANE_COLORS_NO_ALPHA // bc it seems that is not being used internally..
