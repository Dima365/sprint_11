#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "domain.h"
#include "geo.h"
#include "svg.h"

namespace renderer {

struct RenderSettings {
  double width;
  double height;
  double padding;
  double line_width;
  double stop_radius;
  int bus_label_font_size;
  svg::Point bus_label_offset;
  int stop_label_font_size;
  svg::Point stop_label_offset;
  svg::Color underlayer_color;
  double underlayer_width;
  std::vector<svg::Color> color_palette;
};

inline const double EPSILON = 1e-6;

bool IsZero(double value);

class SphereProjector {
 public:
  template <typename PointInputIt>
  SphereProjector(PointInputIt points_begin,
                  PointInputIt points_end,
                  double max_width,
                  double max_height,
                  double padding)
      : padding_(padding) {
    if (points_begin == points_end) {
      return;
    }

    const auto [left_it, right_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
    min_lon_ = left_it->lng;
    const double max_lon = right_it->lng;

    const auto [bottom_it, top_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
    const double min_lat = bottom_it->lat;
    max_lat_ = top_it->lat;

    std::optional<double> width_zoom;
    if (!IsZero(max_lon - min_lon_)) {
      width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
    }

    std::optional<double> height_zoom;
    if (!IsZero(max_lat_ - min_lat)) {
      height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }

    if (width_zoom && height_zoom) {
      zoom_coeff_ = std::min(*width_zoom, *height_zoom);
    } else if (width_zoom) {
      zoom_coeff_ = *width_zoom;
    } else if (height_zoom) {
      zoom_coeff_ = *height_zoom;
    }
  }

  svg::Point operator()(Coordinates coords) const;

 private:
  double padding_;
  double min_lon_ = 0;
  double max_lat_ = 0;
  double zoom_coeff_ = 0;
};

struct Route {
  std::string first_stop;
  std::string last_stop;
  Bus* bus;
  bool is_round;
};

class MapRenderer {
 public:
  svg::Document RenderMap();

  void SetRendererSettings(const RenderSettings& settings);
  void SetRoute(const std::string& number, Route&& route);
  void SetCoordinates(const std::string& name, Stop* stop_ptr);

 private:
  RenderSettings settings_;
  std::map<std::string, Route> bus_ptrs_;
  std::map<std::string, Stop*> stop_ptrs_;

  void RenderLinesBetweenStops(svg::Document& doc,
                               const SphereProjector& sphere_projector);
  void RenderRouteNames(svg::Document& doc,
                        const SphereProjector& sphere_projector);
  void RenderStopSymbol(svg::Document& doc,
                        const SphereProjector& sphere_projector);
  void RenderStopNames(svg::Document& doc,
                       const SphereProjector& sphere_projector);

  void SetDefaultSettingsRouteName(svg::Text& text_underlay, svg::Text& text);
  void SetDefaultSettingsStopName(svg::Text& text_underlay, svg::Text& text);
  void SetColor(svg::Text& text, std::vector<svg::Color>::iterator& iter_color);
  void SetName(svg::Text& text, const std::string& name);
  void SetPositionStop(svg::Text& text,
                       const std::string& stop,
                       const SphereProjector& sphere_projector);
  void SetPositionStop(svg::Text& text,
                       const Coordinates coord,
                       const SphereProjector& sphere_projector);
};

}  // namespace renderer
