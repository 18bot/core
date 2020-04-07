
#include "api.h"
#include "main.h"

int RoboInit(
    const char* contentsDirectory,
    api::LogCallback logCallback,
    api::MoveServoCallback moveServoCallback
) {
    return Hexbot::Create(
        std::string(contentsDirectory),
        logCallback,
        moveServoCallback
    );
}

void RoboUpdate(uint32_t dt)
{
    Hexbot::getInstance()->update(dt);
}

void RoboMove(MovementState state)
{
    Hexbot::getInstance()->move(state, 1.f);
}
