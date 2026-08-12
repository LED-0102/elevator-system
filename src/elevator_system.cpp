#include "elevator_system.h"

#include <stdexcept>
#include <utility>
#include <vector>

ElevatorSystem::ElevatorSystem(
    int numberOfFloors,
    int numberOfElevators
)
    : building_(
          numberOfFloors,
          numberOfElevators
      ),
      dispatchStrategy_(
          std::make_unique<NearestElevatorStrategy>()
      ) {
}

void ElevatorSystem::requestElevator(
    int floor,
    Direction direction
) {
    if (!building_.isValidFloor(floor)) {
        throw std::out_of_range(
            "Invalid floor"
        );
    }

    if (direction == Direction::NONE) {
        throw std::invalid_argument(
            "Hall request must have a direction"
        );
    }

    HallRequest request{
        floor,
        direction
    };

    std::lock_guard<std::mutex> lock(mutex_);

    /*
     * The request is already being served.
     */
    if (hallRequestAssignments_.contains(request)) {
        return;
    }

    assignHallRequest(request);
}

void ElevatorSystem::assignHallRequest(
    const HallRequest& request
) {
    const int elevatorId =
        dispatchStrategy_->selectElevator(
            building_,
            request
        );

    if (elevatorId == -1) {
        return;
    }

    Elevator& elevator =
        building_.elevator(elevatorId);

    elevator.addStop(request.floor);

    hallRequestAssignments_[request] =
        elevatorId;
}

void ElevatorSystem::selectFloor(
    int elevatorId,
    int destinationFloor
) {
    if (!building_.isValidFloor(destinationFloor)) {
        throw std::out_of_range(
            "Invalid destination floor"
        );
    }

    /*
     * elevator() also validates the elevator ID.
     */
    building_
        .elevator(elevatorId)
        .addStop(destinationFloor);
}

void ElevatorSystem::step() {
    /*
     * First make sure pending hall requests don't remain
     * assigned to elevators that are no longer available.
     */
    reassignUnavailableRequests();

    /*
     * Move every elevator independently.
     *
     * Elevator::moveOneFloor() is internally synchronized,
     * so we don't need to hold the system mutex while
     * physically advancing elevators.
     */
    for (auto& elevatorRef : building_.elevators()) {
        Elevator& elevator = elevatorRef.get();

        const int elevatorId = elevator.id();

        const auto reachedFloor =
            elevator.moveOneFloor();

        if (reachedFloor.has_value()) {
            completeHallRequest(
                elevatorId,
                *reachedFloor
            );
        }
    }
}

void ElevatorSystem::completeHallRequest(
    int elevatorId,
    int floor
) {
    std::lock_guard<std::mutex> lock(mutex_);

    /*
     * Only complete requests that were actually assigned
     * to this particular elevator.
     */
    for (auto it = hallRequestAssignments_.begin();
         it != hallRequestAssignments_.end();) {

        const HallRequest& request = it->first;
        const int assignedElevatorId = it->second;

        if (assignedElevatorId == elevatorId &&
            request.floor == floor) {

            it = hallRequestAssignments_.erase(it);
        } else {
            ++it;
        }
    }
}

void ElevatorSystem::reassignUnavailableRequests() {
    std::lock_guard<std::mutex> lock(mutex_);

    /*
     * Collect requests first because modifying an unordered_map
     * while iterating over it would invalidate our iteration.
     */
    std::vector<HallRequest> requestsToReassign;

    for (const auto& [request, elevatorId] :
         hallRequestAssignments_) {

        const Elevator& elevator =
            building_.elevator(elevatorId);

        if (!elevator.isAvailable()) {
            requestsToReassign.push_back(request);
        }
    }

    /*
     * Remove the old assignments.
     */
    for (const HallRequest& request :
         requestsToReassign) {

        hallRequestAssignments_.erase(request);
    }

    /*
     * Try to assign them again.
     */
    for (const HallRequest& request :
         requestsToReassign) {

        assignHallRequest(request);
    }
}

Building& ElevatorSystem::building() {
    return building_;
}

const Building& ElevatorSystem::building() const {
    return building_;
}