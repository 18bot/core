
#include <stdio.h>

#include "main.h"
#include "animation.h"

HexbotPtr Hexbot::s_instance = nullptr;

const std::shared_ptr<Json::CharReader> Hexbot::CharReader = std::shared_ptr<Json::CharReader>(Json::CharReaderBuilder().newCharReader());

int Hexbot::Create(
    const std::string& contentsDirectory,
    api::LogCallback logCallback,
    api::MoveServoCallback moveServoCallback)
{
    s_instance = std::make_shared<Hexbot>(contentsDirectory, logCallback, moveServoCallback);
    
    return 1;
    
}

void Hexbot::log(const std::string& data)
{
    m_logCallback(data.c_str());
}

bool Hexbot::moveServo(int servo, float angle, uint32_t time)
{
    return m_moveServoCallback(servo, angle, time);
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

void Hexbot::update(uint32_t dt)
{
    m_player.update(dt);
}

Hexbot::Hexbot(
        const std::string& contentsDirectory,
        api::LogCallback logCallback, 
		api::MoveServoCallback moveServoCallback) :
    m_randomGen(m_randomDevice()),

    m_contentsDirectory(contentsDirectory),
    m_logCallback(logCallback),
    m_moveServoCallback(moveServoCallback)
{
    m_forwardAnimation = Animation::Create(m_contentsDirectory + "/forward.json");
    
    m_forwardAnimationGroup = AnimationGroup::Create(m_contentsDirectory + "/forward-group.json");
    m_forwardAnimationGroup->play(m_forwardAnimation, m_player);
    
    log("Hexbot Core Initialized!");
}
