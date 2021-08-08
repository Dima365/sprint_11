#include "request_handler.h"

RequestHandler::RequestHandler(transport::Catalogue& tc,
                               renderer::MapRenderer& renderer)
    : tc_(tc), renderer_(renderer) {}

void RequestHandler::AddRoute(const std::string& number,
                              std::vector<std::string>&& stops,
                              bool is_round) {
  renderer::Route route;
  if (!stops.empty()) {
    route.first_stop = stops.front();
    route.last_stop = stops.back();
    if (!is_round) {
      int stops_size = int(stops.size());
      for (int i = stops_size - 2; i >= 0; --i) {
        stops.push_back(stops[i]);
      }
    }
  }
  tc_.AddRoute(number, move(stops));
  route.is_round = is_round;
  route.bus = tc_.GetBusData(number);
  renderer_.SetRoute(number, std::move(route));
}

void RequestHandler::AddStop(const std::string& name, Coordinates coord) {
  tc_.AddStop(name, coord);
  Stop* stop_ptr = tc_.GetStopData(name);
  renderer_.SetCoordinates(name, stop_ptr);
}

void RequestHandler::SetDistance(
    const std::pair<std::string, std::string>& stops,
    uint32_t distance) {
  tc_.SetDistance(stops, distance);
}

void RequestHandler::SetRendererSettings(renderer::RenderSettings& settings) {
  renderer_.SetRendererSettings(settings);
}

Bus* RequestHandler::GetBusData(const std::string& number) {
  return tc_.GetBusData(number);
}

Stop* RequestHandler::GetStopData(const std::string& name) {
  return tc_.GetStopData(name);
}

svg::Document RequestHandler::RenderMap() const {
  return renderer_.RenderMap();
}
