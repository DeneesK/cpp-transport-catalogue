#include "request_handler.h"

namespace request_handler {

RequestHandler::RequestHandler(const catalogue::TransportCatalogue& db): db_(db) {}

void RequestHandler::ProceedRequests(Requests& requests, std::ostream& out) {
    json::Array answer = PrepareAnswer(requests);
    json::Document doc(answer);
    json::Print(doc, out);
}

std::optional<catalogue::BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    if(auto bas_stat = db_.GetBusInfo(std::string(bus_name))) {
        return bas_stat;
    }
    return std::nullopt;
}

std::optional<catalogue::StopInfo> RequestHandler::GetStopStat(const std::string_view& stop_name) const {
    if(auto stop_stat = db_.GetStopInfo(std::string(stop_name))) {
        return stop_stat;
    }    
    return std::nullopt;
}

json::Array RequestHandler::PrepareAnswer(Requests& requests) {
    json::Array answer;
    for(auto req: requests) {
        if(req.type == "Bus") {
            if(auto bus_stat = GetBusStat(req.name)) {
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

        } else {
            if(auto stop_stat = GetStopStat(req.name)) {
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
        }
    }

    return answer;    
}

std::vector<domain::BusRoute> RequestHandler::GetAllRoutes() {
    std::vector<domain::BusRoute> routes;
    auto buses = db_.GetAllBuses();
    for(auto bus: *buses) {
        auto name = bus.name;
        auto is_circular = bus.is_circular;
        std::vector<geo::Coordinates> coords;

        for(auto stop: bus.route) {
            coords.push_back(stop->coordinates);
        }
        routes.push_back({name, coords, is_circular});
    }
    return routes;
}
}