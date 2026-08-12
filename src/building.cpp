#include "building.h"

#include <stdexcept>

Building::Building(int numberOfFloors, int numberOfElevators)
    : numberOfFloors_(numberOfFloors) {
    if (numberOfFloors <= 0) {
        throw std::invalid_argument(
            "Building must have at least one floor"
        );
    }

    if (numberOfElevators <= 0) {
        throw std::invalid_argument(
            "Building must have at least one elevator"
        );
    }

    elevators_.reserve(numberOfElevators);

    for (int id = 0; id < numberOfElevators; ++id) {
        elevators_.push_back(
            std::make_unique<Elevator>(
                id,
                1,
                numberOfFloors
            )
        );
    }
}

int Building::numberOfFloors() const {
    return numberOfFloors_;
}

int Building::numberOfElevators() const {
    return static_cast<int>(elevators_.size());
}

bool Building::isValidFloor(int floor) const {
    return floor >= 1 && floor <= numberOfFloors_;
}

Elevator& Building::elevator(int elevatorId) {
    if (elevatorId < 0 ||
        elevatorId >= numberOfElevators()) {
        throw std::out_of_range(
            "Invalid elevator ID"
        );
    }

    return *elevators_[elevatorId];
}

const Elevator& Building::elevator(int elevatorId) const {
    if (elevatorId < 0 ||
        elevatorId >= numberOfElevators()) {
        throw std::out_of_range(
            "Invalid elevator ID"
        );
    }

    return *elevators_[elevatorId];
}

std::vector<std::reference_wrapper<Elevator>>
Building::elevators() {
    std::vector<std::reference_wrapper<Elevator>> result;
    result.reserve(elevators_.size());

    for (auto& elevator : elevators_) {
        result.emplace_back(*elevator);
    }

    return result;
}

std::vector<std::reference_wrapper<const Elevator>>
Building::elevators() const {
    std::vector<std::reference_wrapper<const Elevator>> result;
    result.reserve(elevators_.size());

    for (const auto& elevator : elevators_) {
        result.emplace_back(*elevator);
    }

    return result;
}