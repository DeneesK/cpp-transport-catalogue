#include "transport_catalogue.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <cassert>

namespace catalogue {

using namespace std;

void TransportCatalogue::AddStop(const string& name, geo::Coordinates coords) {
        stops_.push_back(Stop{move(name), move(coords)});
        stopname_to_stop_[stops_.back().name] = &stops_.back();
        stopname_to_busname_[stops_.back().name];
}

void TransportCatalogue::AddDistance(const string& from_stop, string& to_stop, int dist) {
    assert(stopname_to_stop_.count(from_stop));
    Stop* from = stopname_to_stop_.at(from_stop);

    assert(stopname_to_stop_.count(to_stop));
    auto to = stopname_to_stop_.at(to_stop);
    stops_to_distance_[{from, to}] = dist;
}

void TransportCatalogue::AddBus(const string& name, vector<string> route, bool is_circular) {
    vector<Stop*> r;
    for(auto& stop: route) {
        if(stopname_to_stop_.count(stop)) {
            auto s = stopname_to_stop_.at(stop);
            r.push_back(s);
        }
    }

    buses_.push_back(Bus{move(name), move(r), is_circular});
    busname_to_bus_[buses_.back().name] = &buses_.back();

    for(auto stop: buses_.back().route) {
        stopname_to_busname_[stop->name].emplace(buses_.back().name);
    }
}

const Stop* TransportCatalogue::GetStop(const string& stop_name) const {
        if(stopname_to_stop_.count(stop_name)) {
            return stopname_to_stop_.at(stop_name);
        }
    return nullptr;
}

optional<StopInfo> TransportCatalogue::GetStopInfo(const string& stop_name) const {
    auto stop = GetStop(stop_name);
    if(stop) {
        auto buses = stopname_to_busname_.at(stop_name);
        return StopInfo{buses};
    }
    return nullopt;
}

const Bus* TransportCatalogue::GetBus(const string& bus_name) const {   
    if(busname_to_bus_.count(bus_name)) {
        return busname_to_bus_.at(bus_name);
    }
    return nullptr;
}

BusInfo TransportCatalogue::GetBusInfo(const string& bus_name) {
    auto bus = GetBus(bus_name);
    if(bus){
        if(bus->is_circular) {
            int size = bus->route.size();
            double length = CalculateTrueRouteLength(bus);
            double curvature = length / CalculateGeoRouteLength(bus);
            set<string> uniq;
            for(auto s: bus->route) {
                uniq.emplace(s->name);
            }
            return BusInfo{length, curvature, size, int(uniq.size()), bus->is_circular};
        }
        int size = (bus->route.size() * 2) - 1;
        double length = CalculateTrueRouteLength(bus);
        double curvature = length / (CalculateGeoRouteLength(bus) * 2);
        set<string> uniq;
        for(auto s: bus->route) {
            uniq.emplace(s->name);
        }
        return BusInfo{length, curvature, size, int(uniq.size()), bus->is_circular};
    }
    return BusInfo{};    
}

int TransportCatalogue::GetTrueDistance(Stop* from, Stop* to) {
    if(stops_to_distance_.count({from, to})) {
        return stops_to_distance_.at({from, to});
    }
    return stops_to_distance_.at({to, from});
}

double TransportCatalogue::CalculateGeoRouteLength(const Bus* bus) {
    double result = 0;
    for(auto it = bus->route.begin(); it != bus->route.end(); ++it) {
        if(next(it) != bus->route.end()) {
            result += ComputeDistance((*it)->coordinates, (*next(it))->coordinates);
        }
    }
    return result;
}

double TransportCatalogue::CalculateTrueRouteLength(const Bus* bus) {
    int result = 0;
    if(bus->is_circular) {
        for(auto it = bus->route.begin(); it != bus->route.end(); ++it) {
            if(next(it) != bus->route.end()) {
                result += GetTrueDistance((*it), (*(next(it))));
            }
        }
        return result;       
    }
    for(auto it = bus->route.begin(); it != bus->route.end(); ++it) {
        if(next(it) != bus->route.end()) {
            result += GetTrueDistance((*it), (*next(it)));
        }
    }
    for(auto it = bus->route.rbegin(); it != bus->route.rend(); ++it) {
        if(next(it) != bus->route.rend()) {
            result += GetTrueDistance((*it), (*next(it)));
        }
    }
    return result;  
}

set<string_view> TransportCatalogue::GetBuses(const string& stop_name) const {
    if(stopname_to_busname_.count(stop_name)) {
        return stopname_to_busname_.at(stop_name);
    }
    return {};
}
}