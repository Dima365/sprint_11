#pragma once

#include <deque>
#include <iomanip>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "domain.h"

namespace transport {

class Catalogue {
 public:
  void AddRoute(const std::string& number,
                std::vector<std::string>&& stops);

  void AddStop(const std::string& name, Coordinates coord);

  Bus* FindRoute(const std::string& number);

  Stop* FindStop(const std::string& stop);

  Bus* GetBusData(const std::string& number);

  Stop* GetStopData(const std::string& name);

  void SetDistance(const std::pair<std::string, std::string>& stops,
                   uint32_t distance);

  uint32_t GetDistance(const std::pair<std::string, std::string>& stops);

 private:
  struct PairHasher {
    size_t operator()(const std::pair<Stop*, Stop*>& stops) const {
      return hasher_(stops.first) + hasher_(stops.second) * 37;
    }

   private:
    std::hash<const void*> hasher_;
  };

  std::deque<Bus> buses_;
  std::deque<Stop> stops_;
  std::unordered_map<std::string, Bus*> busesPtr_;
  std::unordered_map<std::string, Stop*> stopsPtr_;
  std::unordered_map<std::pair<Stop*, Stop*>, uint32_t, PairHasher>
      distanceBetweenStops_;

  void SetLengthAndCurvature(Bus& bus, const std::vector<std::string>& stops);
  void SetNumberStopsAndUniqueStops(Bus& bus, const std::vector<std::string>& stops);
  void AddRouteToStops(const std::string& number, const std::vector<std::string>& stops);
};

}  // namespace transport
