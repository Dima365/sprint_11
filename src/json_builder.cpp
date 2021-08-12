#include "json_builder.h"

using namespace json;
using namespace std;

Builder::Builder() {
  nodes_stack_.push_back(&root_);
}

Builder& Builder::StartDict() {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  Node* ptr = new Node{Dict{}};
  nodes_stack_.push_back(ptr);
  keys_.emplace_back(nullopt);
  return *this;
}

Builder& Builder::StartArray() {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  Node* ptr = new Node{Array{}};
  nodes_stack_.push_back(ptr);
  return *this;
}

Builder& Builder::Key(string str) {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  if (nodes_stack_.back()->IsDict()) {
    if (keys_.back()) {
      throw logic_error("Key error"s);
    } else {
      keys_[keys_.size() - 1] = move(str);
    }
  } else {
    throw std::logic_error("Not a dict"s);
  }
  return *this;
}

Builder& Builder::Value(json::Node node) {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  }

  if (nodes_stack_.back()->IsDict()) {
    auto& cont = nodes_stack_.back()->GetValue();
    Dict& dict = get<Dict>(cont);
    if (keys_[keys_.size() - 1] == nullopt) {
      throw logic_error("Key error"s);
    }
    dict[*keys_.back()] = move(node);
    keys_[keys_.size() - 1] = nullopt;

  } else if (nodes_stack_.back()->IsArray()) {
    auto& cont = nodes_stack_.back()->GetValue();
    Array& array = get<Array>(cont);
    array.emplace_back(move(node));

  } else if (node.IsDict()) {
    auto& cont = nodes_stack_.back()->GetValue();
    cont = move(node.AsDict());
    if (nodes_stack_.size() != 1) {
      delete nodes_stack_.back();
    }
    nodes_stack_.pop_back();    
  
  } else if (node.IsArray()) {
    auto& cont = nodes_stack_.back()->GetValue();
    cont = move(node.AsArray());
    if (nodes_stack_.size() != 1) {
      delete nodes_stack_.back();
    }
    nodes_stack_.pop_back();

  } else if (node.IsString()) {
    auto& cont = nodes_stack_.back()->GetValue();
    cont = move(node.AsString());
    if (nodes_stack_.size() != 1) {
      delete nodes_stack_.back();
    }
    nodes_stack_.pop_back();

  } else if (node.IsPureDouble()) {
    auto& cont = nodes_stack_.back()->GetValue();
    cont = node.AsDouble();
    if (nodes_stack_.size() != 1) {
      delete nodes_stack_.back();
    }
    nodes_stack_.pop_back();

  } else if (node.IsInt()) {
    auto& cont = nodes_stack_.back()->GetValue();
    cont = node.AsInt();
    if (nodes_stack_.size() != 1) {
      delete nodes_stack_.back();
    }
    nodes_stack_.pop_back();

  } else if (node.IsBool()) {
    auto& cont = nodes_stack_.back()->GetValue();
    cont = node.AsBool();
    if (nodes_stack_.size() != 1) {
      delete nodes_stack_.back();
    }
    nodes_stack_.pop_back();

  } else if (node.IsNull()) {
    auto& cont = nodes_stack_.back()->GetValue();
    cont = nullptr;
    if (nodes_stack_.size() != 1) {
      delete nodes_stack_.back();
    }
    nodes_stack_.pop_back();
  }
  return *this;
}

Builder& Builder::EndDict() {
  if (nodes_stack_.empty()) {
    throw logic_error("after build"s);
  } else if (!nodes_stack_.back()->IsDict()) {
    throw logic_error("error EndDcit"s);
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
    throw logic_error("error EndArray"s);
  }

  json::Node node = *nodes_stack_.back();
  delete nodes_stack_.back();
  nodes_stack_.pop_back();
  Value(node);
  return *this;
}

Node Builder::Build() {
  if (!nodes_stack_.empty()) {
    throw logic_error("incomplite"s);
  }

  return root_;
}

Builder::~Builder() {
  if (nodes_stack_.size() > 1) {
    for (size_t i = 0; i < nodes_stack_.size() - 1; ++i) {
      if (nodes_stack_.back()) {
        delete nodes_stack_.back();
      }
      nodes_stack_.pop_back();
    }
  }
}