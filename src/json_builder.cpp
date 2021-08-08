#include "json_builder.h"

using namespace json;
using namespace std;

Builder::Builder() {
  nodes_stack_.push_back(&root_);
}

DictItemContext Builder::StartDict() {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  Node* ptr = new Node{Dict{}};
  nodes_stack_.push_back(ptr);
  keys_.push_back(nullopt);
  return DictItemContext{*this};
}

ArrayItemContext Builder::StartArray() {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  Node* ptr = new Node{Array{}};
  nodes_stack_.push_back(ptr);
  return ArrayItemContext{*this};
}

Builder& Builder::Key(string str) {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  if (nodes_stack_.back()->IsDict()) {
    if (keys_.back()) {
      throw logic_error("key"s);
    } else {
      keys_.back() = move(str);
    }
  } else {
    throw std::logic_error("not a dict"s);
  }
  return *this;
}

Builder& Builder::Value(json::Node node) {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  if (nodes_stack_.back()->IsDict()) {
    if (!keys_.back()) {
      throw logic_error("key"s);
    }

    auto& value = nodes_stack_.back()->GetValue();
    Dict& dict = get<Dict>(value);
    dict[*keys_.back()] = move(node);
    keys_.back() = nullopt;

  } else if (nodes_stack_.back()->IsArray()) {
    auto& value = nodes_stack_.back()->GetValue();
    Array& array = get<Array>(value);
    array.emplace_back(move(node));

  } else if (node.IsDict()) {
    auto& value = nodes_stack_.back()->GetValue();
    value = move(node.AsDict());
    nodes_stack_.pop_back();

  } else if (node.IsArray()) {
    auto& value = nodes_stack_.back()->GetValue();
    value = move(node.AsArray());
    nodes_stack_.pop_back();

  } else if (node.IsString()) {
    auto& value = nodes_stack_.back()->GetValue();
    value = move(node.AsString());
    nodes_stack_.pop_back();

  } else if (node.IsPureDouble()) {
    auto& value = nodes_stack_.back()->GetValue();
    value = node.AsDouble();
    nodes_stack_.pop_back();

  } else if (node.IsInt()) {
    auto& value = nodes_stack_.back()->GetValue();
    value = node.AsInt();
    nodes_stack_.pop_back();

  } else if (node.IsBool()) {
    auto& value = nodes_stack_.back()->GetValue();
    value = node.AsBool();
    nodes_stack_.pop_back();

  } else if (node.IsNull()) {
    auto& value = nodes_stack_.back()->GetValue();
    value = nullptr;
    nodes_stack_.pop_back();
  }
  return *this;
}

Builder& Builder::EndDict() {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  } else if (!nodes_stack_.back()->IsDict()) {
    throw logic_error("EndDcit"s);
  }

  json::Node node = *nodes_stack_.back();
  delete nodes_stack_.back();
  nodes_stack_.pop_back();
  keys_.pop_back();
  Value(node);
  return *this;
}

Builder& Builder::EndArray() {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  } else if (!nodes_stack_.back()->IsArray()) {
    throw logic_error("EndArray"s);
  }

  json::Node node = *nodes_stack_.back();
  delete nodes_stack_.back();
  nodes_stack_.pop_back();
  Value(node);
  return *this;
}

Node Builder::Build() {
  if (!nodes_stack_.empty()) {
    throw logic_error("incomplete"s);
  }

  return root_;
}

Builder::~Builder() {
  if (nodes_stack_.size() > 1) {
    for (size_t i = 0; i < nodes_stack_.size() - 1; ++i) {
      if (nodes_stack_.back()) {
        delete nodes_stack_.back();
      }
    }
  }
}

DictItemContext::DictItemContext(Builder& builder) : builder_(builder) {}

KeyItemContext DictItemContext::Key(std::string str) {
  return KeyItemContext{builder_.Key(str)};
}

Builder& DictItemContext::EndDict() {
  return builder_.EndDict();
}

KeyItemContext::KeyItemContext(Builder& builder) : builder_(builder) {}

DictItemContext KeyItemContext::Value(json::Node node) {
  return DictItemContext{builder_.Value(node)};
}

DictItemContext KeyItemContext::StartDict() {
  return builder_.StartDict();
}

ArrayItemContext KeyItemContext::StartArray() {
  return builder_.StartArray();
}

ArrayItemContext::ArrayItemContext(Builder& builder) : builder_(builder) {}

ArrayItemContext ArrayItemContext::Value(json::Node node) {
  return ArrayItemContext{builder_.Value(node)};
}

DictItemContext ArrayItemContext::StartDict() {
  return builder_.StartDict();
}

ArrayItemContext ArrayItemContext::StartArray() {
  return builder_.StartArray();
}

Builder& ArrayItemContext::EndArray() {
  return builder_.EndArray();
}