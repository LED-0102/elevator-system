#include "dispatch_strategy.h"

#include <cmath>
#include <limits>

namespace {

constexpr int INVALID_SCORE = std::numeric_limits<int>::max();

}

int NearestElevatorStrategy::score(
    const Elevator& elevator,
    const HallRequest& request
) {
    if (!elevator.isAvailable()) {
        return INVALID_SCORE;
    }

    const int currentFloor = elevator.currentFloor();

    const int distance =
        std::abs(currentFloor - request.floor);

    const Direction elevatorDirection =
        elevator.direction();

    /*
     * An idle elevator is always a valid candidate.
     */
    if (elevatorDirection == Direction::NONE) {
        return distance;
    }

    /*
     * Elevator is already moving in the requested direction.
     *
     * It is a particularly good candidate if the requested
     * floor lies ahead of it.
     */
    if (elevatorDirection == request.direction) {

        if (request.direction == Direction::UP &&
            currentFloor <= request.floor) {
            return distance;
        }

        if (request.direction == Direction::DOWN &&
            currentFloor >= request.floor) {
            return distance;
        }
    }

    /*
     * Elevator would have to finish its current direction
     * before serving this request.
     *
     * Keep it as a candidate, but apply a significant penalty.
     */
    constexpr int WRONG_DIRECTION_PENALTY = 1000;

    return distance + WRONG_DIRECTION_PENALTY;
}

int NearestElevatorStrategy::selectElevator(
    const Building& building,
    const HallRequest& request
) const {
    if (!building.isValidFloor(request.floor)) {
        return -1;
    }

    if (request.direction == Direction::NONE) {
        return -1;
    }

    int bestElevatorId = -1;
    int bestScore = INVALID_SCORE;

    for (const auto& elevatorRef : building.elevators()) {
        const Elevator& elevator = elevatorRef.get();

        const int currentScore =
            score(elevator, request);

        if (currentScore < bestScore) {
            bestScore = currentScore;
            bestElevatorId = elevator.id();
        }
    }

    return bestElevatorId;
}