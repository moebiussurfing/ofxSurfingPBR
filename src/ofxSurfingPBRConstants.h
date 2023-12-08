#pragma once

//----

// OPTIONAL

#define SURFING__PBR__USE__PLANE_SHADER_AND_DISPLACERS 
// TODO: WIP: add another shader/material.

#define SURFING__PBR__USE__MODELS_MANAGER 
// allow browse model files on the folder.

//----

// DEBUG / WIP

//#define SURFING__SHADER_DEBUG 
// TODO: testing to improve performance.

#define SURFING__PBR__WIP__DISBALE_COLORS_EXTRA
//TODO: disabled as is not finished yet.
// The idea of this code is to have a palette of colors
// linked to the materials easily.

//----

// MODULES

#define SURFING__PBR__USE_LIGHTS_CLASS
#define SURFING__PBR__USE_CUBE_MAP

//----

// CONSTANTS

#define SURFING__PBR__SCENE_SIZE_UNIT 1000.f
// size order magnitude for objects, 
// lights and camera position, distances, etc...

#define SURFING__PBR__PLANE_SIZE_MULTIPLIER 10.f
#define SURFING__PBR__PLANE_SIZE_INFINITE_MODE 200000.f

#define SURFING__PBR__PLANE_RESOLUTION_MIN 2
#define SURFING__PBR__PLANE_RESOLUTION_MAX 1024

#define SURFING__PBR__SHADER_LIMIT_IMAGE_MIN 128
#define SURFING__PBR__SHADER_LIMIT_IMAGE_MAX 1024

#define SURFING__PBR__SCENE_CAMERA_FAR 1000000.f

#define SURFING__PBR__SCENE_TEST_UNIT_SCALE 5.f

//TODO
// there's some problem rounding bright to white/black..
#define SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MIN 0.01f
#define SURFING__PBR__HELPER_GLOBAL_BRIGHT_LIMIT_MAX 0.99f

#define SURFING__PBR__HELPER_GLOBAL_BRIGHT_RESET 0.25f

//----

#define SURFING__PBR__USE_AUTOSAVE_SETTINGS_ENGINE 
// saving happens when each param/s change/s but delayed a second.

#define SURFING__PBR__USE_AUTOSAVE_FORCE_ON_EXIT
// TODO: fix to remove. to fix some unsaved settings bc calling order..

//----

// PBR CONSTANTS

//#define SURFING__PBR__MAX_SHININESS 1.f
#define SURFING__PBR__MAX_SHININESS 128.f

#define SURFING__PBR__SHADOW_DEFAULT_STRENGTH 0.5f
#define SURFING__PBR__SHADOW_DEFAULT_RESOLUTION 1024

static const float SURFING__PBR__LIGHTS_FAR = (float)SURFING__PBR__SCENE_SIZE_UNIT * 1.0f;
static const float SURFING__PBR__LIGHTS_NEAR = SURFING__PBR__LIGHTS_FAR * 0.1f;


#define SURFING__PBR__PLANE_COLORS_NO_ALPHA 
// bc it seems that alpha is not being used internally..

#define SURFING__PBR__BG_COLORS_NO_ALPHA 
// bc it seems that alpha is not being used internally..

//----

// PBR NOTES
// RANGES AND INFO
/*
	shininess is a value between 0 - 128, 128 being the most shiny

	setSpotlightCutOff is clamped between 0 - 90 degrees

	setSpotConcentration is clamped between 0 - 128

	shadow bias is the margin of error the shadow depth
	increasing the bias helps reduce shadow acne, but can cause light leaking
	try to find a good balance that fits your needs
	bias default is 0.005
	//ofShadow::setAllShadowBias(0.007);
	normal bias default is 0
	moves the bias along the normal of the mesh, helps reduce shadow acne
	//ofShadow::setAllShadowNormalBias(-4.f);
*/