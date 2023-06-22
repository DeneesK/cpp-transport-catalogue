#pragma once

#include "json_reader.h"
#include "svg.h"
#include "domain.h"

namespace map_render {
struct RenderSettings {
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    int bus_label_font_size;
    std::pair<double, double> bus_label_offset; 
    int stop_label_font_size;
    std::pair<double, double> stop_label_offset;
    double underlayer_width;
    std::vector<svg::Color> color_palette;
    svg::Color underlayer_color;
    svg::StrokeLineCap stroke_linecap = svg::StrokeLineCap::ROUND;
    svg::StrokeLineJoin stroke_linejoin = svg::StrokeLineJoin::ROUND;
};

class MapRender {
public:
    MapRender(RenderSettings settings): settings_(settings)
    {       
    }
private:
    RenderSettings settings_;
};
}