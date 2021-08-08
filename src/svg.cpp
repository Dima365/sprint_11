#include "svg.h"

using namespace std::literals;
using namespace std;

namespace svg {

std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_join) {
  switch (line_join) {
    case StrokeLineJoin::ARCS:
      return (out << "arcs"s);
    case StrokeLineJoin::BEVEL:
      return (out << "bevel"s);
    case StrokeLineJoin::MITER:
      return (out << "miter"s);
    case StrokeLineJoin::MITER_CLIP:
      return (out << "miter-clip"s);
    case StrokeLineJoin::ROUND:
      return (out << "round"s);
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap) {
  switch (line_cap) {
    case StrokeLineCap::BUTT:
      return (out << "butt"s);
    case StrokeLineCap::ROUND:
      return (out << "round"s);
    case StrokeLineCap::SQUARE:
      return (out << "square"s);
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const std::optional<Color>& color) {
  std::visit(ColorPrinter{out}, *color);
  return out;
}

void Object::Render(const RenderContext& context) const {
  context.RenderIndent();

  // Делегируем вывод тега своим подклассам
  RenderObject(context);

  context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center) {
  center_ = center;
  return *this;
}

Circle& Circle::SetRadius(double radius) {
  radius_ = radius;
  return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
  auto& out = context.out;
  out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
  out << "r=\""sv << radius_ << "\""sv;
  RenderAttrs(out);
  out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
  points_.push_back(point);
  return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
  auto& out = context.out;
  out << "<polyline points=\""s;
  for (size_t i = 0; i < points_.size(); ++i) {
    if (i == points_.size() - 1) {
      out << points_[i].x << ","s << points_[i].y;
    } else {
      out << points_[i].x << ","s << points_[i].y << " "s;
    }
  }
  out << "\""s;
  RenderAttrs(out);
  out << "/>"s;
}

// ---------- Text ------------------
Text& Text::SetPosition(Point pos) {
  pos_ = pos;
  return *this;
}

Text& Text::SetOffset(Point offset) {
  offset_ = offset;
  return *this;
}

Text& Text::SetFontSize(uint32_t size) {
  font_size_ = size;
  return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
  font_family_ = font_family;
  return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
  font_weight_ = font_weight;
  return *this;
}

Text& Text::SetData(std::string data) {
  ConvertSVG(data);
  text_ = data;
  return *this;
}

void Text::RenderObject(const RenderContext& context) const {
  auto& out = context.out;
  out << "<text"s;
  RenderAttrs(out);
  out << " x=\""s << pos_.x << "\" y=\""s << pos_.y << "\" dx=\""s << offset_.x
      << "\" dy=\""s << offset_.y << "\" font-size=\""s << font_size_ << "\""s;
  if (font_family_) {
    out << " font-family=\""s << *font_family_ << "\""s;
  }
  if (font_weight_) {
    out << " font-weight=\""s << *font_weight_ << "\""s;
  }
  out << ">"s << text_ << "</text>"s;
}

void Text::ConvertSVG(std::string& text) {
  size_t pos = 0;
  size_t size_text = text.size();
  for (size_t i = 0; i < size_text; ++i) {
    switch (text[pos]) {
      case '"':
        text.replace(pos, 1, "&quot;"s);
        pos += 6;
        break;
      case '<':
        text.replace(pos, 1, "&lt;"s);
        pos += 4;
        break;
      case '>':
        text.replace(pos, 1, "&gt;"s);
        pos += 4;
        break;
      case '\'':
        text.replace(pos, 1, "&apos;"s);
        pos += 6;
        break;
      case '&':
        text.replace(pos, 1, "&amp;"s);
        pos += 5;
        break;
      default:
        ++pos;
        break;
    }
  }
}

void Document::AddPtr(std::unique_ptr<Object>&& obj) {
  objects_.push_back(move(obj));
}

void Document::Render(ostream& out) const {
  RenderContext ctx(out, 2, 2);
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"s << endl;
  out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"s << endl;
  for (auto& obj : objects_) {
    obj->Render(ctx);
  }
  out << "</svg>"s << endl;
}

}  // namespace svg

namespace shapes {

svg::Polyline CreateStar(svg::Point center,
                         double outer_rad,
                         double inner_rad,
                         int num_rays) {
  using namespace svg;
  Polyline polyline;
  for (int i = 0; i <= num_rays; ++i) {
    double angle = 2 * M_PI * (i % num_rays) / num_rays;
    polyline.AddPoint(
        {center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
    if (i == num_rays) {
      break;
    }
    angle += M_PI / num_rays;
    polyline.AddPoint(
        {center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
  }
  return polyline;
}

Triangle::Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
    : p1_(p1), p2_(p2), p3_(p3) {}

void Triangle::Draw(svg::ObjectContainer& container) const {
  container.Add(
      svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
}

Star::Star(svg::Point center,
           double outer_radius,
           double inner_radius,
           int num_rays)
    : center_(center),
      outer_radius_(outer_radius),
      inner_radius_(inner_radius),
      num_rays_(num_rays) {}

void Star::Draw(svg::ObjectContainer& container) const {
  svg::Polyline polyline;
  polyline =
      shapes::CreateStar(center_, outer_radius_, inner_radius_, num_rays_);
  polyline.SetFillColor("red"s).SetStrokeColor("black"s);
  container.Add(polyline);
}

Snowman::Snowman(svg::Point head_center, double head_radius)
    : head_center_(head_center), head_radius_(head_radius) {}

void Snowman::Draw(svg::ObjectContainer& container) const {
  svg::Circle legs;
  legs.SetCenter({head_center_.x, head_center_.y + 5 * head_radius_});
  legs.SetRadius(head_radius_ * 2);
  legs.SetFillColor("rgb(240,240,240)"s).SetStrokeColor("black"s);
  container.Add(legs);
  svg::Circle body;
  body.SetCenter({head_center_.x, head_center_.y + 2 * head_radius_});
  body.SetRadius(head_radius_ * 1.5);
  body.SetFillColor("rgb(240,240,240)"s).SetStrokeColor("black"s);
  container.Add(body);
  svg::Circle head;
  head.SetCenter(head_center_);
  head.SetRadius(head_radius_);
  head.SetFillColor("rgb(240,240,240)"s).SetStrokeColor("black"s);
  container.Add(head);
}

}  // namespace shapes
