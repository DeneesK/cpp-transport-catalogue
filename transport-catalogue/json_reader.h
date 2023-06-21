#pragma once

#include "json_reader.h"
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "geo.h"

#include <iostream>
#include <unordered_map>

namespace json_reader {
    request_handler::Requests LoadJSON(std::istream& input, catalogue::TransportCatalogue& db);
    
    void ProceedDBRequests(json::Node& req, catalogue::TransportCatalogue& db);
    void SendStops(std::vector<json::Node*>& stops, catalogue::TransportCatalogue& db);
    void SendBuses(std::vector<json::Node*>& buses, catalogue::TransportCatalogue& db);
}