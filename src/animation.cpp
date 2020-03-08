
#include "animation.h"
#include "json/reader.h"
#include "utils.h"
#include "main.h"


Animation::AnimationSet::AnimationSet(const Json::Value& data)
{
    read(data);
}

void Animation::AnimationSet::read(const Json::Value& data)
{
    const Json::Value& bindings = data["bindings"];
    m_bindings.reserve(bindings.size());
    for (const auto& binding : bindings)
    {
        m_bindings.push_back(binding.asString());
    }
    m_length = data["length"].asFloat();

    const Json::Value& frames = data["frames"];
    for (Json::ValueConstIterator it = frames.begin(); it != frames.end(); it++)
    {
        m_frames.emplace_back(*it);
    }
}

Animation::AnimationSetPlay::AnimationSetPlay(const Animation& animation, const Json::Value& data) :
    m_set(nullptr)
{
    read(animation, data);
}

void Animation::AnimationSetPlay::read(const Animation& animation, const Json::Value& data)
{
    m_position = data["position"].asFloat();

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
    if (!Hexbot::CharReader->parse(str.c_str(), str.c_str() + str.size(), &root, &errors))
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

Animation::BoundFrame& Animation::findBoundFrame(Animation::BoundFrames& frames, uint32_t time)
{
    const auto& f = std::find_if(frames.begin(), frames.end(), [time](const BoundFrame& it)
    {
        return it.getPosition() == time;
    });

    if (f == frames.end())
    {
        for (Animation::BoundFrames::const_iterator it = frames.begin(); it != frames.end(); it++)
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

Animation::BoundFrames Animation::generateFrames() const
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
            Animation::BoundFrame& boundFrame = findBoundFrame(frames, position);
            boundFrame.assign(frame, play.getBindings());
        }
    };

    return std::move(frames);
}

AnimationInstancePtr Animation::newInstance(bool autoPlay)
{
    return AnimationInstance::Create(shared_from_this(), autoPlay);
}

// ------------------

void Animation::BoundFrame::assign(
    const Animation::AnimationFrame& frame,
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
// ------------------

Animation::AnimationFrame::AnimationFrame(const Json::Value& data)
{
    read(data);
}

void Animation::AnimationFrame::read(const Json::Value& data)
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

// ------------------

AnimationInstancePtr AnimationInstance::Create(const AnimationPtr& animation, bool autoPlay)
{
    return AnimationInstancePtr(new AnimationInstance(animation, autoPlay));
}

AnimationInstance::AnimationInstance(const AnimationPtr& animation, bool autoPlay) :
    m_time(0),
    m_animation(animation),
    m_frames(animation->generateFrames()),
    m_currentFrame(m_frames.begin()),
    m_active(autoPlay)
{
}

void AnimationInstance::reset()
{
    m_time = 0;
    m_currentFrame = m_frames.begin();
}

void AnimationInstance::restart(uint32_t delay)
{
    m_currentFrame = m_frames.begin();
    m_time = -delay;
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

void AnimationInstance::bind(const std::string& name, int servo, float coef, int offset)
{
    m_bindings.emplace(std::piecewise_construct,
        std::forward_as_tuple(name),
        std::forward_as_tuple(servo, coef, offset));
}

void AnimationInstance::bind(const Bindings& bindings)
{
    m_bindings = bindings;
}

void AnimationInstance::activateFrame(const Animation::BoundFrame& frame) const
{
    const HexbotPtr& hexbot = Hexbot::getInstance();

    const Animation::BoundFrame::FrameMoves& moves = frame.getMoves();
    for (auto it = moves.begin(); it != moves.end(); it++)
    {
        const auto& frame = it->second;
        const std::string& name = it->first;
        
        auto bit = m_bindings.find(name);
        if (bit == m_bindings.end())
            continue;
        int servo = bit->second.servo;
        float coef = bit->second.coef;
        float offset = bit->second.offset;

        hexbot->moveServo(servo, ((float)std::get<0>(frame) * coef) + offset, std::get<1>(frame));
    }
}

bool AnimationInstance::update(uint32_t dt)
{
    if (!m_active)
        return false;
    
    m_time += dt;
    
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


AnimationGroupPtr AnimationGroup::Create(const std::string& filename)
{
    return AnimationGroupPtr(new AnimationGroup(filename));
}

AnimationGroup::Track::Track(const Json::Value& data)
{
    read(data);
}

void AnimationGroup::Track::read(const Json::Value& data)
{
    m_delay = data["delay"].asFloat();
    
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

AnimationGroup::AnimationGroup(const std::string& filename)
{
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    
    std::string errors;
    Json::Value root;
    if (!Hexbot::CharReader->parse(str.c_str(), str.c_str() + str.size(), &root, &errors))
    {
        std::cerr << "Failed to load animation group " << filename << ": " << errors << std::endl;
        abort();
    }
    
    read(root);
}
    
void AnimationGroup::play(const AnimationPtr& animation, AnimationPlayer& player)
{
    for (Tracks::const_iterator it = m_tracks.begin(); it != m_tracks.end(); it++)
    {
        int index = it - m_tracks.begin();
        
        player.setTrack(index, animation, it->getDelay(), it->getBindings());
    }
}

void AnimationGroup::read(const Json::Value& data)
{
    const Json::Value& tracks = data["tracks"];
    
    for (Json::ValueConstIterator it = tracks.begin(); it != tracks.end(); it++)
    {
        m_tracks.emplace_back(*it);
    }
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

void AnimationPlayer::setTrack(int track, const AnimationInstancePtr& instance)
{
    m_tracks[track] = instance;
}

void AnimationPlayer::setTrack(int track, const AnimationPtr& animation, float delay, const AnimationInstance::Bindings& bindings)
{
    const AnimationInstancePtr& instance = animation->newInstance();
    instance->bind(bindings);
    instance->restart(delay);
    
    setTrack(track, instance);
}
