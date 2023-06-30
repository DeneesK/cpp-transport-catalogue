#pragma once

#include "json.h"
#include "json_builder.h"
#include "map_renderer.h"

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

    void RenderJSON(request_handler::Requests& requests, request_handler::RequestHandler& rh,  const std::string& rendered_map, std::ostream& out);
}