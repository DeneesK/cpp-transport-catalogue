#include "svg.h"

namespace svg {

using namespace std::literals;

std::ostream& operator<<(std::ostream& os, const StrokeLineCap& line_cap) {
    if(line_cap == StrokeLineCap::BUTT) {
        os << "butt";
    } else if(line_cap == StrokeLineCap::ROUND) {
        os << "round";
    } else if(line_cap == StrokeLineCap::SQUARE) {
        os << "square";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& line_join) {
    if(line_join == StrokeLineJoin::ARCS) {
        os << "arcs";
    } else if(line_join == StrokeLineJoin::BEVEL) {
        os << "bevel";
    } else if(line_join == StrokeLineJoin::MITER) {
        os << "miter";
    } else if(line_join == StrokeLineJoin::MITER_CLIP) {
        os << "miter-clip";
    } else if(line_join == StrokeLineJoin::ROUND) {
        os << "round";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Color& color) {
    if (std::holds_alternative<Rgb>(color)) {
        auto c = std::get<Rgb>(color);
        os << "rgb(" << c.red << "," << c.green << "," << c.blue << ")";
    } else if(std::holds_alternative<Rgba>(color)) {
        auto c = std::get<Rgba>(color);
        os << "rgba(" << c.red << "," << c.green << "," << c.blue << "," << c.opacity << ")";
    } else if(std::holds_alternative<std::string>(color)) {
        os << std::get<std::string>(color);
    } else {
        os << "none";
    }
    return os;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    context.RenderIndent();
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(std::move(point));
    return *this;
}

// <polyline points="20,40 22.9389,45.9549 29.5106,46.9098" />
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    context.RenderIndent();
    out << "<polyline points=\"";
    bool is_first = true;
    for (auto point: points_) {
        if (is_first) {
            out << point.x << "," << point.y;
            is_first = false;
            continue;
        }
        out  << " " << point.x << "," << point.y;
    }
    out << "\"";
    RenderAttrs(context.out);
    out << "/>";
}

// ---------- Text ------------------

Text& Text::SetPosition(Point pos) {
    position_ = pos;
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

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetData(std::string data) {
    DoEscapingСharacters(data);
    return *this;
}

void Text::DoEscapingСharacters(std::string& data) {
    std::stringstream out;
    for(char& c: data) {
        if(need_to_escape_.count(c)) {
            out << need_to_escape_.at(c);
            continue;
        }
        out << c;
    }
    data_ = out.str();
} 

// <text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    context.RenderIndent();
    out << "<text"; 
    RenderAttrs(context.out);
    out << " x=\"" << position_.x <<"\"" << " " << "y=\"" << position_.y << "\"" << " ";
    out << "dx=\"" << offset_.x <<"\"" << " " "dy=\"" << offset_.y <<"\"" << " " << "font-size=\"" << font_size_ << "\"";
    if(font_family_ != "") {
        out << " font-family=\"" << font_family_ << "\"";
    }    
    if(font_weight_ != "") {
        out << " font-weight=\"" << font_weight_ << "\"";
    }
    out << ">" << data_;
    out << "</text>";
}

// ---------- Document ------------------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.push_back(std::move(obj));
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
    for(auto&& obj: objects_) {
        obj.get()->Render(out);
    }
    out << "</svg>" << std::endl;
}
}  // namespace svg