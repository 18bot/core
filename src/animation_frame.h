
#ifndef HEXBOT_ANIMATION_FRAME_H
#define HEXBOT_ANIMATION_FRAME_H

#include <json/value.h>
#include <list>

class AnimationFrame
{
    friend class AnimationInstance;

public:
    AnimationFrame(const Json::Value& data);
    typedef std::tuple<int, uint32_t> FrameMove;
    typedef std::map<std::string, FrameMove> FrameMoves;

protected:
    void read(const Json::Value& data);

public:
    uint32_t getPosition() const { return m_position; }
    const FrameMoves& getMoves() const { return m_moves; }

private:
    FrameMoves m_moves;
    uint32_t m_position;
};

class BoundFrame
{
    friend class AnimationInstance;

public:
    BoundFrame(uint32_t position) :
        m_position(position) {}

    typedef std::tuple<int, uint32_t> FrameMove;
    typedef std::map<std::string, FrameMove> FrameMoves;

    void assign(const AnimationFrame& frame, const std::map<std::string, std::string>& bindings);

public:
    uint32_t getPosition() const { return m_position; }
    const FrameMoves& getMoves() const { return m_moves; }

private:
    FrameMoves m_moves;
    uint32_t m_position;
};

#endif //HEXBOT_ANIMATION_FRAME_H
