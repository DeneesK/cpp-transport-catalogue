#include "transport_catalogue.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <cassert>

namespace catalogue {

using namespace std;

void TransportCatalogue::AddStop(string name, Coordinates coords) {
        stops_.push_back(Stop{move(name), move(coords)});
        stopname_to_stop_[stops_.back().name] = &stops_.back();
        stopname_to_busname_[stops_.back().name];
}

void TransportCatalogue::AddDistance(string from_stop, vector<pair<string, int>> to_stop_dis) {
    assert(stopname_to_stop_.count(from_stop));
    Stop* from_s = stopname_to_stop_.at(from_stop);

    for(auto& [to_stop_name, dist]: to_stop_dis) {
        assert(stopname_to_stop_.count(to_stop_name));
        auto to_stop = stopname_to_stop_.at(to_stop_name);
        stops_to_distance_[{from_s, to_stop}] = dist;
    }
}

void TransportCatalogue::AddBus(string name, vector<string> route, bool is_circular) {
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

const pair<Stop*, bool> TransportCatalogue::GetStop(string stop_name) const {
        if(stopname_to_stop_.count(stop_name)) {
            return {stopname_to_stop_.at(stop_name), true};
        }
    return {nullptr, false};
}

const pair<Bus*, bool> TransportCatalogue::GetBus(string bus_name) const {   
    if(busname_to_bus_.count(bus_name)) {
        return {busname_to_bus_.at(bus_name), true};
    }
    return {nullptr, false};
}

int TransportCatalogue::GetTrueDistance(Stop* from, Stop* to) {
    if(stops_to_distance_.count({from, to})) {
        return stops_to_distance_.at({from, to});
    }
    return stops_to_distance_.at({to, from});
}

double TransportCatalogue::CalculateGeoRouteLength(Bus* bus) {
    double result = 0;
    for(auto it = bus->route.begin(); it != bus->route.end(); ++it) {
        if(next(it) != bus->route.end()) {
            result += ComputeDistance((*it)->coordinates, (*next(it))->coordinates);
        }
    }
    return result;
}

double TransportCatalogue::CalculateTrueRouteLength(Bus* bus) {
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

set<string_view> TransportCatalogue::GetBuses(string stop_name) const {
    if(stopname_to_busname_.count(stop_name)) {
        return stopname_to_busname_.at(stop_name);
    }
    return {};
}
}