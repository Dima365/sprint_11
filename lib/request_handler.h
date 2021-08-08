#pragma once

#include <string>
#include <vector>

#include "domain.h"
#include "geo.h"
#include "map_renderer.h"
#include "svg.h"
#include "transport_catalogue.h"

class RequestHandler {
 public:
  RequestHandler(transport::Catalogue& tc, renderer::MapRenderer& renderer);

  void AddRoute(const std::string& number,
                std::vector<std::string>&& stops,
                bool is_round);

  void AddStop(const std::string& name, Coordinates coord);

  void SetDistance(const std::pair<std::string, std::string>& stops,
                   uint32_t distance);

  void SetRendererSettings(renderer::RenderSettings settings);

  Bus* GetBusData(const std::string& number);

  Stop* GetStopData(const std::string& name);

  svg::Document RenderMap() const;

 private:
  transport::Catalogue& tc_;
  renderer::MapRenderer& renderer_;
};
