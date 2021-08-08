#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg {
using namespace std::literals;

struct Point {
  Point() = default;
  Point(double x, double y) : x(x), y(y) {}
  double x = 0;
  double y = 0;
};

struct Rgb {
  Rgb() = default;

  Rgb(uint8_t rd, uint8_t grn, uint8_t ble) : red(rd), green(grn), blue(ble) {}

  std::uint8_t red = 0;
  std::uint8_t green = 0;
  std::uint8_t blue = 0;
};

struct Rgba {
  Rgba() = default;

  Rgba(uint8_t rd, uint8_t grn, uint8_t ble, double opt)
      : red(rd), green(grn), blue(ble), opacity(opt) {}

  std::uint8_t red = 0;
  std::uint8_t green = 0;
  std::uint8_t blue = 0;
  double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

inline const std::string NoneColor{"none"s};

enum class StrokeLineCap {
  BUTT,
  ROUND,
  SQUARE,
};

std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap);

enum class StrokeLineJoin {
  ARCS,
  BEVEL,
  MITER,
  MITER_CLIP,
  ROUND,
};

std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_cap);

struct RenderContext {
  RenderContext(std::ostream& out) : out(out) {}

  RenderContext(std::ostream& out, int indent_step, int indent = 0)
      : out(out), indent_step(indent_step), indent(indent) {}

  RenderContext Indented() const {
    return {out, indent_step, indent + indent_step};
  }

  void RenderIndent() const {
    for (int i = 0; i < indent; ++i) {
      out.put(' ');
    }
  }

  std::ostream& out;
  int indent_step = 0;
  int indent = 0;
};

class Object {
 public:
  void Render(const RenderContext& context) const;

  virtual ~Object() = default;

 private:
  virtual void RenderObject(const RenderContext& context) const = 0;
};

struct ColorPrinter {
  std::ostream& out;

  void operator()(std::monostate) const { out << NoneColor; }

  void operator()(std::string str) const { out << str; }

  void operator()(Rgb rgb) const {
    out << "rgb("s << unsigned(rgb.red) << ","s << unsigned(rgb.green) << ","s
        << unsigned(rgb.blue) << ")"s;
  }

  void operator()(Rgba rgba) const {
    out << "rgba("s << unsigned(rgba.red) << ","s << unsigned(rgba.green)
        << ","s << unsigned(rgba.blue) << ","s << rgba.opacity << ")"s;
  }
};

std::ostream& operator<<(std::ostream& out, const std::optional<Color>& color);

template <typename Owner>
class PathProps {
 public:
  Owner& SetFillColor(Color color) {
    fill_color_ = std::move(color);
    return AsOwner();
  }
  Owner& SetStrokeColor(Color color) {
    stroke_color_ = std::move(color);
    return AsOwner();
  }

  Owner& SetStrokeWidth(double width) {
    stroke_width_ = width;
    return AsOwner();
  }

  Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
    line_cap_ = line_cap;
    return AsOwner();
  }

  Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
    line_join_ = line_join;
    return AsOwner();
  }

 protected:
  ~PathProps() = default;

  void RenderAttrs(std::ostream& out) const {
    using namespace std::literals;

    if (fill_color_) {
      out << " fill=\""s << *fill_color_ << "\""s;
    }
    if (stroke_color_) {
      out << " stroke=\""s << *stroke_color_ << "\""s;
    }
    if (stroke_width_) {
      out << " stroke-width=\""s << *stroke_width_ << "\""s;
    }
    if (line_cap_) {
      out << " stroke-linecap=\""s << *line_cap_ << "\""s;
    }
    if (line_join_) {
      out << " stroke-linejoin=\""s << *line_join_ << "\""s;
    }
  }

 private:
  Owner& AsOwner() { return static_cast<Owner&>(*this); }

  std::optional<Color> fill_color_;
  std::optional<Color> stroke_color_;
  std::optional<double> stroke_width_;
  std::optional<StrokeLineCap> line_cap_;
  std::optional<StrokeLineJoin> line_join_;
};

class Circle final : public Object, public PathProps<Circle> {
 public:
  Circle& SetCenter(Point center);
  Circle& SetRadius(double radius);

 private:
  void RenderObject(const RenderContext& context) const override;

  Point center_;
  double radius_ = 1.0;
};

class Polyline final : public Object, public PathProps<Polyline> {
 public:
  Polyline& AddPoint(Point point);

 private:
  std::vector<Point> points_;

  void RenderObject(const RenderContext& context) const override;
};

class Text final : public Object, public PathProps<Text> {
 public:
  Text& SetPosition(Point pos);

  Text& SetOffset(Point offset);

  Text& SetFontSize(uint32_t size);

  Text& SetFontFamily(std::string font_family);

  Text& SetFontWeight(std::string font_weight);

  Text& SetData(std::string data);

 private:
  Point pos_;
  Point offset_;
  uint32_t font_size_ = 1;
  std::optional<std::string> font_family_;
  std::optional<std::string> font_weight_;
  std::string text_ = "";

  void RenderObject(const RenderContext& context) const override;

  void ConvertSVG(std::string& text);
};

class ObjectContainer {
 public:
  template <typename Obj>
  void Add(Obj obj) {
    AddPtr(std::make_unique<Obj>(std::move(obj)));
  }

  virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

  virtual ~ObjectContainer() = default;
};

class Drawable {
 public:
  virtual void Draw(ObjectContainer& container) const = 0;

  virtual ~Drawable() = default;
};

class Document : public ObjectContainer {
 public:
  void AddPtr(std::unique_ptr<Object>&& obj) override;

  void Render(std::ostream& out) const;

 private:
  std::vector<std::unique_ptr<Object>> objects_;
};

}  // namespace svg

namespace shapes {

svg::Polyline CreateStar(svg::Point center,
                         double outer_rad,
                         double inner_rad,
                         int num_rays);

class Triangle : public svg::Drawable {
 public:
  Triangle(svg::Point p1, svg::Point p2, svg::Point p3);

  void Draw(svg::ObjectContainer& container) const override;

 private:
  svg::Point p1_, p2_, p3_;
};

class Star : public svg::Drawable {
 public:
  Star(svg::Point center,
       double outer_radius,
       double inner_radius,
       int num_rays);

  void Draw(svg::ObjectContainer& container) const override;

 private:
  svg::Point center_;
  double outer_radius_;
  double inner_radius_;
  int num_rays_;
};

class Snowman : public svg::Drawable {
 public:
  Snowman(svg::Point head_center, double head_radius);

  void Draw(svg::ObjectContainer& container) const override;

 private:
  svg::Point head_center_;
  double head_radius_;
};

}  // namespace shapes

template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin,
                 DrawableIterator end,
                 svg::ObjectContainer& target) {
  for (auto it = begin; it != end; ++it) {
    (*it)->Draw(target);
  }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
  using namespace std;
  DrawPicture(begin(container), end(container), target);
}
