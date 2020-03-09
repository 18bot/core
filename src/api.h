
#ifndef HEXBOT_API
#define HEXBOT_API

// callbacks
#include <cstdint>
#include "callbacks.h"

#ifdef HEXBOT_STATIC
#   define SPEC_API
#else
#   ifdef WIN32
#   define SPEC_API extern "C" __declspec(dllexport)
#   else
#   define SPEC_API
#   endif
#endif

// api itself

extern "C"
{
	SPEC_API int RoboInit(
        const char* contentsDirectory,
        api::LogCallback logCallback,
        api::MoveServoCallback moveServoCallback
    );
    
	SPEC_API void RoboUpdate(uint32_t dt);

	enum MovementState
    {
	    MOVE_Stop = 0,
        MOVE_Forward,
        MOVE_Backward,
        MOVE_Left,
        MOVE_Right,
        MOVE_Sit
    };

    SPEC_API void RoboMove(MovementState state);
}

#endif
