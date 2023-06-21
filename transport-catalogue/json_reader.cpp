#include "json_reader.h"

namespace json_reader {
    using StopToStopDist = std::vector<std::pair<std::string, json::Node>>;

    request_handler::Requests LoadJSON(std::istream& input, catalogue::TransportCatalogue& db) {
        json::Document doc = json::Load(input);

        json::Dict dict = doc.GetRoot().AsMap();
        json::Array base_requests = dict.at("base_requests").AsArray();
        json::Array stat_requests = dict.at("stat_requests").AsArray();

        ProceedDBStoring(base_requests, db);

        request_handler::Requests requests;        

        for(auto req: stat_requests) {
            json::Dict request = req.AsMap();
            auto id = request.at("id").AsInt();
            auto type = request.at("type").AsString();
            auto name = request.at("name").AsString();
            requests.push_back(request_handler::Request{id, type, name});
        }
        return requests;
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
}