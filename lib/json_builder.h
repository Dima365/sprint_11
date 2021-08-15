#include <algorithm>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "json.h"

namespace json {

class ArrayItemContext;
class KeyItemContext;
class DictItemContext;

class Builder {
 public:
  Builder();

  DictItemContext StartDict();
  ArrayItemContext StartArray();

  Builder& Key(std::string str);

  Builder& Value(json::Node node);

  Builder& EndArray();
  Builder& EndDict();

  Node Build();

  ~Builder();

 private:
  json::Node root_;
  std::vector<std::optional<std::string>> keys_;
  std::vector<json::Node*> nodes_stack_;
};

class DictItemContext {
 public:
  explicit DictItemContext(Builder& builder);

  KeyItemContext Key(std::string str);

  Builder& EndDict();

 private:
  Builder& builder_;
};

class KeyItemContext {
 public:
  explicit KeyItemContext(Builder& builder);

  DictItemContext Value(json::Node node);

  DictItemContext StartDict();

  ArrayItemContext StartArray();

 private:
  Builder& builder_;
};

class ArrayItemContext {
 public:
  explicit ArrayItemContext(Builder& builder);

  ArrayItemContext Value(json::Node node);

  DictItemContext StartDict();

  ArrayItemContext StartArray();

  Builder& EndArray();

 private:
  Builder& builder_;
};

}  // namespace json