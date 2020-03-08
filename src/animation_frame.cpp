
#include "animation_frame.h"

AnimationFrame::AnimationFrame(const Json::Value& data)
{
    read(data);
}

void AnimationFrame::read(const Json::Value& data)
{
    m_position = data["position"].asUInt();

    const Json::Value& moves = data["moves"];

    for (Json::ValueConstIterator it = moves.begin(); it != moves.end(); it++)
    {
        std::string name = it.name();

        const Json::Value& at = *it;

        int angle = at[0].asInt();
        uint32_t time = at[1].asUInt();

        m_moves.emplace(std::piecewise_construct,
            std::forward_as_tuple(name),
            std::forward_as_tuple(angle, time));
    }
}


void BoundFrame::assign(
    const AnimationFrame& frame,
    const std::map<std::string, std::string>& bindings)
{
    for (const auto& binding: frame.getMoves())
    {
        const auto& boundMove = bindings.find(binding.first);
        if (boundMove == bindings.end())
        {
            throw std::runtime_error("No bindings");
        }

        m_moves.emplace(std::piecewise_construct,
            std::forward_as_tuple(boundMove->second),
            std::forward_as_tuple(binding.second));
    }
}