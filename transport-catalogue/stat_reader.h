#pragma once

#include "transport_catalogue.h"

namespace request_reader {

std::istream& StatReader(std::istream& input, catalogue::TransportCatalogue& catalog, std::ostream& out);

std::string GetRouteInfo(const std::string& text, catalogue::TransportCatalogue& catalog);
std::string GetStopInfo(const std::string& stop_name, catalogue::TransportCatalogue& catalog);
}