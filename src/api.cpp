
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

void RoboUpdate(float dt)
{
    Hexbot::getInstance()->update(dt);
}
