#ifndef VIEWING_RAY_H
#define VIEWING_RAY_H

#include "Ray.h"
#include "Camera.h"

// Construct a viewing ray for a given camera and pixel coordinates.
//
// Inputs:
//   camera  camera parameters
//   i  row index of pixel
//   j  column index of pixel
//   width  image width in pixels
//   height  image height in pixels
// Outputs:
//   ray  viewing ray
void viewing_ray(
  const Camera & camera,
  const int i,
  const int j,
  const int width,
  const int height,
  Ray & ray);

#endif
