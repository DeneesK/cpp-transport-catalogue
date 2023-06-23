#include "map_renderer.h"

#include <unordered_set>

namespace map_render {

MapRender::MapRender(RenderSettings settings): settings_(settings) {}

void MapRender::Render(request_handler::RequestHandler& rh, std::ostream& out) {
    std::vector<domain::BusRoute> routes = rh.GetAllRoutes();
    std::unordered_set<geo::Coordinates, geo::CoordinateshHasher> unique_cords;
    for(auto route: routes) {
        unique_cords.insert(route.coordinates.begin(), route.coordinates.end());
    }

    domain::SphereProjector project(unique_cords.begin(), unique_cords.end(), settings_.width, settings_.height, settings_.padding);

    svg::Document doc;
    int i = 0;
    sort(routes.begin(), routes.end(), [](auto& l, auto& r){return l.name < r.name;});
    
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
    doc.Render(out);
}
}