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

      text.SetFillColor(*iter_color);
      ++iter_color;
      if (iter_color == settings_.color_palette.end()) {
        iter_color = settings_.color_palette.begin();
      }

      text_underlay.SetData(number);
      text.SetData(number);

      const Stop* stop_ptr = stop_ptrs_.at(route.first_stop);
      svg::Point point = sphere_projector(stop_ptr->coord);
      text_underlay.SetPosition(point);
      text.SetPosition(point);

      doc.Add(text_underlay);
      doc.Add(text);

      if (route.first_stop != route.last_stop) {
        stop_ptr = stop_ptrs_.at(route.last_stop);
        point = sphere_projector(stop_ptr->coord);
        text_underlay.SetPosition(point);
        text.SetPosition(point);

        doc.Add(text_underlay);
        doc.Add(text);
      }
    }
  }
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

      svg::Point point = sphere_projector(stop_ptr->coord);
      text_underlay.SetPosition(point);
      text_underlay.SetData(name);
      text.SetPosition(point);
      text.SetData(name);

      doc.Add(text_underlay);
      doc.Add(text);
    }
  }
}

}  // namespace renderer
