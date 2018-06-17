
#ifndef HEXBOT_ANIMATION
#define HEXBOT_ANIMATION

#include "api.h"
#include "utils.h"

typedef std::shared_ptr<class Animation> AnimationPtr;
typedef std::shared_ptr<class AnimationGroup> AnimationGroupPtr;
typedef std::shared_ptr<class AnimationInstance> AnimationInstancePtr;

class Animation: public std::enable_shared_from_this<Animation>
{
public:
    class AnimationFrame
    {
        friend class AnimationInstance;
        
    public:
        AnimationFrame(const Json::Value& data);
        typedef std::tuple<int, float> FrameMove;
        typedef std::map<std::string, FrameMove> FrameMoves;
        
    protected:
        void read(const Json::Value& data);
        
    public:
        float getPosition() const { return m_position; }
        const FrameMoves& getMoves() const { return m_moves; }
    
    private:
        FrameMoves m_moves;
        float m_position;
    };
    
    typedef std::list<AnimationFrame> AnimationFrames;
    
public:
    static AnimationPtr Create(const std::string& filename);
    
public:
    AnimationInstancePtr newInstance(bool autoPlay = false);
    const AnimationFrames& getFrames() const { return m_frames; }
    bool isLoop() const { return m_loop; }
    float getLength() const { return m_length; }

private:
    Animation(const std::string& filename);
    
protected:
    void read(const Json::Value& data);
    
private:
    bool m_loop;
    float m_length;
    AnimationFrames m_frames;
};

class AnimationInstance
{
public:
    typedef std::map<std::string, int> Bindings;
    
public:
    static AnimationInstancePtr Create(const AnimationPtr& animation, bool autoPlay);
    
private:
    AnimationInstance(const AnimationPtr& animation, bool autoPlay);
    
public:
    bool update(float dt);
    
    void bind(const std::string& name, int servo);
    void bind(const Bindings& bindings);
    
    void restart(float delay = 0);
    void start();
    void stop();
    
private:
    
    void activateFrame(const Animation::AnimationFrame& frame) const;
    void reset();
    
private:
    float m_time;
    AnimationPtr m_animation;
    Animation::AnimationFrames::const_iterator m_currentFrame;
    bool m_active;
    Bindings m_bindings;
};

class AnimationPlayer;

class AnimationGroup
{
public:
    static AnimationGroupPtr Create(const std::string& filename);
    
public:
    class Track
    {
    public:
        Track(const Json::Value& data);
        
    private:
        void read(const Json::Value& data);
        
    public:
        float getDelay() const { return m_delay; }
        const AnimationInstance::Bindings& getBindings() const { return m_bindings; }
        
    private:
        float m_delay;
        AnimationInstance::Bindings m_bindings;
    };
    
    typedef std::vector<Track> Tracks;
    
private:
    AnimationGroup(const std::string& filename);
    
public:
    void play(const AnimationPtr& animation, AnimationPlayer& player);
    
protected:
    void read(const Json::Value& data);
    
private:
    Tracks m_tracks;
};

class AnimationPlayer
{
public:
    void update(float dt);
    void setTrack(int track, const AnimationInstancePtr& instance);
    void setTrack(int track, const AnimationPtr& animation, float delay, const AnimationInstance::Bindings& bindings);
    
private:
    std::map<int, AnimationInstancePtr> m_tracks;
};

#endif
