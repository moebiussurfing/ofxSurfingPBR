#pragma once

//--

// OPTIONAL

#define SURFING__USE_FILE_BROWSER // allow browse model files on the folder.

#define SURFING__USE__PLANE_SHADER_AND_DISPLACERS // TODO: WIP: add another shader/material.

//--
 
// DEBUG

//#define SURFING__CLAMP_PLANE_SIZE_BC_PERFORMANCE // TODO: testing to improve performance.

//--

// MODULES

#define SURFING__USE_CUBE_MAP

//--

// CONSTANTS

#define SURFING__PLANE_RESOLUTION 10
#define SURFING__PLANE_RESOLUTION_MAX 100
#define SURFING__PLANE_SIZE_INFINITE_MODE 200000.f
#define SURFING__PLANE_SIZE_MULTIPLIER 20.f

#define SURFING__SCENE_SIZE_UNIT 1000.f

#define SURFING__USE_AUTOSAVE_ENGINE

//--

// PBR CONSTANTS

#define SURFING__PBR__MAX_SHININESS 100