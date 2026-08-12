#pragma once

#include "building.h"
#include "request.h"

class DispatchStrategy {
public:
    virtual ~DispatchStrategy() = default;

    virtual int selectElevator(
        const Building& building,
        const HallRequest& request
    ) const = 0;
};

class NearestElevatorStrategy final : public DispatchStrategy {
public:
    int selectElevator(
        const Building& building,
        const HallRequest& request
    ) const override;

private:
    static int score(
        const Elevator& elevator,
        const HallRequest& request
    );
};