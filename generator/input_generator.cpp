#include <iostream>
#include <fstream>
#include <string_view>
#include <vector>
#include <random>
#include <unordered_set>
#include <ctime>
#include <algorithm>
#include <cassert>
#include <unordered_map>

#include "json.h"

using namespace std::literals;

std::string gen_random_string(std::mt19937_64 &rand_gen, size_t max_len,
                              std::string_view edge_chars, std::string_view inner_chars) noexcept {
  assert(max_len > 0);
  assert(!edge_chars.empty());
  assert(!inner_chars.empty());

  std::uniform_int_distribution<size_t> distribution{1, max_len};
  std::string random_string(distribution(rand_gen), '\0');

  distribution.param(std::uniform_int_distribution<size_t>::param_type{size_t{0}, edge_chars.size() - 1});
  random_string.front() = edge_chars[distribution(rand_gen)];
  random_string.back() = edge_chars[distribution(rand_gen)];

  distribution.param(std::uniform_int_distribution<size_t>::param_type{size_t{0}, inner_chars.size() - 1});
  for (size_t i = 1; i + 1 < random_string.size(); ++i) {
    random_string[i] = inner_chars[distribution(rand_gen)];
  }
  return random_string;
}

std::vector<std::string> gen_random_strings(std::mt19937_64 &rand_gen, size_t elements, size_t max_len_size,
                                            std::string_view edge_chars, std::string_view inner_chars) {
  std::vector<std::string> result;
  result.reserve(elements);
  std::unordered_set<std::string_view> unique_random_strings{elements};
  while (unique_random_strings.size() != elements) {
    result.emplace_back(gen_random_string(rand_gen, max_len_size, edge_chars, inner_chars));
    if (!unique_random_strings.emplace(result.back()).second) {
      result.pop_back();
    }
  }
  return result;
}

int gen_input(std::ostream &out, size_t buses_count, size_t stops_count, size_t route_size, size_t request_count_buses, size_t request_count_stops, size_t count_map) {
  constexpr std::string_view edge_chars{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdfghijklmnopqrstuvwxyz123456789"};
  constexpr std::string_view inner_chars{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdfghijklmnopqrstuvwxyz123456789 "};
  std::mt19937_64 rand_gen{static_cast<size_t>(std::time(nullptr))};

  auto buses = gen_random_strings(rand_gen, buses_count, 20, edge_chars, inner_chars);
  auto stops = gen_random_strings(rand_gen, stops_count, 20, edge_chars, inner_chars);

  std::uniform_real_distribution<double> coordinate{1, 2};
  std::uniform_int_distribution<size_t> route_selector{0, 1};
  std::uniform_int_distribution<size_t> stop_selector{0, stops.size() - 1};
  std::uniform_int_distribution<size_t> rout_size_selector{0, route_size};
  std::uniform_int_distribution<int> rout_length{0, 1000001};

  json::Array base_nodes;
  std::unordered_map<std::string, std::unordered_set<std::string>> adjacent_stops;
  while (buses_count || stops_count) {
    std::uniform_int_distribution<size_t> distribution{0, buses_count + stops_count - 1};
    if (stops_count && distribution(rand_gen) >= buses_count) {
      json::Dict node_dict{{"type"s, "Stop"s},
                           {"name"s, stops[--stops_count]},
                           {"latitude"s, coordinate(rand_gen)},
                           {"longitude"s, coordinate(rand_gen)}};
      base_nodes.push_back(json::Node{node_dict});
    } else {
      json::Array bus_stops;
      const char route_type = ">-"[route_selector(rand_gen)];
      const std::string first_stop = stops[stop_selector(rand_gen)];
      bus_stops.push_back(first_stop);
      std::string stop = first_stop;
      const size_t route_size_rand = rout_size_selector(rand_gen);
      for (size_t i = 1; i + 1 < route_size_rand; ++i) {
        std::string adjacent_stop = stops[stop_selector(rand_gen)];
        adjacent_stops[stop].insert(adjacent_stop);
        bus_stops.push_back(json::Node{adjacent_stop});
        stop = adjacent_stop;
      }
      std::string last_stop = (route_type == '>' ? first_stop : stops[stop_selector(rand_gen)]);
      adjacent_stops[stop].insert(last_stop);
      bus_stops.push_back(json::Node{last_stop});
      json::Dict node_dict{{"type"s, "Bus"s},
                           {"name"s, buses[--buses_count]},
                           {"stops"s, bus_stops},
                           {"is_roundtrip"s, route_type == '>' ? true : false}};
      base_nodes.push_back(json::Node{node_dict});
    }
  }

  for (auto& [stop, neighbors] : adjacent_stops) {
    for (json::Node& node : base_nodes) {
      json::Dict node_dict = node.AsMap();
      if (node_dict.at("type"s).AsString() == "Stop"s && 
          node_dict.at("name"s).AsString() == stop) {
        json::Dict distance;
        for (auto& neighbor : neighbors) {
          distance[neighbor] = rout_length(rand_gen);        
        }
        node_dict.insert({"road_distances"s, json::Node{distance}});
        node = json::Node(node_dict);
      }
    }
  }
  /*
  if (request_count_buses > buses.size()) {
    buses.reserve(request_count_buses);
    while (buses.size() != request_count_buses) {
      buses.emplace_back(gen_random_string(rand_gen, 20, edge_chars, inner_chars));
    }
  }
  */
  std::shuffle(buses.begin(), buses.end(), rand_gen);
  //buses.resize(request_count_buses);
  /*
  if (request_count_stops > stops.size()) {
    stops.reserve(request_count_stops);
    while (stops.size() != request_count_stops) {
      stops.emplace_back(gen_random_string(rand_gen, 20, edge_chars, inner_chars));
    }
  }
  */
  std::shuffle(stops.begin(), stops.end(), rand_gen);
  //stops.resize(request_count_stops);

  json::Array stat_nodes;
  int id = 0;
  for (auto& bus : buses) {
    json::Dict node_dict{{"id"s, id},
                         {"type"s, "Bus"s},
                         {"name"s, bus}};
    ++id;
    stat_nodes.push_back(json::Node{node_dict});
  }
  for (auto& stop : stops) {
    json::Dict node_dict{{"id"s, id},
                         {"type"s, "Stop"s},
                         {"name"s, stop}};
    ++id;
    stat_nodes.push_back(json::Node{node_dict});
  }
  for (int i = 0; i < count_map; ++i) {
    json::Dict node_dict{{"id"s, id},
                         {"type"s, "Map"s}};
    ++id;
    stat_nodes.push_back(json::Node{node_dict});
  }
  std::shuffle(stat_nodes.begin(), stat_nodes.end(), rand_gen);

  json::Dict render_nodes{{"width"s, 200},
                          {"height"s, 200},
                          {"padding"s, 30},
                          {"stop_radius"s, 5},
                          {"line_width"s, 14},
                          {"bus_label_font_size"s, 20},
                          {"bus_label_offset"s, json::Array{7, 15}},
                          {"stop_label_font_size"s, 20},
                          {"stop_label_offset"s, json::Array{7, -3}},
                          {"underlayer_color"s, json::Array{255, 255, 255, 0.85}},
                          {"underlayer_width"s, 3},
                          {"color_palette", json::Array{"green"s, json::Array{255, 160, 0}, "red"s, "blue"s}}};

  json::Dict nodes_dict{{"base_requests"s, base_nodes},
                        {"stat_requests"s, stat_nodes}, 
                        {"render_settings", render_nodes}};
  json::Print(json::Document{json::Node{nodes_dict}}, out);

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 7) {
    std::cerr << "Not enough params" << std::endl;
    return 1;
  }
  const size_t buses_count = std::stoul(argv[1]);
  const size_t stops_count = std::stoul(argv[2]);
  const size_t route_size = std::stoul(argv[3]);
  const size_t request_count_buses = std::stoul(argv[4]);
  const size_t request_count_stops = std::stoul(argv[5]);
  const size_t count_map = std::stoul(argv[6]);
  if (argc > 6) {
    std::ofstream input_file{argv[7]};
    return gen_input(input_file, buses_count, stops_count, route_size, request_count_buses, request_count_stops, count_map);
  }
  return gen_input(std::cout, buses_count, stops_count, route_size, request_count_buses, request_count_stops, count_map);
}