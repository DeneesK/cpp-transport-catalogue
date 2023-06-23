#pragma once

namespace geo {

struct Coordinates {
    double lat = 0; // Широта
    double lng = 0; // Долгота
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

struct CoordinateshHasher {
    size_t operator()(Coordinates key) const {
        return static_cast<size_t>(key.lat * 17 + key.lng * 19);
    }
};

double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo