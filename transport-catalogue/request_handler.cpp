#include "request_handler.h"

#include <algorithm>
#include <optional>

namespace request_handler {

RequestHandler::RequestHandler(const catalogue::TransportCatalogue& db): db_(db) {}

std::optional<catalogue::BusInfo> RequestHandler::GetBusInfo(const std::string_view& bus_name) const {
    if(auto bas_stat = db_.GetBusInfo(std::string(bus_name))) {
        return bas_stat;
    }
    return std::nullopt;
}

std::optional<catalogue::StopInfo> RequestHandler::GetStopInfo(const std::string_view& stop_name) const {
    if(auto stop_stat = db_.GetStopInfo(std::string(stop_name))) {
        return stop_stat;
    }    
    return std::nullopt;
}

std::vector<domain::BusRoute> RequestHandler::GetAllBuses() {
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

std::vector<domain::Stop> RequestHandler::GetAllStops() {
    std::vector<domain::Stop> stps;   
    auto stops = db_.GetAllStops();
    for(auto stop: *stops) {
        if(db_.GetStopInfo(stop.name) != std::nullopt && !db_.GetStopInfo(stop.name).value().buses.empty()) {
                stps.push_back({stop.name, stop.coordinates});   
        }
    }
    return stps;
}
}