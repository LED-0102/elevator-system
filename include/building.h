#pragma once

#include "elevator.h"

#include <memory>
#include <vector>

class Building {
public:
    Building(int numberOfFloors, int numberOfElevators);

    int numberOfFloors() const;
    int numberOfElevators() const;

    bool isValidFloor(int floor) const;

    Elevator& elevator(int elevatorId);
    const Elevator& elevator(int elevatorId) const;

    std::vector<std::reference_wrapper<Elevator>> elevators();
    std::vector<std::reference_wrapper<const Elevator>> elevators() const;

private:
    int numberOfFloors_;
    std::vector<std::unique_ptr<Elevator>> elevators_;
};