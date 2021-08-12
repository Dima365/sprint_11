#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include <optional>

#include "json.h"

namespace json {

class Builder {
 public:
  Builder();

  Builder& StartDict();
  Builder& StartArray();

  Builder& Key(std::string str);

  Builder& Value(json::Node node);

  Builder& EndDict();
  Builder& EndArray();
  
  Node Build();

  ~Builder();

 private:
  json::Node root_;
  std::vector<std::optional<std::string>> keys_;
  std::vector<json::Node*> nodes_stack_;
};

}  // namespace json