#include "geo.h"

inline const int EarthRadius = 6371000;

double ComputeDistance(Coordinates from, Coordinates to) {
  using namespace std;
  static const double dr = M_PI / 180.0;
  return acos(sin(from.lat * dr) * sin(to.lat * dr) +
              cos(from.lat * dr) * cos(to.lat * dr) *
                  cos(abs(from.lng - to.lng) * dr)) *
         EarthRadius;
}