#include "elevator_system.h"

#include <cassert>

void testCarRequest() {
    ElevatorSystem system(10, 2);

    system.selectFloor(0, 5);

    const auto& elevator =
        system.building().elevator(0);

    assert(
        elevator.direction() == Direction::UP
    );

    system.step();
    system.step();

    assert(
        elevator.currentFloor() == 3
    );
}

void testElevatorStopsAtDestination() {
    ElevatorSystem system(10, 1);

    system.selectFloor(0, 5);

    for (int i = 0; i < 4; ++i) {
        system.step();
    }

    const auto& elevator =
        system.building().elevator(0);

    assert(elevator.currentFloor() == 5);
}

void testHallRequest() {
    ElevatorSystem system(10, 2);

    system.requestElevator(
        5,
        Direction::UP
    );

    bool foundElevator = false;

    for (const auto& elevatorRef :
         system.building().elevators()) {

        const Elevator& elevator =
            elevatorRef.get();

        if (elevator.direction() == Direction::UP) {
            foundElevator = true;
            break;
        }
    }

    assert(foundElevator);
}

void testDuplicateHallRequest() {
    ElevatorSystem system(10, 3);

    system.requestElevator(
        5,
        Direction::UP
    );

    system.requestElevator(
        5,
        Direction::UP
    );

    int movingElevators = 0;

    for (const auto& elevatorRef :
         system.building().elevators()) {

        const Elevator& elevator =
            elevatorRef.get();

        if (elevator.direction() == Direction::UP) {
            ++movingElevators;
        }
    }

    assert(movingElevators == 1);
}

void testOppositeHallRequestsCanExist() {
    ElevatorSystem system(10, 2);

    system.requestElevator(
        5,
        Direction::UP
    );

    system.requestElevator(
        5,
        Direction::DOWN
    );

    /*
     * Both requests are logically different and should
     * therefore not be deduplicated.
     *
     * We don't assert that they necessarily use different
     * elevators because that is a dispatch-policy decision.
     */
    const auto& building = system.building();

    bool foundMovingElevator = false;

    for (const auto& elevatorRef :
         building.elevators()) {

        if (elevatorRef.get().direction() != Direction::NONE) {
            foundMovingElevator = true;
            break;
        }
    }

    assert(foundMovingElevator);
}

void testInvalidFloor() {
    ElevatorSystem system(10, 2);

    bool threw = false;

    try {
        system.requestElevator(
            11,
            Direction::UP
        );
    } catch (...) {
        threw = true;
    }

    assert(threw);
}

void testEmergencyElevatorIsUnavailable() {
    ElevatorSystem system(10, 2);

    system.building()
        .elevator(0)
        .setEmergency();

    system.requestElevator(
        5,
        Direction::UP
    );

    const auto& emergencyElevator =
        system.building().elevator(0);

    assert(
        emergencyElevator.state() ==
        ElevatorState::EMERGENCY
    );
}

void testOutOfServiceElevatorIsUnavailable() {
    ElevatorSystem system(10, 2);

    system.building()
        .elevator(0)
        .setOutOfService();

    system.requestElevator(
        5,
        Direction::UP
    );

    const auto& elevator =
        system.building().elevator(1);

    assert(
        elevator.direction() == Direction::UP
    );
}

int main() {
    testCarRequest();
    testElevatorStopsAtDestination();
    testHallRequest();
    testDuplicateHallRequest();
    testOppositeHallRequestsCanExist();
    testInvalidFloor();
    testEmergencyElevatorIsUnavailable();
    testOutOfServiceElevatorIsUnavailable();

    return 0;
}