#include "elevator_system.h"

#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string directionToString(Direction direction) {
    switch (direction) {
        case Direction::UP:
            return "UP";
        case Direction::DOWN:
            return "DOWN";
        case Direction::NONE:
            return "NONE";
    }

    return "UNKNOWN";
}

std::string stateToString(ElevatorState state) {
    switch (state) {
        case ElevatorState::IDLE:
            return "IDLE";
        case ElevatorState::MOVING_UP:
            return "MOVING_UP";
        case ElevatorState::MOVING_DOWN:
            return "MOVING_DOWN";
        case ElevatorState::DOOR_OPEN:
            return "DOOR_OPEN";
        case ElevatorState::EMERGENCY:
            return "EMERGENCY";
        case ElevatorState::OUT_OF_SERVICE:
            return "OUT_OF_SERVICE";
    }

    return "UNKNOWN";
}

void printStatus(const ElevatorSystem& system) {
    const Building& building = system.building();

    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "             ELEVATOR STATUS\n";
    std::cout << "========================================\n";

    for (const auto& elevatorRef : building.elevators()) {
        const Elevator& elevator = elevatorRef.get();

        std::cout
            << "Elevator " << elevator.id()
            << " | Floor: " << elevator.currentFloor()
            << " | Direction: "
            << directionToString(elevator.direction())
            << " | State: "
            << stateToString(elevator.state())
            << '\n';
    }

    std::cout << "========================================\n\n";
}

void printHelp() {
    std::cout << R"(
Commands:

  hall <floor> <up|down>
      Someone on a floor requests an elevator.

      Example:
        hall 5 up
        hall 12 down

  car <elevator_id> <floor>
      Passenger inside an elevator selects a destination.

      Example:
        car 0 15

  step [n]
      Advance the simulation by n floors/ticks.
      Defaults to 1.

      Example:
        step
        step 5

  status
      Show the current state of every elevator.

  emergency <elevator_id>
      Put an elevator into emergency mode.

      Example:
        emergency 2

  out <elevator_id>
      Put an elevator out of service.

      Example:
        out 1

  available <elevator_id>
      Return an emergency/out-of-service elevator to service.

      Example:
        available 1

  help
      Show this help message.

  quit
      Exit the simulation.

)";
}

bool parseDirection(
    const std::string& value,
    Direction& direction
) {
    if (value == "up" || value == "UP") {
        direction = Direction::UP;
        return true;
    }

    if (value == "down" || value == "DOWN") {
        direction = Direction::DOWN;
        return true;
    }

    return false;
}

} // namespace

int main() {
    constexpr int NUMBER_OF_FLOORS = 20;
    constexpr int NUMBER_OF_ELEVATORS = 4;

    ElevatorSystem system(
        NUMBER_OF_FLOORS,
        NUMBER_OF_ELEVATORS
    );

    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "         ELEVATOR SYSTEM SIMULATOR\n";
    std::cout << "========================================\n";
    std::cout
        << "Building: "
        << NUMBER_OF_FLOORS
        << " floors, "
        << NUMBER_OF_ELEVATORS
        << " elevators\n\n";

    printHelp();
    printStatus(system);

    std::string line;

    while (true) {
        std::cout << "elevator> ";

        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line.empty()) {
            continue;
        }

        std::istringstream input(line);

        std::string command;
        input >> command;

        if (command == "quit" ||
            command == "exit") {

            std::cout << "Shutting down simulator.\n";
            break;
        }

        if (command == "help") {
            printHelp();
            continue;
        }

        if (command == "status") {
            printStatus(system);
            continue;
        }

        if (command == "hall") {
            int floor;
            std::string directionString;

            if (!(input >> floor >> directionString)) {
                std::cout
                    << "Usage: hall <floor> <up|down>\n";
                continue;
            }

            Direction direction;

            if (!parseDirection(
                    directionString,
                    direction
                )) {

                std::cout
                    << "Direction must be 'up' or 'down'.\n";

                continue;
            }

            try {
                system.requestElevator(
                    floor,
                    direction
                );

                std::cout
                    << "Hall request registered: floor "
                    << floor
                    << " "
                    << directionToString(direction)
                    << '\n';

            } catch (const std::exception& ex) {
                std::cout
                    << "Error: "
                    << ex.what()
                    << '\n';
            }

            printStatus(system);
            continue;
        }

        if (command == "car") {
            int elevatorId;
            int floor;

            if (!(input >> elevatorId >> floor)) {
                std::cout
                    << "Usage: car <elevator_id> <floor>\n";
                continue;
            }

            try {
                system.selectFloor(
                    elevatorId,
                    floor
                );

                std::cout
                    << "Elevator "
                    << elevatorId
                    << " requested floor "
                    << floor
                    << ".\n";

            } catch (const std::exception& ex) {
                std::cout
                    << "Error: "
                    << ex.what()
                    << '\n';
            }

            printStatus(system);
            continue;
        }

        if (command == "step") {
            int count = 1;

            if (input >> count) {
                if (count <= 0) {
                    std::cout
                        << "Step count must be positive.\n";
                    continue;
                }
            }

            std::cout
                << "\nAdvancing simulation by "
                << count
                << " tick(s)...\n";

            for (int i = 0; i < count; ++i) {
                system.step();
            }

            printStatus(system);
            continue;
        }

        if (command == "emergency") {
            int elevatorId;

            if (!(input >> elevatorId)) {
                std::cout
                    << "Usage: emergency <elevator_id>\n";
                continue;
            }

            try {
                system.building()
                    .elevator(elevatorId)
                    .setEmergency();

                std::cout
                    << "Elevator "
                    << elevatorId
                    << " is now in EMERGENCY mode.\n";

            } catch (const std::exception& ex) {
                std::cout
                    << "Error: "
                    << ex.what()
                    << '\n';
            }

            printStatus(system);
            continue;
        }

        if (command == "out") {
            int elevatorId;

            if (!(input >> elevatorId)) {
                std::cout
                    << "Usage: out <elevator_id>\n";
                continue;
            }

            try {
                system.building()
                    .elevator(elevatorId)
                    .setOutOfService();

                std::cout
                    << "Elevator "
                    << elevatorId
                    << " is now OUT_OF_SERVICE.\n";

            } catch (const std::exception& ex) {
                std::cout
                    << "Error: "
                    << ex.what()
                    << '\n';
            }

            printStatus(system);
            continue;
        }

        if (command == "available") {
            int elevatorId;

            if (!(input >> elevatorId)) {
                std::cout
                    << "Usage: available <elevator_id>\n";
                continue;
            }

            try {
                system.building()
                    .elevator(elevatorId)
                    .setAvailable();

                std::cout
                    << "Elevator "
                    << elevatorId
                    << " returned to service.\n";

            } catch (const std::exception& ex) {
                std::cout
                    << "Error: "
                    << ex.what()
                    << '\n';
            }

            printStatus(system);
            continue;
        }

        std::cout
            << "Unknown command: "
            << command
            << "\nType 'help' for available commands.\n";
    }

    return 0;
}