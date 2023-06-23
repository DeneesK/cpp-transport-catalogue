#pragma once

#include "request_handler.h"

namespace map_render {

struct RenderSettings;

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
    MapRender(RenderSettings settings);
    void Render(request_handler::RequestHandler& rh, std::ostream& out);
private:
    RenderSettings settings_;

    void LineRender(svg::Document& doc, std::vector<domain::BusRoute> routes, domain::SphereProjector& project);
    void BusNameRender(svg::Document& doc, std::vector<domain::BusRoute> routes, domain::SphereProjector& project);
    void StopLableRender(svg::Document& doc, std::vector<domain::Stop> stops, domain::SphereProjector& project);
    void StopNameRender(svg::Document& doc, std::vector<domain::Stop> stops, domain::SphereProjector& project);
};
}