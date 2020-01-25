
#ifndef HEXBOT_MAIN
#define HEXBOT_MAIN

#include "api.h"
#include "utils.h"
#include "animation.h"

typedef std::shared_ptr<class Hexbot> HexbotPtr;

class Hexbot
{
    public:
        static const std::shared_ptr<Json::CharReader> CharReader;
        static const HexbotPtr& getInstance() { return s_instance; }
    
    public:
        static int Create(
            const std::string& contentsDirectory,
            api::LogCallback logCallback,
            api::MoveServoCallback moveServoCallback);
    
        Hexbot(
            const std::string& contentsDirectory,
            api::LogCallback logCallback,
            api::MoveServoCallback moveServoCallback);
    
        void update(float dt);
        void cameraSnapshot(int width, int height, int dataLength, void* data);
    
        int randomInt(int a, int b);
        float randomFloat(float a, float b);
    
    public:
        void log(const std::string& data);
        bool moveServo(int servo, float angle, float time);
    
        const AnimationPlayer& getPlayer() const { return m_player; }
        AnimationPlayer& getPlayer() { return m_player; }
    
    private:
        std::random_device m_randomDevice;
        std::mt19937_64 m_randomGen;
    
    private:
        static HexbotPtr s_instance;
    
        std::string m_contentsDirectory;
        api::LogCallback m_logCallback;
        api::MoveServoCallback m_moveServoCallback;
    
        AnimationPtr m_forwardAnimation;
        AnimationGroupPtr m_forwardAnimationGroup;
    
        AnimationPlayer m_player;
};

#endif
