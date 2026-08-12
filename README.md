# Elevator System

A multi-elevator simulator built with **C++20 + CMake**.

You can interact with the building from a simple CLI, send requests, move elevators through the simulation, and watch the system handle multiple elevators.

## Run it

### Requirements

* C++20
* CMake 3.20+

### Build

```bash
cmake -S . -B build
cmake --build build
```

### Run

```bash
./build/elevator_app
```

You'll start with something like:

```text
========================================
         ELEVATOR SYSTEM SIMULATOR
========================================
Building: 20 floors, 4 elevators

========================================
             ELEVATOR STATUS
========================================
Elevator 0 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 1 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 2 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 3 | Floor: 1 | Direction: NONE | State: IDLE
========================================

elevator>
```

## Try it

Call an elevator:

```text
elevator> hall 5 up

Hall request registered: floor 5 UP

Elevator 0 | Floor: 1 | Direction: UP | State: MOVING_UP
Elevator 1 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 2 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 3 | Floor: 1 | Direction: NONE | State: IDLE
```

Move the simulation:

```text
elevator> step 4

Advancing simulation by 4 tick(s)...

Elevator 0 | Floor: 5 | Direction: NONE | State: DOOR_OPEN
Elevator 1 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 2 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 3 | Floor: 1 | Direction: NONE | State: IDLE
```

Get inside and choose a destination:

```text
elevator> car 0 15

Elevator 0 requested floor 15.

elevator> step 10

Advancing simulation by 10 tick(s)...

Elevator 0 | Floor: 15 | Direction: NONE | State: DOOR_OPEN
Elevator 1 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 2 | Floor: 1 | Direction: NONE | State: IDLE
Elevator 3 | Floor: 1 | Direction: NONE | State: IDLE
```

You can also throw several requests at it:

```text
elevator> hall 8 up
elevator> hall 17 down
elevator> hall 3 up

elevator> step 5

elevator> status

========================================
             ELEVATOR STATUS
========================================
Elevator 0 | Floor: 8 | Direction: UP | State: DOOR_OPEN
Elevator 1 | Floor: 6 | Direction: UP | State: MOVING_UP
Elevator 2 | Floor: 17 | Direction: DOWN | State: DOOR_OPEN
Elevator 3 | Floor: 3 | Direction: UP | State: DOOR_OPEN
========================================
```

## Commands

```text
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
```

## What's inside

The project is intentionally kept small:

```text
elevator-system/
├── CMakeLists.txt
├── include/
│   ├── building.h
│   ├── dispatch_strategy.h
│   ├── elevator.h
│   ├── elevator_system.h
│   ├── request.h
│   └── types.h
├── src/
│   ├── building.cpp
│   ├── dispatch_strategy.cpp
│   ├── elevator.cpp
│   ├── elevator_system.cpp
│   └── main.cpp
└── tests/
    └── elevator_system_test.cpp
```

The basic flow is:

```text
                    +----------------+
                    | ElevatorSystem |
                    +-------+--------+
                            |
                +-----------+-----------+
                |                       |
         +------v------+        +-------v--------+
         |   Building  |        | Dispatch       |
         |             |        | Strategy       |
         +------+------+        +-------+--------+
                |                       |
          +-----+-----+                 |
          |     |     |                 |
         E0    E1    E2       NearestElevator
```

A hall request goes through the dispatcher, gets assigned to an elevator, and that elevator manages its own movement and stops.

## Tests

Build and run the tests:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

That's pretty much it.

Have fun breaking the elevators LoL.
