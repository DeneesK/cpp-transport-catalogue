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
    
    void ProceedDBStoring(json::Array& req, catalogue::TransportCatalogue& db);
    void SendStops(json::Array& stops, catalogue::TransportCatalogue& db);
    void SendBuses(json::Array& buses, catalogue::TransportCatalogue& db);
}