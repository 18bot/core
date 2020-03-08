//
// Created by Desertkun on 3/8/20.
//

#ifndef HEXBOT_ANIMATION_SET_H
#define HEXBOT_ANIMATION_SET_H

#include <json/value.h>
#include "animation_frame.h"
#include "animation.h"

class AnimationSet
{
public:
    AnimationSet(const Json::Value& data);
    void read(const Json::Value& data);

    const AnimationFrames& getFrames() const { return m_frames; }
    uint32_t getLength() const { return m_length; }
    const std::vector<std::string>& getBindings() const { return m_bindings; }

private:
    uint32_t m_length;
    AnimationFrames m_frames;
    std::vector<std::string> m_bindings;
};

class AnimationSetPlay
{
public:
    AnimationSetPlay(const Animation& animation, const Json::Value& data);
    void read(const Animation& animation, const Json::Value& data);

    uint32_t getPosition() const { return m_position; }
    const AnimationSet* getSet() const { return m_set; }
    const std::map<std::string, std::string>& getBindings() const { return m_bindings; }

private:
    uint32_t m_position;
    const AnimationSet* m_set;
    std::map<std::string, std::string> m_bindings;
};


#endif //HEXBOT_ANIMATION_SET_H
