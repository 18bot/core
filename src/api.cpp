
#include "api.h"
#include "main.h"

int RoboInit(
    const char* contentsDirectory,
    api::LogCallback logCallback,
    api::GetGyroscopeDataCallback getGyroscopeDataCallback,
    api::GetAccelerometerDataCallback getAccelerometerDataCallback,
    api::MoveServoCallback moveServoCallback,
    api::RequestCameraSnapshotCallback requestCameraSnapshotCallback
) {
    return Hexbot::Create(
        std::string(contentsDirectory),
        logCallback,
        getGyroscopeDataCallback,
        getAccelerometerDataCallback,
        moveServoCallback,
        requestCameraSnapshotCallback
    );
}

void RoboUpdate(float dt)
{
    Hexbot::getInstance()->update(dt);
}


void RoboCameraSnapshot(int width, int height, int dataLength, void* data)
{
    Hexbot::getInstance()->cameraSnapshot(width, height, dataLength, data);
}
