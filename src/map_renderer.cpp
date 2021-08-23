#include "map_renderer.h"

#include <iterator>
#include <memory>

using namespace std;

namespace renderer {

bool IsZero(double value) {
  return std::abs(value) < EPSILON;
}

svg::Point SphereProjector::operator()(Coordinates coords) const {
  return {(coords.lng - min_lon_) * zoom_coeff_ + padding_,
          (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
}

svg::Document MapRenderer::RenderMap() {
  vector<Coordinates> coords;
  coords.reserve(stop_ptrs_.size());
  for (const auto& [name, stop_ptr] : stop_ptrs_) {
    if (!stop_ptr->buses.empty()) {
      coords.push_back(stop_ptr->coord);
    }
  }
  SphereProjector sphere_projector(coords.begin(), coords.end(),
                                   settings_.width, settings_.height,
                                   settings_.padding);

  svg::Document doc;

  RenderLinesBetweenStops(doc, sphere_projector);
  RenderRouteNames(doc, sphere_projector);
  RenderStopSymbol(doc, sphere_projector);
  RenderStopNames(doc, sphere_projector);

  return doc;
}

void MapRenderer::SetRendererSettings(const RenderSettings& settings) {
  settings_ = settings;
}

void MapRenderer::SetRoute(const string& number, Route&& route) {
  bus_ptrs_[number] = move(route);
}

void MapRenderer::SetCoordinates(const string& name, Stop* stop_ptr) {
  stop_ptrs_[name] = stop_ptr;
}

void MapRenderer::RenderLinesBetweenStops(
    svg::Document& doc,
    const SphereProjector& sphere_projector) {
  vector<svg::Color>::iterator iter_color = settings_.color_palette.begin();
  for (const auto& [number, route] : bus_ptrs_) {
    if (!route.bus->stops.empty()) {
      svg::Polyline line;
      line.SetFillColor(svg::NoneColor);
      line.SetStrokeWidth(settings_.line_width);
      line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
      line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

      line.SetStrokeColor(*iter_color);
      ++iter_color;
      if (iter_color == settings_.color_palette.end()) {
        iter_color = settings_.color_palette.begin();
      }

      for (const string& stop : route.bus->stops) {
        Stop* stop_ptr = stop_ptrs_.at(stop);
        svg::Point point = sphere_projector(stop_ptr->coord);
        line.AddPoint(point);
      }

      doc.Add(line);
    }
  }
}

void MapRenderer::RenderRouteNames(svg::Document& doc,
                                   const SphereProjector& sphere_projector) {
  vector<svg::Color>::iterator iter_color = settings_.color_palette.begin();
  for (const auto& [number, route] : bus_ptrs_) {
    if (!route.bus->stops.empty()) {
      svg::Text text_underlay;
      svg::Text text;
      SetDefaultSettingsRouteName(text_underlay, text);
      SetColor(text, iter_color);
      SetName(text_underlay, number);
      SetName(text, number);
      SetPositionStop(text_underlay, route.first_stop, sphere_projector);
      SetPositionStop(text, route.first_stop, sphere_projector);
      doc.Add(text_underlay);
      doc.Add(text);

      if (route.first_stop != route.last_stop) {
        SetPositionStop(text_underlay, route.last_stop, sphere_projector);
        SetPositionStop(text, route.last_stop, sphere_projector);
        doc.Add(text_underlay);
        doc.Add(text);
      }
    }
  }
}

void MapRenderer::SetDefaultSettingsRouteName(svg::Text& text_underlay, svg::Text& text) {
  text_underlay.SetOffset(settings_.bus_label_offset);
  text_underlay.SetFontSize(settings_.bus_label_font_size);
  text_underlay.SetFontWeight("bold"s);
  text_underlay.SetFontFamily("Verdana"s);
  text_underlay.SetFillColor(settings_.underlayer_color);
  text_underlay.SetStrokeColor(settings_.underlayer_color);
  text_underlay.SetStrokeWidth(settings_.underlayer_width);
  text_underlay.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
  text_underlay.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

  text.SetOffset(settings_.bus_label_offset);
  text.SetFontSize(settings_.bus_label_font_size);
  text.SetFontFamily("Verdana"s);
  text.SetFontWeight("bold"s);
}

void MapRenderer::SetColor(svg::Text& text,
                           vector<svg::Color>::iterator& iter_color) {
  text.SetFillColor(*iter_color);
  ++iter_color;
  if (iter_color == settings_.color_palette.end()) {
    iter_color = settings_.color_palette.begin();
  }
}

void MapRenderer::SetName(svg::Text& text, const string& name) {
  text.SetData(name);
}

void MapRenderer::SetPositionStop(svg::Text& text,
                                  const string& stop,
                                  const SphereProjector& sphere_projector) {
  const Stop* stop_ptr = stop_ptrs_.at(stop);
  svg::Point point = sphere_projector(stop_ptr->coord);
  text.SetPosition(point);
}

void MapRenderer::RenderStopSymbol(svg::Document& doc,
                                   const SphereProjector& sphere_projector) {
  for (const auto& [name, stop_ptr] : stop_ptrs_) {
    if (!stop_ptr->buses.empty()) {
      svg::Circle circle;
      svg::Point point = sphere_projector(stop_ptr->coord);
      circle.SetCenter(point);
      circle.SetRadius(settings_.stop_radius);
      circle.SetFillColor("white"s);
      doc.Add(circle);
    }
  }
}

void MapRenderer::RenderStopNames(svg::Document& doc,
                                  const SphereProjector& sphere_projector) {
  for (const auto& [name, stop_ptr] : stop_ptrs_) {
    if (!stop_ptr->buses.empty()) {
      svg::Text text_underlay;
      svg::Text text;
      SetDefaultSettingsStopName(text_underlay, text);
      SetName(text_underlay, name);
      SetName(text, name);
      SetPositionStop(text_underlay, stop_ptr->coord, sphere_projector);
      SetPositionStop(text, stop_ptr->coord, sphere_projector);
      doc.Add(text_underlay);
      doc.Add(text);
    }
  }
}

void MapRenderer::SetDefaultSettingsStopName(svg::Text& text_underlay,
                                             svg::Text& text) {
  text_underlay.SetOffset(settings_.stop_label_offset);
  text_underlay.SetFontSize(settings_.stop_label_font_size);
  text_underlay.SetFontFamily("Verdana"s);
  text_underlay.SetFillColor(settings_.underlayer_color);
  text_underlay.SetStrokeColor(settings_.underlayer_color);
  text_underlay.SetStrokeWidth(settings_.underlayer_width);
  text_underlay.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
  text_underlay.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

  text.SetFillColor("black"s);
  text.SetOffset(settings_.stop_label_offset);
  text.SetFontSize(settings_.stop_label_font_size);
  text.SetFontFamily("Verdana"s);
}

void MapRenderer::SetPositionStop(svg::Text& text,
                     const Coordinates coord,
                     const SphereProjector& sphere_projector) {
  svg::Point point = sphere_projector(coord);
  text.SetPosition(point);
}

}  // namespace renderer
