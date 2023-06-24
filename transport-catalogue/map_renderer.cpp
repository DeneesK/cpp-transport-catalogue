#include "map_renderer.h"

#include <unordered_set>
#include <algorithm>

namespace map_render {

MapRender::MapRender(RenderSettings settings): settings_(settings) {}

void MapRender::Render(request_handler::RequestHandler& rh, std::ostream& out) {
    std::vector<domain::BusRoute> routes = rh.GetAllRoutes();
    std::unordered_set<geo::Coordinates, geo::CoordinatesHasher> unique_cords;
    auto stops = rh.GetAllStops();
    
    for(auto route: routes) {
        unique_cords.insert(route.coordinates.begin(), route.coordinates.end());
    }

    domain::SphereProjector project(unique_cords.begin(), unique_cords.end(), settings_.width, settings_.height, settings_.padding);
    svg::Document doc;

    sort(routes.begin(), routes.end(), [](auto& lhs, auto& rhs){return std::lexicographical_compare(lhs.name.begin(), lhs.name.end(),
                                                                                                    rhs.name.begin(), rhs.name.end());});
    
    LineRender(doc, routes, project);
    BusNameRender(doc, routes, project);
    StopLableRender(doc, stops, project);
    StopNameRender(doc, stops, project);
    doc.Render(out);
}

void MapRender::LineRender(svg::Document& doc, std::vector<domain::BusRoute> routes, domain::SphereProjector& project) {
    int i = 0;
    for(auto route: routes) {
        svg::Polyline line;
        if (!route.coordinates.empty()) {
            for(auto stop: route.coordinates) {
                line.AddPoint(project(stop));
            }
            if(!route.is_circular && route.coordinates.size() >= 2) {
                for(auto it = route.coordinates.rbegin()+1; it != route.coordinates.rend(); ++it) {
                        line.AddPoint(project(*it));
                    }
            }
            }
            line.SetFillColor("none");
            line.SetStrokeColor(settings_.color_palette[i % settings_.color_palette.size()]);
            line.SetStrokeWidth(settings_.line_width);
            line.SetStrokeLineCap(settings_.stroke_linecap);
            line.SetStrokeLineJoin(settings_.stroke_linejoin);
            doc.Add(line);
            ++i;
        }
}

void MapRender::BusNameRender(svg::Document& doc, std::vector<domain::BusRoute> routes, domain::SphereProjector& project){
    int i = 0;
    for(auto route: routes) {
        if (!route.coordinates.empty()) {
            svg::Text text;
            svg::Text underlayer;
            underlayer.SetFillColor(settings_.underlayer_color);
            underlayer.SetStrokeColor(settings_.underlayer_color);
            underlayer.SetStrokeWidth(settings_.underlayer_width);
            underlayer.SetStrokeLineCap(settings_.stroke_linecap);
            underlayer.SetStrokeLineJoin(settings_.stroke_linejoin);
            text.SetFillColor(settings_.color_palette[i % settings_.color_palette.size()]);

            underlayer.SetData(route.name);
            text.SetData(route.name);
            
            underlayer.SetPosition(project(route.coordinates.front()));
            text.SetPosition(project(route.coordinates.front()));
            underlayer.SetOffset({settings_.bus_label_offset.first, settings_.bus_label_offset.second});
            text.SetOffset({settings_.bus_label_offset.first, settings_.bus_label_offset.second});
            underlayer.SetFontSize(static_cast<uint32_t>(settings_.bus_label_font_size));
            text.SetFontSize(static_cast<uint32_t>(settings_.bus_label_font_size));
            underlayer.SetFontFamily("Verdana");
            text.SetFontFamily("Verdana");
            underlayer.SetFontWeight("bold");
            text.SetFontWeight("bold");

            if(!route.is_circular && route.coordinates.size() >= 2 && route.coordinates.front() != route.coordinates.back()) {
                doc.Add(underlayer);
                doc.Add(text);
                
                underlayer.SetPosition(project(route.coordinates.back()));
                text.SetPosition(project(route.coordinates.back()));
            }

            doc.Add(underlayer);
            doc.Add(text);
            ++i;
        }
    }
}

void MapRender::StopLableRender(svg::Document& doc, std::vector<domain::Stop> stops, domain::SphereProjector& project) {
    for(auto stop: stops) {
        svg::Circle circle;
        circle.SetCenter(project(stop.coords));
        circle.SetRadius(settings_.stop_radius);
        circle.SetFillColor("white");
        doc.Add(circle);
    }
}

void MapRender::StopNameRender(svg::Document& doc, std::vector<domain::Stop> stops, domain::SphereProjector& project) {
    for(auto stop: stops) {
        svg::Text text;
        svg::Text underlayer;
        underlayer.SetFillColor(settings_.underlayer_color);
        underlayer.SetStrokeColor(settings_.underlayer_color);
        underlayer.SetStrokeWidth(settings_.underlayer_width);
        underlayer.SetStrokeLineCap(settings_.stroke_linecap);
        underlayer.SetStrokeLineJoin(settings_.stroke_linejoin);
        underlayer.SetPosition(project(stop.coords));
        underlayer.SetOffset({settings_.stop_label_offset.first, settings_.stop_label_offset.second});
        underlayer.SetFontSize(settings_.stop_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetData(stop.name);
        
        text.SetFillColor("black");
        text.SetPosition(project(stop.coords));
        text.SetOffset({settings_.stop_label_offset.first, settings_.stop_label_offset.second});
        text.SetFontSize(settings_.stop_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetData(stop.name);

        doc.Add(underlayer);
        doc.Add(text);        
    }
}
}