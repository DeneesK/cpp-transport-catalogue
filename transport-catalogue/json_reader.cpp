#include "json_reader.h"

namespace json_reader {
    using StopToStopDist = std::vector<std::pair<std::string, json::Node>>;

    ReaderResult LoadJSON(std::istream& input, catalogue::TransportCatalogue& db) {
        json::Document doc = json::Load(input);

        json::Dict dict = doc.GetRoot().AsMap();
        json::Array base_requests = dict.at("base_requests").AsArray();
        json::Dict render_settings = dict.at("render_settings").AsMap();
        json::Array stat_requests = dict.at("stat_requests").AsArray();

        ProceedDBStoring(base_requests, db);
        request_handler::Requests requests = ParseStatRequests(stat_requests);
        map_render::RenderSettings settings = ParseRenderSettings(render_settings);

        return ReaderResult{requests, settings};
    }

    void ProceedDBStoring(json::Array& req, catalogue::TransportCatalogue& db) {
        json::Array buses;
        json::Array stops;
        for(json::Node elem: req) {
            if(elem.AsMap().at("type").AsString() == "Bus") {
                buses.push_back(elem);
            } else {
                stops.push_back(elem);
            }
        }
        SendStops(stops, db);
        SendBuses(buses, db);
    }

    void SendStops(json::Array& stops, catalogue::TransportCatalogue& db) {
        StopToStopDist distances;
        for(json::Node stop: stops) {
            json::Dict stp = stop.AsMap();

            db.AddStop(stp.at("name").AsString(), {stp.at("latitude").AsDouble(), stp.at("longitude").AsDouble()});
            distances.push_back({stp.at("name").AsString(), stp.at("road_distances")});
        }
        for(auto dist: distances) {
            auto from_stop = dist.first;
            for(auto [to_stop, dist_to]: dist.second.AsMap()) {
                auto to = to_stop;
                db.AddDistance(from_stop, to, dist_to.AsInt());
            }
        }
    }

    void SendBuses(std::vector<json::Node>& buses, catalogue::TransportCatalogue& db) {
        for(auto bus: buses) {
            auto bus_dict = bus.AsMap();
            auto route = bus_dict.at("stops").AsArray();
            std::vector<std::string> stops;
            for(auto stop: route) {
                stops.push_back(stop.AsString());
            }
            db.AddBus(bus_dict.at("name").AsString(), stops, bus_dict.at("is_roundtrip").AsBool());
        }
    }

    request_handler::Requests ParseStatRequests(json::Array stat_requests) {
        request_handler::Requests requests;        
        for(auto req: stat_requests) {
            json::Dict request = req.AsMap();
            auto id = request.at("id").AsInt();
            auto type = request.at("type").AsString();
            if(request.count("name")) {
              auto name = request.at("name").AsString();
              requests.push_back(request_handler::Request{id, type, name});
            } else {
                requests.push_back(request_handler::Request{id, type, ""});
            }
        }
        return requests;
    }

    map_render::RenderSettings ParseRenderSettings(json::Dict render_settings) {
        map_render::RenderSettings settings;
        settings.height = render_settings.at("height").AsDouble();
        settings.width = render_settings.at("width").AsDouble();
        settings.padding = render_settings.at("padding").AsDouble();
        settings.line_width = render_settings.at("line_width").AsDouble();
        settings.stop_radius = render_settings.at("stop_radius").AsDouble();
        settings.bus_label_font_size = render_settings.at("bus_label_font_size").AsInt();
        settings.bus_label_offset.first = render_settings.at("bus_label_offset").AsArray()[0].AsDouble();
        settings.bus_label_offset.second = render_settings.at("bus_label_offset").AsArray()[1].AsDouble();
        settings.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
        settings.stop_label_offset.first = render_settings.at("stop_label_offset").AsArray()[0].AsDouble();
        settings.stop_label_offset.second = render_settings.at("stop_label_offset").AsArray()[1].AsDouble();
        settings.underlayer_width = render_settings.at("underlayer_width").AsDouble();
        
        for (auto color: render_settings.at("color_palette").AsArray()) {
            if(color.IsString()) {
                settings.color_palette.push_back(color.AsString());
            } else {
            if (color.AsArray().size() == 3) {
                auto color_arr = color.AsArray();
                settings.color_palette.push_back(svg::Rgb{static_cast<uint16_t>(color_arr[0].AsInt()),
                                                          static_cast<uint16_t>(color_arr[1].AsInt()),
                                                          static_cast<uint16_t>(color_arr[2].AsInt())});
            } else {
                auto color_arr = color.AsArray();
                settings.color_palette.push_back(svg::Rgba{static_cast<uint16_t>(color_arr[0].AsInt()),
                                                           static_cast<uint16_t>(color_arr[1].AsInt()),
                                                           static_cast<uint16_t>(color_arr[2].AsInt()),
                                                           color_arr[3].AsDouble()});
            }                
            }
        }
        
        if (render_settings.at("underlayer_color").IsString()) {
            settings.underlayer_color = render_settings.at("underlayer_color").AsString();
        } else {
            if (render_settings.at("underlayer_color").AsArray().size() == 3) {
                auto color_arr = render_settings.at("underlayer_color").AsArray();
                settings.underlayer_color = svg::Rgb{static_cast<uint16_t>(color_arr[0].AsInt()),
                                                     static_cast<uint16_t>(color_arr[1].AsInt()),
                                                     static_cast<uint16_t>(color_arr[2].AsInt())};
            } else {
                auto color_arr = render_settings.at("underlayer_color").AsArray();
                settings.underlayer_color = svg::Rgba{static_cast<uint16_t>(color_arr[0].AsInt()),
                                                      static_cast<uint16_t>(color_arr[1].AsInt()),
                                                      static_cast<uint16_t>(color_arr[2].AsInt()),
                                                      color_arr[3].AsDouble()};
            }
        }

        return settings;
    }

void RenderJSON(request_handler::Requests& requests, request_handler::RequestHandler& rh, const std::string& renndered_map, std::ostream& out) {
    json::Array answer;
    for(auto req: requests) {
        if(req.type == "Bus") {
            if(auto bus_stat = rh.GetBusInfo(req.name)) {
                answer.push_back(json::Node{json::Dict{
                    {"curvature", bus_stat.value().curvature},
                    {"request_id", req.id},
                    {"route_length", bus_stat.value().length},
                    {"stop_count", bus_stat.value().stops_on_route},
                    {"unique_stop_count", bus_stat.value().unique_stops},
                    }});
            } else {
                answer.push_back(json::Node{json::Dict{{"request_id", req.id},
                {"error_message", json::Node{std::string("not found")}}
                }});
            }

        } else if(req.type == "Stop") {
            if(auto stop_stat = rh.GetStopInfo(req.name)) {
                json::Array route;
                for (auto bus: stop_stat.value().buses) {
                    route.push_back(std::string(bus));
                }
                answer.push_back(json::Node{json::Dict{
                    {"buses", route},
                    {"request_id", req.id},
                    }});
            } else {
                answer.push_back(json::Node{json::Dict{{"request_id", req.id},
                {"error_message", json::Node{std::string("not found")}}
                }});
            }
        } else {
            answer.push_back(json::Node{json::Dict{{"request_id", req.id},
                {"map", json::Node{renndered_map}}
                }});
        }
    }
    json::Document doc(answer);
    json::Print(doc, out);
}
}
