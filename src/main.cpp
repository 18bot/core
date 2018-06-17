
#include <stdio.h>

#include "main.h"
#include "animation.h"

HexbotPtr Hexbot::s_instance = nullptr;


int Hexbot::Create(
    const std::string& contentsDirectory,
    api::LogCallback logCallback,
    api::GetGyroscopeDataCallback getGyroscopeDataCallback,
    api::GetAccelerometerDataCallback getAccelerometerDataCallback,
    api::MoveServoCallback moveServoCallback,
    api::RequestCameraSnapshotCallback requestCameraSnapshotCallback)
{
    s_instance = std::make_shared<Hexbot>(contentsDirectory, logCallback, getGyroscopeDataCallback,
        getAccelerometerDataCallback, moveServoCallback, requestCameraSnapshotCallback);
    
    return 1;
    
}

void Hexbot::log(const std::string& data)
{
    m_logCallback(data.c_str());
}

bool Hexbot::getGyroscopeData(float& x, float& y, float& z)
{
    return m_getGyroscopeDataCallback(x, y, z);
}

bool Hexbot::getAccelerometerData(float& x, float& y, float& z)
{
    return m_getAccelerometerDataCallback(x, y, z);
}

bool Hexbot::moveServo(int servo, float angle, float time)
{
    return m_moveServoCallback(servo, angle, time);
}

bool Hexbot::requestCameraSnapshot(SnapshotCallback callback)
{
    m_currentSnapshotCallback = callback;
    bool result = m_requestCameraSnapshotCallback();
    m_currentSnapshotCallback = nullptr;
    return result;
}

int Hexbot::randomInt(int a, int b)
{
    std::uniform_int_distribution<int> randomInt(a, b);
    return randomInt(m_randomGen);
}

float Hexbot::randomFloat(float a, float b)
{
    std::uniform_real_distribution<float> randomFloat(a, b);
    return randomFloat(m_randomGen);
}

void Hexbot::cameraSnapshot(int width, int height, int dataLength, void* data)
{
    if (m_currentSnapshotCallback)
        m_currentSnapshotCallback(width, height, dataLength, data);
}

void Hexbot::update(float dt)
{
    m_player.update(dt);
}

Hexbot::Hexbot(
        const std::string& contentsDirectory,
        api::LogCallback logCallback, api::GetGyroscopeDataCallback getGyroscopeDataCallback,
        api::GetAccelerometerDataCallback getAccelerometerDataCallback, api::MoveServoCallback moveServoCallback,
        api::RequestCameraSnapshotCallback requestCameraSnapshotCallback) :
    m_randomGen(m_randomDevice()),

    m_contentsDirectory(contentsDirectory),
    m_logCallback(logCallback),
    m_getGyroscopeDataCallback(getGyroscopeDataCallback),
    m_getAccelerometerDataCallback(getAccelerometerDataCallback),
    m_moveServoCallback(moveServoCallback),
    m_requestCameraSnapshotCallback(requestCameraSnapshotCallback)
{
    m_forwardAnimation = Animation::Create(m_contentsDirectory + "/forward.json");
    
    m_forwardAnimationGroup = AnimationGroup::Create(m_contentsDirectory + "/forward-group.json");
    m_forwardAnimationGroup->play(m_forwardAnimation, m_player);
    
    log("Hexbot Core Initialized!");
}
