#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

struct Coordinates {
  Coordinates() = default;

  explicit Coordinates(double latitude, double longitude)
      : lat(latitude), lng(longitude) {}

  double lat = 0;
  double lng = 0;
};

double ComputeDistance(Coordinates from, Coordinates to);
