
#include "animation.h"
#include "json/reader.h"
#include "utils.h"

#include "animation_frame.h"
#include "animation_set.h"
#include "animation_char.h"

#include <algorithm>

AnimationPtr Animation::Create(const std::string& filename)
{
    return AnimationPtr(new Animation(filename));
}

Animation::Animation(const std::string& filename)
{
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    std::string errors;
    Json::Value root;
    if (!JSONCharReader::Reader->parse(str.c_str(), str.c_str() + str.size(), &root, &errors))
    {
        std::cerr << "Failed to load animation group " << filename << ": " << errors << std::endl;
        abort();
    }

    read(root);
}

void Animation::read(const Json::Value& data)
{
    m_loop = data["loop"].asBool();
    m_length = data["length"].asFloat();

    const Json::Value& sets = data["sets"];
    for (Json::ValueConstIterator it = sets.begin(); it != sets.end(); it++)
    {
        m_sets.emplace(std::piecewise_construct,
            std::forward_as_tuple(it.name()),
            std::forward_as_tuple(*it));
    }

    const Json::Value& plays = data["play"];
    for (const auto& play: plays)
    {
        m_plays.emplace_back(*this, play);
    }
}

BoundFrame& Animation::findBoundFrame(BoundFrames& frames, uint32_t time)
{
    const auto& f = std::find_if(frames.begin(), frames.end(), [time](const BoundFrame& it)
    {
        return it.getPosition() == time;
    });

    if (f == frames.end())
    {
        for (BoundFrames::const_iterator it = frames.begin(); it != frames.end(); it++)
        {
            if (it->getPosition() > time)
            {
                const auto new_it = frames.emplace(it, time);
                return *new_it;
            }
        }

        frames.emplace_back(time);
        return frames.back();
    }

    return *f;
}

BoundFrames Animation::generateFrames() const
{
    BoundFrames frames;

    for (const AnimationSetPlay& play: m_plays)
    {
        for (const AnimationFrame& frame: play.getSet()->getFrames())
        {
            uint32_t position = play.getPosition() + frame.getPosition();
            if (position > m_length)
            {
                position -= m_length;
            }
            BoundFrame& boundFrame = findBoundFrame(frames, position);
            boundFrame.assign(frame, play.getBindings());
        }
    };

    return std::move(frames);
}

AnimationInstancePtr Animation::newInstance(api::MoveServoCallback moveCallback,
    const PlayerBindingsPtr& bindings, bool autoPlay)
{
    return AnimationInstance::Create(moveCallback, shared_from_this(), bindings, autoPlay);
}

// ------------------

AnimationInstancePtr AnimationInstance::Create(
    api::MoveServoCallback moveCallback,
    const AnimationPtr& animation,
    const PlayerBindingsPtr& bindings,
    bool autoPlay)
{
    return AnimationInstancePtr(new AnimationInstance(
        moveCallback, animation, bindings, autoPlay));
}

AnimationInstance::AnimationInstance(api::MoveServoCallback moveCallback, const AnimationPtr& animation,
        const PlayerBindingsPtr& bindings, bool autoPlay) :
    m_moveCallback(moveCallback),
    m_time(0),
    m_speed(1),
    m_bindings(bindings),
    m_animation(animation),
    m_frames(animation->generateFrames()),
    m_currentFrame(m_frames.begin()),
    m_active(autoPlay)
{
}

void AnimationInstance::reset()
{
    m_time = 0;
    m_speed = 1;
    m_currentFrame = m_frames.begin();
}

void AnimationInstance::restart(uint32_t delay, float speed)
{
    m_currentFrame = m_frames.begin();
    m_time = -delay;
    m_speed = speed;
    m_active = true;
}

void AnimationInstance::start()
{
    m_active = true;
}

void AnimationInstance::stop()
{
    m_active = false;
}

void AnimationInstance::activateFrame(const BoundFrame& frame) const
{
    const BoundFrame::FrameMoves& moves = frame.getMoves();

    for (auto it = moves.begin(); it != moves.end(); it++)
    {
        const auto& frame = it->second;
        const std::string& name = it->first;
        
        auto bit = m_bindings->getBindings().find(name);
        if (bit == m_bindings->getBindings().end())
            continue;
        int servo = bit->second.servo;
        float coef = bit->second.coef;
        float offset = bit->second.offset;

        m_moveCallback(servo, ((float)std::get<0>(frame) * coef) + offset, std::get<1>(frame) * m_speed);
    }
}

bool AnimationInstance::update(uint32_t dt)
{
    if (!m_active)
        return false;
    
    m_time += dt * m_speed;
    
    if (m_currentFrame != m_frames.end())
    {
        while (m_time >= m_currentFrame->getPosition())
        {
            activateFrame(*m_currentFrame);
            m_currentFrame++;
            
            if (m_currentFrame == m_frames.end())
                break;
        }
    }
    
    if (m_time >= m_animation->getLength())
    {
        reset();
        
        if (!m_animation->isLoop())
        {
            stop();
        }
    }
    
    return true;
}

// ------------------


PlayerBindingsPtr PlayerBindings::Create(const std::string& filename)
{
    return PlayerBindingsPtr(new PlayerBindings(filename));
}

void PlayerBindings::read(const Json::Value& data)
{
    const Json::Value& bindings = data["bindings"];
    
    for (Json::ValueConstIterator it = bindings.begin(); it != bindings.end(); it++)
    {
        const auto& bind = (*it)["bind"];
        const auto& coef = (*it)["coef"];
        const auto& offset = (*it)["offset"];

        m_bindings.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(it.name()),
            std::forward_as_tuple(bind.asInt(), coef.isNull() ? 1 : coef.asFloat(), offset.asFloat())
        );
    }
}

PlayerBindings::PlayerBindings(const std::string& filename)
{
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    
    std::string errors;
    Json::Value root;
    if (!JSONCharReader::Reader->parse(str.c_str(), str.c_str() + str.size(), &root, &errors))
    {
        std::cerr << "Failed to load animation group " << filename << ": " << errors << std::endl;
        abort();
    }
    
    read(root);
}

// ------------------

void AnimationPlayer::update(uint32_t dt)
{
    for (auto it = m_tracks.cbegin(); it != m_tracks.cend() /* not hoisted */; /* no increment */)
    {
      if (!it->second->update(dt))
      {
        m_tracks.erase(it++);    // or "it = m.erase(it)" since C++11
      }
      else
      {
        ++it;
      }
    }
}

AnimationPlayer::AnimationPlayer(api::MoveServoCallback moveCallback) :
    m_moveCallback(moveCallback)
{
}

void AnimationPlayer::setTrack(int track, const AnimationInstancePtr& instance)
{
    m_tracks[track] = instance;
}

void AnimationPlayer::setTrack(int track, const AnimationPtr& animation, float delay, float speed, const PlayerBindingsPtr& bindings)
{
    const AnimationInstancePtr& instance = animation->newInstance(m_moveCallback, bindings);
    instance->restart(delay, speed);
    setTrack(track, instance);
}
