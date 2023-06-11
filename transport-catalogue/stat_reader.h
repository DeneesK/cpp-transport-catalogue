#pragma once

#include "transport_catalogue.h"

namespace output_reader {

std::istream& StatReader(std::istream& input, catalogue::TransportCatalogue& catalog);
void PrintStat(std::vector<std::string>& r);

std::string GetRouteInfo(std::string text, catalogue::TransportCatalogue& catalog);
std::string GetStopInfo(std::string stop_name, catalogue::TransportCatalogue& catalog);
}