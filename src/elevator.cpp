#include "elevator.h"

#include <stdexcept>

Elevator::Elevator(
    int id,
    int minFloor,
    int maxFloor
)
    : id_(id),
      minFloor_(minFloor),
      maxFloor_(maxFloor),
      currentFloor_(minFloor),
      state_(ElevatorState::IDLE),
      direction_(Direction::NONE) {

    if (id < 0) {
        throw std::invalid_argument(
            "Elevator ID cannot be negative"
        );
    }

    if (minFloor > maxFloor) {
        throw std::invalid_argument(
            "Minimum floor cannot be greater than maximum floor"
        );
    }
}

int Elevator::id() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return id_;
}

int Elevator::currentFloor() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentFloor_;
}

ElevatorState Elevator::state() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

Direction Elevator::direction() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return direction_;
}

bool Elevator::isAvailable() const {
    std::lock_guard<std::mutex> lock(mutex_);

    return state_ != ElevatorState::EMERGENCY &&
           state_ != ElevatorState::OUT_OF_SERVICE;
}

void Elevator::addStop(int floor) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (floor < minFloor_ || floor > maxFloor_) {
        throw std::out_of_range(
            "Requested floor is outside elevator range"
        );
    }

    if (state_ == ElevatorState::EMERGENCY ||
        state_ == ElevatorState::OUT_OF_SERVICE) {
        return;
    }

    if (floor == currentFloor_) {
        state_ = ElevatorState::DOOR_OPEN;
        return;
    }

    /*
     * If the elevator is already moving in a direction,
     * prefer continuing in that direction whenever possible.
     *
     * Otherwise place the request into the opposite queue.
     */
    if (direction_ == Direction::UP) {
        if (floor > currentFloor_) {
            upStops_.insert(floor);
        } else {
            downStops_.insert(floor);
        }
    } else if (direction_ == Direction::DOWN) {
        if (floor < currentFloor_) {
            downStops_.insert(floor);
        } else {
            upStops_.insert(floor);
        }
    } else {
        /*
         * Elevator is idle. Choose based on the requested
         * floor's position.
         */
        if (floor > currentFloor_) {
            upStops_.insert(floor);
        } else {
            downStops_.insert(floor);
        }
    }

    updateDirection();
}

void Elevator::updateDirection() {
    /*
     * Continue in the current direction as long as there
     * are stops in that direction.
     */
    if (direction_ == Direction::UP) {
        if (!upStops_.empty()) {
            state_ = ElevatorState::MOVING_UP;
            return;
        }

        /*
         * No more upward stops. Reverse only now.
         */
        if (!downStops_.empty()) {
            direction_ = Direction::DOWN;
            state_ = ElevatorState::MOVING_DOWN;
            return;
        }

        direction_ = Direction::NONE;
        state_ = ElevatorState::IDLE;
        return;
    }

    if (direction_ == Direction::DOWN) {
        if (!downStops_.empty()) {
            state_ = ElevatorState::MOVING_DOWN;
            return;
        }

        /*
         * No more downward stops. Reverse only now.
         */
        if (!upStops_.empty()) {
            direction_ = Direction::UP;
            state_ = ElevatorState::MOVING_UP;
            return;
        }

        direction_ = Direction::NONE;
        state_ = ElevatorState::IDLE;
        return;
    }

    /*
     * Elevator is idle.
     */
    if (!upStops_.empty()) {
        direction_ = Direction::UP;
        state_ = ElevatorState::MOVING_UP;
    } else if (!downStops_.empty()) {
        direction_ = Direction::DOWN;
        state_ = ElevatorState::MOVING_DOWN;
    } else {
        direction_ = Direction::NONE;
        state_ = ElevatorState::IDLE;
    }
}

bool Elevator::hasStopAtCurrentFloor() const {
    if (upStops_.contains(currentFloor_)) {
        return true;
    }

    if (downStops_.contains(currentFloor_)) {
        return true;
    }

    return false;
}

void Elevator::handleArrival() {
    if (direction_ == Direction::UP) {
        auto it = upStops_.find(currentFloor_);

        if (it != upStops_.end()) {
            upStops_.erase(it);
            state_ = ElevatorState::DOOR_OPEN;
        }
    } else if (direction_ == Direction::DOWN) {
        auto it = downStops_.find(currentFloor_);

        if (it != downStops_.end()) {
            downStops_.erase(it);
            state_ = ElevatorState::DOOR_OPEN;
        }
    }

    updateDirection();
}

std::optional<int> Elevator::moveOneFloor() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ == ElevatorState::EMERGENCY ||
        state_ == ElevatorState::OUT_OF_SERVICE ||
        state_ == ElevatorState::DOOR_OPEN) {
        return std::nullopt;
    }

    if (direction_ == Direction::NONE) {
        updateDirection();

        if (direction_ == Direction::NONE) {
            return std::nullopt;
        }
    }

    if (direction_ == Direction::UP) {
        if (currentFloor_ >= maxFloor_) {
            updateDirection();
            return std::nullopt;
        }

        ++currentFloor_;
        state_ = ElevatorState::MOVING_UP;
    } else {
        if (currentFloor_ <= minFloor_) {
            updateDirection();
            return std::nullopt;
        }

        --currentFloor_;
        state_ = ElevatorState::MOVING_DOWN;
    }

    const bool reachedStop = hasStopAtCurrentFloor();

    if (reachedStop) {
        handleArrival();
        return currentFloor_;
    }

    return std::nullopt;
}

void Elevator::openDoor() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ == ElevatorState::EMERGENCY ||
        state_ == ElevatorState::OUT_OF_SERVICE) {
        return;
    }

    state_ = ElevatorState::DOOR_OPEN;
}

void Elevator::closeDoor() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ != ElevatorState::DOOR_OPEN) {
        return;
    }

    updateDirection();
}

void Elevator::setEmergency() {
    std::lock_guard<std::mutex> lock(mutex_);

    state_ = ElevatorState::EMERGENCY;
    direction_ = Direction::NONE;

    upStops_.clear();
    downStops_.clear();
}

void Elevator::setOutOfService() {
    std::lock_guard<std::mutex> lock(mutex_);

    state_ = ElevatorState::OUT_OF_SERVICE;
    direction_ = Direction::NONE;

    upStops_.clear();
    downStops_.clear();
}

void Elevator::setAvailable() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (state_ != ElevatorState::EMERGENCY &&
        state_ != ElevatorState::OUT_OF_SERVICE) {
        return;
    }

    state_ = ElevatorState::IDLE;
    direction_ = Direction::NONE;
}