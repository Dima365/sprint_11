#pragma once

#include <set>
#include <string>
#include <vector>

#include "geo.h"

struct Bus {
  int stopsOnRoute = 0;
  int uniqueStops = 0;
  double routeLength = 0;
  double curvature = 0;
  std::vector<std::string> stops;
};

struct Stop {
  Coordinates coord;
  std::set<std::string> buses;
};
