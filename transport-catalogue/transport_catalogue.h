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

namespace catalogue {

struct Stop {
    std::string name;
    Coordinates coordinates;
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

class TransportCatalogue {
public:
    void AddStop(std::string name, Coordinates coords);
    void AddBus(std::string name, std::vector<std::string> route, bool is_circular);
    void AddDistance(std::string from_stop, std::vector<std::pair<std::string, int>> to_stop_dis);

    const std::pair<Stop*, bool> GetStop(std::string stop_name) const;
    const std::pair<Bus*, bool> GetBus(std::string bus_name) const;
    std::set<std::string_view> GetBuses(std::string stop_name) const;

    double CalculateGeoRouteLength(Bus* bus);
    double CalculateTrueRouteLength(Bus* bus);
    int GetTrueDistance(Stop* from, Stop* to);

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, std::set<std::string_view>> stopname_to_busname_;
    std::unordered_map<std::pair<Stop*, Stop*>, int, StopHasher> stops_to_distance_;
};
}