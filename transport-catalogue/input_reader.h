#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace input_reader {

enum class Info {
    StopInfo,
    DistanceInfo,
    BusInfo
};

std::istream& InputReader(std::istream& input, catalogue::TransportCatalogue& catalog);

std::pair<double, double> ParseCoordinates(std::string line);
std::pair<std::string, Coordinates> ParseStop(std::string text);
std::pair<std::string, std::vector<std::string>> ParseBus(std::string text);
std::vector<std::pair<std::string, int>> ParseDistance(std::string text);
bool IsCircular(std::string& route);

    namespace string_processing {
        std::string Strip(std::string line);
        std::vector<std::string> SplitIntoWords(std::string text, char by_char);
        std::pair<std::string, std::string> Split(std::string line, char by);
    }

void StoreData(std::vector<std::pair<Info, std::string>>& data, catalogue::TransportCatalogue& catalog);
}