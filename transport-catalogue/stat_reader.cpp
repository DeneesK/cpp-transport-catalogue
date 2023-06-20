#include "stat_reader.h"
#include "input_reader.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace request_reader {

using namespace std;

istream& StatReader(istream& input, catalogue::TransportCatalogue& catalog, ostream& out) {
    string data;
    string result;
    getline(input, data);
    int count = stoi(data);
    while(count > 0) {
        getline(input, data);
        if(data.find("Stop") != string::npos) {
            result = GetStopInfo(data, catalog);
            out << result << endl;            
        }
        if(data.find("Bus") != string::npos) {
            result = GetRouteInfo(data, catalog);
            out << result << endl;
        }
        --count;
    }
    return input;
}

string GetRouteInfo(const string& text, catalogue::TransportCatalogue& catalog) {
    auto [_, bus_name] = input_reader::string_processing::Split(text, ' ');
    ostringstream out;
    out << "Bus " << bus_name << ": ";
    auto bus = catalog.GetBus(bus_name);
    if(bus){
        catalogue::BusInfo b = catalog.GetBusInfo(bus_name);
        out << b.stops_on_route << " stops on route, ";
        out << b.unique_stops << " unique stops, ";
        out << setprecision(6) << b.length << " route length, ";
        out << setprecision(6) << b.curvature << " curvature";
        return out.str();
    }
    out << "not found";
    return out.str();   
}

string GetStopInfo(const string& stop_name, catalogue::TransportCatalogue& catalog) {
        ostringstream out;
        out << stop_name << ": ";
        auto [_, s_name] = input_reader::string_processing::Split(stop_name, ' ');
        auto stop = catalog.GetStop(s_name);
        if(stop) {
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
}
