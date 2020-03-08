
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

    typedef std::list<AnimationFrame> AnimationFrames;

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

    typedef std::list<BoundFrame> BoundFrames;

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

    typedef std::map<std::string, AnimationSet> AnimationSets;

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

    typedef std::list<AnimationSetPlay> AnimationSetPlays;

public:
    static AnimationPtr Create(const std::string& filename);
    
public:
    AnimationInstancePtr newInstance(bool autoPlay = false);
    const AnimationSets& getSets() const { return m_sets; }
    bool isLoop() const { return m_loop; }
    uint32_t getLength() const { return m_length; }

    BoundFrames generateFrames() const;

private:
    static Animation::BoundFrame& findBoundFrame(Animation::BoundFrames& frames, uint32_t time);
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
    
public:
    static AnimationInstancePtr Create(const AnimationPtr& animation, bool autoPlay);
    
private:
    AnimationInstance(const AnimationPtr& animation, bool autoPlay);
    
public:
    bool update(uint32_t dt);
    
    void bind(const std::string& name, int servo, float coef, int offset);
    void bind(const Bindings& bindings);
    
    void restart(uint32_t delay = 0);
    void start();
    void stop();
    
private:
    
    void activateFrame(const Animation::BoundFrame& frame) const;
    void reset();
    
private:
    uint32_t m_time;
    AnimationPtr m_animation;
    Animation::BoundFrames m_frames;
    Animation::BoundFrames::const_iterator m_currentFrame;
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
        uint32_t getDelay() const { return m_delay; }
        const AnimationInstance::Bindings& getBindings() const { return m_bindings; }
        
    private:
        uint32_t m_delay;
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
    void update(uint32_t dt);
    void setTrack(int track, const AnimationInstancePtr& instance);
    void setTrack(int track, const AnimationPtr& animation, float delay, const AnimationInstance::Bindings& bindings);
    
private:
    std::map<int, AnimationInstancePtr> m_tracks;
};

#endif
