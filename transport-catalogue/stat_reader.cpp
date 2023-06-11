#include "stat_reader.h"
#include "input_reader.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace request_reader {

using namespace std;

istream& StatReader(istream& input, catalogue::TransportCatalogue& catalog) {
    string data;
    string result;
    vector<string> r;
    getline(input, data);
    int count = stoi(data);
    while(count > 0) {
        getline(input, data);
        if(data.find("Stop") != string::npos) {
            result = GetStopInfo(data, catalog);
            r.push_back(result);            
        }
        if(data.find("Bus") != string::npos) {
            result = GetRouteInfo(data, catalog);
            r.push_back(result);
        }
        --count;
    }
    PrintStat(r);
    return input;
}

string GetRouteInfo(string text, catalogue::TransportCatalogue& catalog) {
    auto [_, bus_name] = input_reader::string_processing::Split(text, ' ');
    ostringstream out;
    out << "Bus " << bus_name << ": ";
    auto bus = catalog.GetBus(bus_name);
    if(bus.second){
        if(bus.first->is_circular) {
            int size = bus.first->route.size();
            double length = catalog.CalculateTrueRouteLength(bus.first);
            double curvature = length / catalog.CalculateGeoRouteLength(bus.first);
            set<string> uniq;
            for(auto s: bus.first->route) {
                uniq.emplace(s->name);
            }
            out << size << " stops on route, ";
            out << uniq.size() << " unique stops, ";
            out << setprecision(6) << length << " route length, ";
            out << setprecision(6) << curvature << " curvature";
            return out.str();
        }
        int size = (bus.first->route.size() * 2) - 1;
        double length = catalog.CalculateTrueRouteLength(bus.first);
            double curvature = length / (catalog.CalculateGeoRouteLength(bus.first) * 2);
        set<string> uniq;
        for(auto s: bus.first->route) {
            uniq.emplace(s->name);
        }
        out << size << " stops on route, ";
        out << uniq.size() << " unique stops, ";
        out << setprecision(6) << length << " route length, ";
        out << setprecision(6) << curvature << " curvature";
        return out.str();
    }
    out << "not found";
    return out.str();   
}

string GetStopInfo(string stop_name, catalogue::TransportCatalogue& catalog) {
        ostringstream out;
        out << stop_name << ": ";
        auto [_, s_name] = input_reader::string_processing::Split(stop_name, ' ');
        auto stop = catalog.GetStop(s_name);
        if(stop.second) {
            auto buses = catalog.GetBuses(s_name);
            if(buses.empty()) {
                out << "no buses";
                return out.str(); 
            }
            out << "buses";
            for(auto bus: buses) {
                out << " " << bus;
            }
            return out.str();
        }
        out << "not found";
    return out.str();
}

void PrintStat(vector<string>& r) {
    for(auto& s: r) {
        cout << s << endl;
    }
}
}
