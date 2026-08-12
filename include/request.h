#pragma once

#include "types.h"

#include <cstddef>

struct HallRequest {
    int floor;
    Direction direction;

    bool operator==(const HallRequest& other) const {
        return floor == other.floor &&
               direction == other.direction;
    }
};

struct HallRequestHash {
    std::size_t operator()(const HallRequest& request) const {
        const std::size_t floorHash =
            std::hash<int>{}(request.floor);

        const std::size_t directionHash =
            std::hash<int>{}(
                static_cast<int>(request.direction)
            );

        return floorHash ^
               (directionHash << 1);
    }
};

struct CarRequest {
    int elevatorId;
    int destinationFloor;
};