#include "domain.h"

namespace domain {
    bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}
}