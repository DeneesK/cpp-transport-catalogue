#include "input_reader.h"

#include <algorithm>
#include <utility>

namespace input_reader {

using namespace std;

istream& InputReader(istream& input, catalogue::TransportCatalogue& catalog){
    vector<pair<Info, string>> result;
    string data;
    getline(input, data);
    int count = stoi(data);
    while(count > 0) {
        getline(input, data);
        if(data.find("Stop") != string::npos) {
            if(data.find_first_of(',') != data.find_last_of(',')) {
                size_t pos = data.find_first_of(',', data.find_first_of(',')+1);
                string coord_part = data.substr(0, pos);

                result.push_back({Info::StopInfo, coord_part});
                result.push_back({Info::DistanceInfo, data});
            } else {
              result.push_back({Info::StopInfo, data});  
            }
        }
        if(data.find("Bus") != string::npos) {
            result.push_back({Info::BusInfo, data});
        }
        --count;
    }
    sort(result.begin(), result.end(), [](const auto& l, const auto& r){return l.first < r.first;});
    StoreData(result, catalog);
    return input;
}

pair<double, double> ParseCoordinates(string line) {
    auto lat_begin = line.find_first_of("1234567890");
    auto comma_pos = line.find(',');
    string coor_lng = line.substr(comma_pos + 2);
    auto lng_begin = coor_lng.find_first_of("1234567890");
    double lat = stod(line.substr(lat_begin));
    double lng = stod(coor_lng.substr(lng_begin));
    return {lat, lng};
}

pair<string, Coordinates> ParseStop(string text) {
    auto [name, coords] = string_processing::Split(text, ':');
    auto coordinates = ParseCoordinates(coords);
    return {string(name.substr(5)), {coordinates.first, coordinates.second}};
}

pair<string, vector<string>> ParseBus(string text) {
    vector<string> route;
    auto [name, stops] = string_processing::Split(text.substr(4), ':');
        if(stops.find("-") != string::npos) {
            route = string_processing::SplitIntoWords(stops, '-');
        }
        if(stops.find(">") != string::npos) {
            route = string_processing::SplitIntoWords(stops, '>');
        }
    return {name, route};    
}

vector<pair<string, int>> ParseDistance(string text) {
    vector<pair<string, int>> dists;
    text = text.substr(text.find_first_of(',', text.find_first_of(',')+1)+2);
    while(text.find_first_of(',') != string::npos) {
        auto sep = text.find_first_of('m');
        int dist = stoi(text.substr(0, sep));
        string name = text.substr(sep+5, (text.find_first_of(',') - (sep+5)));
        dists.push_back({name, dist});
        text.erase(0, text.find_first_of(',')+2);
    }
    auto sep = text.find_first_of('m');
    int dist = stoi(text.substr(0, sep));
    string name = text.substr(sep+5);
    dists.push_back({name, dist});
    return dists;
}

bool IsCircular(string& route) {
        if(route.find(">") != string::npos) {
            return true;
        }
    return false;
}

    namespace string_processing {
        string Strip(string line) {
            auto begin = line.find_first_not_of(" \n\t\r");
            auto end = line.find_last_not_of(" \n\t\r");
            if (end != string::npos) {
                line.erase(end+1);
                return line.substr(begin);
            }
            return "";
        }

        vector<string> SplitIntoWords(string text, char by_char) {
            int i = 0;
            int len = 0;
            vector<string> words;
            for_each(text.begin(), text.end(),
                [&](const char& c) {
                    if (c == by_char) {
                        if (len) {
                            words.push_back(Strip(text.substr(i, len)));
                            i += len;
                            len = 0;
                        }
                        ++i;

                    } else {
                        ++len;
                    }
                });
            
            if (len) {
                words.push_back(Strip(text.substr(i, len)));
            } 
            return words;
        }

        pair<string, string> Split(string line, char by) {
            size_t pos = line.find(by);
            string left = line.substr(0, pos);

            if (pos < line.size() && pos + 1 < line.size()) {
                return {left, line.substr(pos + 1)};
            } else {
                return {left, string()};
            }
        }
    }
void StoreData(vector<pair<Info, string>>& data, catalogue::TransportCatalogue& catalog) {
    for(auto& [type, text]: data) {
        if(type == Info::StopInfo) {
            auto stop = ParseStop(text);
            catalog.AddStop(stop.first, stop.second);
        }
        if(type == Info::BusInfo) {
            auto bus = ParseBus(text);
            bool is_circular = IsCircular(text);
            catalog.AddBus(bus.first, bus.second, is_circular);
        }

        if(type == Info::DistanceInfo) {
            auto to_distance = ParseDistance(text);
            auto [name, _] = string_processing::Split(text, ':');
            string from_stop = name.substr(5);
            catalog.AddDistance(from_stop, to_distance);
        }        
    }
}
}