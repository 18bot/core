
#ifndef HEXBOT_ANIMATION
#define HEXBOT_ANIMATION

#include "callbacks.h"
#include "utils.h"

typedef std::shared_ptr<class Animation> AnimationPtr;
typedef std::shared_ptr<class PlayerBindings> PlayerBindingsPtr;
typedef std::shared_ptr<class AnimationInstance> AnimationInstancePtr;

typedef std::list<class BoundFrame> BoundFrames;
typedef std::list<class AnimationFrame> AnimationFrames;
typedef std::map<std::string, class AnimationSet> AnimationSets;
typedef std::list<class AnimationSetPlay> AnimationSetPlays;

class Animation: public std::enable_shared_from_this<Animation>
{
public:
    static AnimationPtr Create(const std::string& filename);
    
public:
    AnimationInstancePtr newInstance(
        api::MoveServoCallback moveCallback,
        const PlayerBindingsPtr& bindings,
        bool autoPlay = false);
    const AnimationSets& getSets() const { return m_sets; }
    bool isLoop() const { return m_loop; }
    uint32_t getLength() const { return m_length; }

    BoundFrames generateFrames() const;

private:
    static BoundFrame& findBoundFrame(BoundFrames& frames, uint32_t time);
    Animation(const std::string& filename);
    
protected:
    void read(const Json::Value& data);
    
private:
    bool m_loop;
    uint32_t m_length;
    AnimationSets m_sets;
    AnimationSetPlays m_plays;
};

class AnimationInstance
{
public:
    static AnimationInstancePtr Create(
        api::MoveServoCallback moveCallback,
        const AnimationPtr& animation,
        const PlayerBindingsPtr& bindings,
        bool autoPlay);
    
private:
    AnimationInstance(
        api::MoveServoCallback moveCallback,
        const AnimationPtr& animation,
        const PlayerBindingsPtr& bindings,
        bool autoPlay);
    
public:
    bool update(uint32_t dt);

    void restart(uint32_t delay = 0, float speed = 1);
    void start();
    void stop();
    
private:
    
    void activateFrame(const BoundFrame& frame) const;
    void reset();
    
private:
    api::MoveServoCallback m_moveCallback;
    uint32_t m_time;
    float m_speed;
    AnimationPtr m_animation;
    PlayerBindingsPtr m_bindings;
    BoundFrames m_frames;
    BoundFrames::const_iterator m_currentFrame;
    bool m_active;
};

class AnimationPlayer;

class PlayerBindings
{
public:
    static PlayerBindingsPtr Create(const std::string& filename);

    struct Binding
    {
        int servo;
        float coef;
        float offset;

        Binding(int servo, float coef, float offset) :
            servo(servo), coef(coef), offset(offset)
        {}
    };

    typedef std::map<std::string, Binding> Bindings;

    const Bindings& getBindings() const { return m_bindings; }

private:
    PlayerBindings(const std::string& filename);

protected:
    void read(const Json::Value& data);
    
private:
    Bindings m_bindings;
};

class AnimationPlayer
{
public:
    AnimationPlayer(api::MoveServoCallback moveCallback);

    void update(uint32_t dt);
    void setTrack(int track, const AnimationInstancePtr& instance);
    void setTrack(int track, const AnimationPtr& animation, float delay, float speed, const PlayerBindingsPtr& bindings);
    
private:
    std::map<int, AnimationInstancePtr> m_tracks;
    api::MoveServoCallback m_moveCallback;
};

#endif
