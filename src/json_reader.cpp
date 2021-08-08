#include <set>
#include <vector>

#include "json_reader.h"

using namespace std;

JsonReader::JsonReader(RequestHandler& handler, istream& input)
    : handler_(handler), requests_(json::Load(input)) {
  EnterData(requests_.GetRoot());
}

void JsonReader::Print(ostream& output) {
  json::Print(GetJsonAnswers(), output);
}

void JsonReader::EnterData(const json::Node& node) {
  for (const auto& [type_requests, node_tmp] : node.AsDict()) {
    if (type_requests == "base_requests"s) {
      AddStop(node_tmp);
      AddDistance(node_tmp);
      AddRoute(node_tmp);
    } else if (type_requests == "render_settings"s) {
      SetRendererSettings(node_tmp);
    }
  }
}

void JsonReader::AddStop(const json::Node& node_base_requests) {
  for (const json::Node& node_base_request : node_base_requests.AsArray()) {
    const json::Dict& map_base_request = node_base_request.AsDict();
    if (map_base_request.at("type"s).AsString() == "Stop"s) {
      const string& name = map_base_request.at("name"s).AsString();
      double latitude = map_base_request.at("latitude"s).AsDouble();
      double longitude = map_base_request.at("longitude"s).AsDouble();
      Coordinates coord{latitude, longitude};
      handler_.AddStop(name, coord);
    }
  }
}

void JsonReader::AddDistance(const json::Node& node_base_requests) {
  for (const json::Node& node_base_request : node_base_requests.AsArray()) {
    const json::Dict& map_base_request = node_base_request.AsDict();
    if (map_base_request.at("type"s).AsString() == "Stop"s) {
      const string& name = map_base_request.at("name"s).AsString();
      const auto iter = map_base_request.find("road_distances"s);
      if (iter != map_base_request.end()) {
        const json::Node& node_stops_dist = iter->second;
        for (const auto& [stop, distance] : node_stops_dist.AsDict()) {
          pair<string, string> stops{name, stop};
          handler_.SetDistance(stops, distance.AsInt());
        }
      }
    }
  }
}

void JsonReader::AddRoute(const json::Node& node_base_requests) {
  for (const json::Node& node_base_request : node_base_requests.AsArray()) {
    const json::Dict& map_base_request = node_base_request.AsDict();
    vector<string> stops;
    if (map_base_request.at("type"s).AsString() == "Bus"s) {
      const string& name = map_base_request.at("name"s).AsString();
      for (const json::Node& node_stop :
           map_base_request.at("stops"s).AsArray()) {
        stops.push_back(node_stop.AsString());
      }
      bool is_roundtrip = map_base_request.at("is_roundtrip"s).AsBool();
      handler_.AddRoute(name, move(stops), is_roundtrip);
    }
  }
}

svg::Color Conver2Color(const json::Node& node) {
  svg::Color color;
  if (node.IsArray()) {
    const json::Array& array = node.AsArray();
    if (array.size() == 3) {
      color =
          svg::Rgb{uint8_t(array.at(0).AsInt()), uint8_t(array.at(1).AsInt()),
                   uint8_t(array.at(2).AsInt())};
    } else if (array.size() == 4) {
      color =
          svg::Rgba{uint8_t(array.at(0).AsInt()), uint8_t(array.at(1).AsInt()),
                    uint8_t(array.at(2).AsInt()), array.at(3).AsDouble()};
    }
  } else if (node.IsString()) {
    color = node.AsString();
  }
  return color;
}

void JsonReader::SetRendererSettings(const json::Node& node_render_settings) {
  renderer::RenderSettings settings;
  for (const auto& [name_param, value] : node_render_settings.AsDict()) {
    if (name_param == "width"s) {
      settings.width = value.AsDouble();

    } else if (name_param == "height"s) {
      settings.height = value.AsDouble();

    } else if (name_param == "padding"s) {
      settings.padding = value.AsDouble();

    } else if (name_param == "stop_radius"s) {
      settings.stop_radius = value.AsDouble();

    } else if (name_param == "line_width"s) {
      settings.line_width = value.AsDouble();

    } else if (name_param == "bus_label_font_size"s) {
      settings.bus_label_font_size = value.AsInt();

    } else if (name_param == "bus_label_offset"s) {
      json::Array x_y = value.AsArray();
      settings.bus_label_offset.x = x_y.front().AsDouble();
      settings.bus_label_offset.y = x_y.back().AsDouble();

    } else if (name_param == "stop_label_font_size"s) {
      settings.stop_label_font_size = value.AsInt();

    } else if (name_param == "stop_label_offset"s) {
      json::Array x_y = value.AsArray();
      settings.stop_label_offset.x = x_y.front().AsDouble();
      settings.stop_label_offset.y = x_y.back().AsDouble();

    } else if (name_param == "underlayer_color"s) {
      settings.underlayer_color = Conver2Color(value);

    } else if (name_param == "underlayer_width"s) {
      settings.underlayer_width = value.AsDouble();

    } else if (name_param == "color_palette"s) {
      for (const json::Node& node : value.AsArray()) {
        settings.color_palette.push_back(Conver2Color(node));
      }
    }
  }
  handler_.SetRendererSettings(settings);
}

json::Node JsonReader::GetNodeBus(const json::Dict& map_state_request) {
  const string& name = map_state_request.at("name"s).AsString();
  int request_id = map_state_request.at("id"s).AsInt();
  Bus* bus_ptr = handler_.GetBusData(name);
  if (bus_ptr != nullptr) {
    json::Node node{json::Dict{{"curvature"s, bus_ptr->curvature},
                               {"request_id"s, request_id},
                               {"route_length"s, double(bus_ptr->routeLength)},
                               {"stop_count"s, bus_ptr->stopsOnRoute},
                               {"unique_stop_count"s, bus_ptr->uniqueStops}}};
    return node;
  } else {
    json::Node node{json::Dict{{"request_id"s, request_id},
                               {"error_message"s, "not found"s}}};
    return node;
  }
}

json::Node JsonReader::GetNodeStop(const json::Dict& map_state_request) {
  const string& name = map_state_request.at("name"s).AsString();
  Stop* stop_ptr = handler_.GetStopData(name);
  int request_id = map_state_request.at("id"s).AsInt();
  if (stop_ptr != nullptr) {
    const set<string>& buses = stop_ptr->buses;
    json::Array array_buses{buses.begin(), buses.end()};
    json::Node node{
        json::Dict{{"buses"s, array_buses}, {"request_id"s, request_id}}};
    return node;
  } else {
    json::Node node(json::Dict{{"request_id"s, request_id},
                               {"error_message"s, "not found"s}});
    return node;
  }
}

json::Document JsonReader::GetJsonAnswers() {
  json::Array nodes;
  const json::Node& node_dict = requests_.GetRoot();
  for (const auto& [type_requests, node_state_requests] : node_dict.AsDict()) {
    if (type_requests == "stat_requests"s) {
      for (const json::Node& node_state_request :
           node_state_requests.AsArray()) {
        const json::Dict& map_state_request = node_state_request.AsDict();
        if (map_state_request.at("type"s).AsString() == "Stop"s) {
          nodes.push_back(GetNodeStop(map_state_request));
        } else if (map_state_request.at("type"s).AsString() == "Bus"s) {
          nodes.push_back(GetNodeBus(map_state_request));
        } else if (map_state_request.at("type"s).AsString() == "Map"s) {
          ostringstream out;
          svg::Document doc = handler_.RenderMap();
          doc.Render(out);
          int request_id = map_state_request.at("id"s).AsInt();
          string str = out.str();
          if (str[str.size() - 1] == '\n') {
            str.resize(str.size() - 1);
          }
          json::Node node{
              json::Dict{{"map"s, str}, {"request_id"s, request_id}}};
          nodes.push_back(node);
        }
      }
    }
  }
  return json::Document{json::Node{nodes}};
}
