#pragma once

#include "building.h"
#include "dispatch_strategy.h"
#include "request.h"

#include <memory>
#include <mutex>
#include <unordered_map>

class ElevatorSystem {
public:
    ElevatorSystem(
        int numberOfFloors,
        int numberOfElevators
    );

    void requestElevator(
        int floor,
        Direction direction
    );

    void selectFloor(
        int elevatorId,
        int destinationFloor
    );

    void step();

    Building& building();
    const Building& building() const;

private:
    Building building_;

    std::unique_ptr<DispatchStrategy> dispatchStrategy_;

    /*
     * Tracks which elevator is responsible for each
     * currently active hall request.
     *
     * Example:
     *
     *   {5, UP}   -> elevator 2
     *   {10, DOWN} -> elevator 0
     */
    std::unordered_map<
        HallRequest,
        int,
        HallRequestHash
    > hallRequestAssignments_;

    mutable std::mutex mutex_;

    void assignHallRequest(
        const HallRequest& request
    );

    void completeHallRequest(
        int elevatorId,
        int floor
    );

    void reassignUnavailableRequests();
};