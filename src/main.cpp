
#include "main.h"
#include "animation.h"

HexbotPtr Hexbot::s_instance = nullptr;

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
    m_player(moveServoCallback)
{
    m_forwardAnimation = Animation::Create(m_contentsDirectory + "/forward.json");
    m_backwardAnimation = Animation::Create(m_contentsDirectory + "/backward.json");
    m_leftAnimation = Animation::Create(m_contentsDirectory + "/left.json");
    m_rightAnimation = Animation::Create(m_contentsDirectory + "/right.json");
    m_stayAnimation = Animation::Create(m_contentsDirectory + "/stay.json");
    m_sitAnimation = Animation::Create(m_contentsDirectory + "/sit.json");

    m_playerBindings = PlayerBindings::Create(m_contentsDirectory + "/bindings.json");
    
    log("Hexbot Core Initialized!");
}

void Hexbot::move(MovementState state)
{
    switch (state)
    {
        case MOVE_Forward:
        {
            m_player.setTrack(0, m_forwardAnimation, 0, m_playerBindings);
            break;
        }
        case MOVE_Backward:
        {
            m_player.setTrack(0, m_backwardAnimation, 0, m_playerBindings);
            break;
        }
        case MOVE_Left:
        {
            m_player.setTrack(0, m_leftAnimation, 0, m_playerBindings);
            break;
        }
        case MOVE_Right:
        {
            m_player.setTrack(0, m_rightAnimation, 0, m_playerBindings);
            break;
        }
        case MOVE_Sit:
        {
            m_player.setTrack(0, m_sitAnimation, 0, m_playerBindings);
            break;
        }
        default:
        {
            m_player.setTrack(0, m_stayAnimation, 0, m_playerBindings);
            break;
        }
    }
}