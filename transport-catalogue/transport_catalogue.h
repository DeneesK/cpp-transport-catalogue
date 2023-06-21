#pragma once

#include "geo.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <unordered_map>
#include <utility>
#include <set>
#include <functional>
#include <optional>

namespace catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<Stop*> route;
    bool is_circular;
};

struct StopHasher {
    size_t operator()(std::pair<Stop*, Stop*> stops) const {
        size_t hash_ = std::hash<const void*>{}(stops.first) * 17 + std::hash<const void*>{}(stops.second) * 23;
        return hash_;
    }
};

struct BusInfo {
    double length;
    double curvature;
    int stops_on_route;
    int unique_stops;
    bool is_circular;
};

struct StopInfo {
    std::set<std::string_view> buses;
};

class TransportCatalogue {
public:
    void AddStop(const std::string& name, geo::Coordinates coords);
    void AddBus(const std::string&, std::vector<std::string> route, bool is_circular);
    void AddDistance(const std::string& from_stop, std::string& to_stop, int dist);

    std::optional<BusInfo> GetBusInfo(const std::string& bus_name) const;
    std::optional<StopInfo> GetStopInfo(const std::string& stop_name) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_busname_;
    std::unordered_map<std::pair<Stop*, Stop*>, int, StopHasher> stops_to_distance_;
    
    const Stop* GetStop(const std::string& stop_name) const;
    const Bus* GetBus(const std::string& bus_name) const;
    std::set<std::string_view> GetBuses(const std::string& stop_name) const;

    double CalculateGeoRouteLength(const Bus* bus) const;
    double CalculateTrueRouteLength(const Bus* bus) const;
    int GetTrueDistance(Stop* from, Stop* to) const;
};
}