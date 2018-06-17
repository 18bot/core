
#include "animation.h"
#include "json/reader.h"
#include "utils.h"
#include "main.h"

AnimationPtr Animation::Create(const std::string& filename)
{
    return AnimationPtr(new Animation(filename));
}

Animation::Animation(const std::string& filename)
{
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    
    Json::Value root;
    if (!Json::Reader().parse(str, root))
    {
        std::cerr << "Failed to load animation " << filename << std::endl;
        abort();
    }
    
    read(root);
}

void Animation::read(const Json::Value& data)
{
    m_loop = data["loop"].asBool();
    m_length = data["length"].asFloat();
    
    const Json::Value& frames = data["frames"];
    
    for (Json::ValueConstIterator it = frames.begin(); it != frames.end(); it++)
    {
        m_frames.emplace_back(*it);
    }
}

AnimationInstancePtr Animation::newInstance(bool autoPlay)
{
    return AnimationInstance::Create(shared_from_this(), autoPlay);
}

// ------------------

Animation::AnimationFrame::AnimationFrame(const Json::Value& data)
{
    read(data);
}

void Animation::AnimationFrame::read(const Json::Value& data)
{
    m_position = data["position"].asFloat();
    
    const Json::Value& moves = data["moves"];
    
    for (Json::ValueConstIterator it = moves.begin(); it != moves.end(); it++)
    {
        std::string name = it.name();
        
        const Json::Value& at = *it;
        
        int angle = at[0].asInt();
        float time = at[1].asFloat();
    
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
    m_currentFrame(animation->getFrames().begin()),
    m_active(autoPlay)
{
}

void AnimationInstance::reset()
{
    m_time = 0;
    m_currentFrame = m_animation->getFrames().begin();
}

void AnimationInstance::restart(float delay)
{
    m_currentFrame = m_animation->getFrames().begin();
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

void AnimationInstance::bind(const std::string& name, int servo)
{
    m_bindings[name] = servo;
}

void AnimationInstance::bind(const Bindings& bindings)
{
    m_bindings = bindings;
}

void AnimationInstance::activateFrame(const Animation::AnimationFrame& frame) const
{
    const HexbotPtr& hexbot = Hexbot::getInstance();

    const Animation::AnimationFrame::FrameMoves& moves = frame.getMoves();
    for (Animation::AnimationFrame::FrameMoves::const_iterator it = moves.begin(); it != moves.end(); it++)
    {
        const Animation::AnimationFrame::FrameMove& frame = it->second;
        const std::string& name = it->first;
        
        Bindings::const_iterator bit = m_bindings.find(name);
        if (bit == m_bindings.end())
            continue;
        int servo = bit->second;
        
        hexbot->moveServo(servo, std::get<0>(frame), std::get<1>(frame));
    }
}

bool AnimationInstance::update(float dt)
{
    if (!m_active)
        return false;
    
    m_time += dt;
    
    if (m_currentFrame != m_animation->getFrames().end())
    {
        while (m_time >= m_currentFrame->getPosition())
        {
            activateFrame(*m_currentFrame);
            m_currentFrame++;
            
            if (m_currentFrame == m_animation->getFrames().end())
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
        m_bindings[it.name()] = it->asInt();
    }
}

AnimationGroup::AnimationGroup(const std::string& filename)
{
    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    
    Json::Value root;
    if (!Json::Reader().parse(str, root))
    {
        std::cerr << "Failed to load animation group " << filename << std::endl;
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

void AnimationPlayer::update(float dt)
{
    for (std::map<int, AnimationInstancePtr>::const_iterator it = m_tracks.cbegin(); it != m_tracks.cend() /* not hoisted */; /* no increment */)
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
