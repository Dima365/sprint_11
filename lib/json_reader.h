#pragma once

#include <istream>
#include <ostream>

#include "domain.h"
#include "geo.h"
#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"

class JsonReader {
 public:
  JsonReader(RequestHandler& handler, std::istream& input);
  void Print(std::ostream& output);

 private:
  RequestHandler& handler_;
  json::Document requests_;

  void EnterData(const json::Node& node);
  void AddRoute(const json::Node& node_base_requests);
  void AddDistance(const json::Node& node_base_requests);
  void AddStop(const json::Node& node_base_requests);

  void SetRendererSettings(const json::Node& node_render_settings);

  json::Document GetJsonAnswers();
  json::Node GetNodeStop(const json::Dict& map_state_request);
  json::Node GetNodeBus(const json::Dict& map_state_request);
};
