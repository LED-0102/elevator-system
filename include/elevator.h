#pragma once

#include "types.h"

#include <mutex>
#include <optional>
#include <set>

class Elevator {
public:
    Elevator(int id, int minFloor, int maxFloor);

    int id() const;
    int currentFloor() const;
    ElevatorState state() const;
    Direction direction() const;

    bool isAvailable() const;

    void addStop(int floor);

    /*
     * Moves the elevator by one floor.
     *
     * Returns the floor if the elevator reached a scheduled
     * stop during this movement.
     */
    std::optional<int> moveOneFloor();

    void openDoor();
    void closeDoor();

    void setEmergency();
    void setOutOfService();
    void setAvailable();

private:
    int id_;
    int minFloor_;
    int maxFloor_;

    int currentFloor_;

    ElevatorState state_;
    Direction direction_;

    std::set<int> upStops_;
    std::set<int> downStops_;

    mutable std::mutex mutex_;

    void updateDirection();
    bool hasStopAtCurrentFloor() const;
    void handleArrival();
};