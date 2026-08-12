#pragma once

enum class Direction {
    UP,
    DOWN,
    NONE
};

enum class ElevatorState {
    IDLE,
    MOVING_UP,
    MOVING_DOWN,
    DOOR_OPEN,
    EMERGENCY,
    OUT_OF_SERVICE
};

enum class DoorState {
    OPEN,
    CLOSED
};