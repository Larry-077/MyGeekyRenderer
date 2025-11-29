#include "viewing_ray.h"

void viewing_ray(
  const Camera & camera,
  const int i,
  const int j,
  const int width,
  const int height,
  Ray & ray)
{
  ray.origin = camera.e;
  double u = camera.width / width * ( j + 0.5) - camera.width / 2;
  double v = camera.height / 2 - camera.height / height * ( i + 0.5);
  ray.direction = u * camera.u + v * camera.v - camera.d * camera.w;
}

