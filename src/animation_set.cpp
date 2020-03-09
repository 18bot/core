
#include "animation_set.h"

AnimationSet::AnimationSet(const Json::Value& data)
{
    read(data);
}

void AnimationSet::read(const Json::Value& data)
{
    const Json::Value& bindings = data["bindings"];
    m_bindings.reserve(bindings.size());
    for (const auto& binding : bindings)
    {
        m_bindings.push_back(binding.asString());
    }
    m_length = data["length"].asUInt();

    const Json::Value& frames = data["frames"];
    for (Json::ValueConstIterator it = frames.begin(); it != frames.end(); it++)
    {
        m_frames.emplace_back(*it);
    }
}

AnimationSetPlay::AnimationSetPlay(const Animation& animation, const Json::Value& data) :
    m_set(nullptr)
{
    read(animation, data);
}

void AnimationSetPlay::read(const Animation& animation, const Json::Value& data)
{
    m_position = data["position"].asUInt();

    auto set = animation.getSets().find(data["set"].asString());
    if (set == animation.getSets().end())
    {
        throw std::runtime_error("Set was not found");
    }

    m_set = &set->second;

    const Json::Value& bindings = data["bindings"];
    for (Json::ValueConstIterator it = bindings.begin(); it != bindings.end(); it++)
    {
        m_bindings.emplace(std::piecewise_construct,
            std::forward_as_tuple(it.name()),
            std::forward_as_tuple(it->asString()));
    }
}