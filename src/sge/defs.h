#pragma once

// pixels per world unit at default zoom level
#define SGE_WORLD_SCALE (384.0 / 17.0)

// isometric camera at 45 deg, 75 deg
#define SGE_CAMERA_ISO_ANGLE (75.0 * M_PI / 180.0)
#define SGE_Y_SCALE cos(SGE_CAMERA_ISO_ANGLE)

#define SGE_TEXTURES_ROOT "assets/textures/"
#define SGE_BASE_ROTATION_NAME "rot_base"

#define SGE_INT_MAX ((int)0x7FFFFFFF)
#define SGE_INT_MIN ((int)0x80000000)
#define SGE_FLOAT_MAX (3.402823466e+38F)
#define SGE_FLOAT_MIN (-3.402823466e+38F)
