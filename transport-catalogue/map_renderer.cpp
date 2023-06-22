#include "map_renderer.h"

namespace map_render {

MapRender::MapRender(RenderSettings settings): settings_(settings) {}

void MapRender::Render(request_handler::RequestHandler& rh, std::ostream& out) {
    std::vector<domain::BusRoute> routes = rh.GetAllRoutes();
    std::vector<geo::Coordinates> coords;
    for(auto route: routes) {
        coords.insert(coords.end(), route.coordinates.begin(), route.coordinates.end());
    }
    domain::SphereProjector project(coords.begin(), coords.end(), settings_.width, settings_.height, settings_.padding);

    svg::Document doc;
    int i = 0;
    sort(routes.begin(), routes.end(), [](auto& l, auto& r){return l.name < r.name;});
    
    for(auto route: routes) {
        svg::Polyline line;
        if (!route.coordinates.empty()) {
            for(auto stop: route.coordinates) {
                line.AddPoint(project(stop));
            }
            if(!route.is_circular) {
            for(auto it = route.coordinates.rbegin()+1; it != route.coordinates.rend(); ++it) {
                line.AddPoint(project(*it));
            }
            }
            if(i > int(settings_.color_palette.size()-1)) {
                i = 0;
            }
            line.SetFillColor("none");
            line.SetStrokeColor(settings_.color_palette[i]);
            line.SetStrokeWidth(settings_.line_width);
            line.SetStrokeLineCap(settings_.stroke_linecap);
            line.SetStrokeLineJoin(settings_.stroke_linejoin);
            doc.Add(line);
            ++i;
        }
    }
    
    doc.Render(out);
}
}