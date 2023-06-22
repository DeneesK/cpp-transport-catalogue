#pragma once

#include "json_reader.h"
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "geo.h"

#include <iostream>
#include <unordered_map>

namespace json_reader {
    
    struct ReaderResult {
        request_handler::Requests requests;
        map_render::RenderSettings settings;

    };
    request_handler::Requests ParseStatRequests(json::Array stat_requests);
    map_render::RenderSettings ParseRenderSettings(json::Dict render_settings);

    ReaderResult LoadJSON(std::istream& input, catalogue::TransportCatalogue& db);

    void ProceedDBStoring(json::Array& req, catalogue::TransportCatalogue& db);
    void SendStops(json::Array& stops, catalogue::TransportCatalogue& db);
    void SendBuses(json::Array& buses, catalogue::TransportCatalogue& db);
}