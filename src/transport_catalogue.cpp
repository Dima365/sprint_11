#include <unordered_set>
#include <cassert>

#include "geo.h"
#include "transport_catalogue.h"

using namespace std;

namespace transport {

void Catalogue::AddRoute(const string& number, vector<string>&& stops) {
  AddRouteToStops(number, stops);
  Bus bus;
  SetLengthAndCurvature(bus, stops);
  SetNumberStopsAndUniqueStops(bus, stops);
  bus.stops = move(stops);
  buses_.push_back(bus);
  busesPtr_.insert({number, &buses_.back()});
}

void Catalogue::SetLengthAndCurvature(Bus& bus,
                                      const vector<string>& stops) {
  double geographicLength = 0;
  for (size_t i = 0; i < stops.size() - 1 && stops.size() != 0; ++i) {
    pair<string, string> stops_pair{stops[i], stops[i + 1]};
    bus.routeLength += GetDistance(stops_pair);
    geographicLength += ComputeDistance(stopsPtr_[stops[i]]->coord,
                                        stopsPtr_[stops[i + 1]]->coord);
  }
  bus.curvature = double(bus.routeLength) / geographicLength;
}

void Catalogue::SetNumberStopsAndUniqueStops(Bus& bus, const vector<string>& stops) {
  unordered_set<string> stopsSet;
  for (const string& stop : stops) {
    stopsSet.insert(stop);
  }
  bus.uniqueStops = stopsSet.size();
  bus.stopsOnRoute = stops.size();
}

void Catalogue::AddRouteToStops(const string& number, const vector<string>& stops) {
  for (auto& stop : stops) {
    stopsPtr_[stop]->buses.insert(number);
  } 
}

void Catalogue::AddStop(const string& name, Coordinates coord) {
  Stop stop;
  stop.coord.lat = coord.lat;
  stop.coord.lng = coord.lng;
  stops_.push_back(stop);
  stopsPtr_.insert({name, &stops_.back()});
}

Bus* Catalogue::FindRoute(const string& number) {
  auto iter = busesPtr_.find(number);
  if (iter == busesPtr_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

Stop* Catalogue::FindStop(const string& stop) {
  auto iter = stopsPtr_.find(stop);
  if (iter == stopsPtr_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

Bus* Catalogue::GetBusData(const string& number) {
  auto iter = busesPtr_.find(number);
  if (iter == busesPtr_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

Stop* Catalogue::GetStopData(const string& name) {
  auto iter = stopsPtr_.find(name);
  if (iter == stopsPtr_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

void Catalogue::SetDistance(const pair<string, string>& stops,
                            uint32_t distance) {
  pair<Stop*, Stop*> stopPtrs(FindStop(stops.first), FindStop(stops.second));
  distanceBetweenStops_[stopPtrs] = distance;
}

uint32_t Catalogue::GetDistance(const pair<string, string>& stops) {
  pair<Stop*, Stop*> stopPtrs(FindStop(stops.first), FindStop(stops.second));
  auto iter = distanceBetweenStops_.find(stopPtrs);
  if (iter == distanceBetweenStops_.end()) {
    pair<Stop*, Stop*> stopPtrsBackwards(FindStop(stops.second),
                                         FindStop(stops.first));
    auto iterBackwards = distanceBetweenStops_.find(stopPtrsBackwards);
    if (iterBackwards == distanceBetweenStops_.end()) {
      return 0;
    } else {
      return iterBackwards->second;
    }
  } else {
    return iter->second;
  }
}

}  // namespace transport
