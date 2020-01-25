
#ifndef HEXBOT_API
#define HEXBOT_API

// callbacks

namespace api
{
    typedef void (*LogCallback) (const char*);
    typedef bool (*MoveServoCallback) (int servo, float angle, float time);
};

#ifdef WIN32
#define SPEC_API extern "C" __declspec(dllexport)
#else
#define SPEC_API  
#endif

// api itself

extern "C"
{
	SPEC_API int RoboInit(
        const char* contentsDirectory,
        api::LogCallback logCallback,
        api::MoveServoCallback moveServoCallback
    );
    
	SPEC_API void RoboUpdate(float dt);
}

#endif
