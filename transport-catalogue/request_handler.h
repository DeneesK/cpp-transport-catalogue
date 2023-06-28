#pragma once

#include "transport_catalogue.h"
#include "json.h"
#include "domain.h"

namespace request_handler {

struct Request;
using Requests = std::vector<request_handler::Request>;

struct Request {
    int id;
    std::string type;
    std::string name;
};

class RequestHandler {
public:
    RequestHandler(const catalogue::TransportCatalogue& db);

    std::vector<domain::BusRoute> GetAllBuses();
    std::vector<domain::Stop> GetAllStops();
    std::optional<catalogue::BusInfo> GetBusInfo(const std::string_view& bus_name) const;
    std::optional<catalogue::StopInfo> GetStopInfo(const std::string_view& bus_name) const;
private:
    const catalogue::TransportCatalogue& db_;
};
}