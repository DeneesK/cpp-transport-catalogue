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

    void ProceedRequests(Requests& requests, std::ostream& out);
    std::vector<domain::BusRoute> GetAllRoutes();

private:
    const catalogue::TransportCatalogue& db_;

    std::optional<catalogue::BusInfo> GetBusStat(const std::string_view& bus_name) const;
    std::optional<catalogue::StopInfo> GetStopStat(const std::string_view& bus_name) const;
    
    json::Array PrepareAnswer(Requests& requests);
};
}