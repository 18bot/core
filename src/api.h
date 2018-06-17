
#ifndef HEXBOT_API
#define HEXBOT_API

// callbacks

namespace api
{
    typedef void (*LogCallback) (const char*);
    typedef bool (*GetGyroscopeDataCallback) (float& x, float& y, float& z);
    typedef bool (*GetAccelerometerDataCallback) (float& x, float& y, float& z);
    typedef bool (*MoveServoCallback) (int servo, float angle, float time);
    typedef bool (*RequestCameraSnapshotCallback) ();
};

#ifdef WIN32
#define SPEC_API __declspec(dllexport)
#else
#define SPEC_API  
#endif

// api itself

extern "C"
{
	SPEC_API int RoboInit(
        const char* contentsDirectory,
        api::LogCallback logCallback,
        api::GetGyroscopeDataCallback getGyroscopeDataCallback,
        api::GetAccelerometerDataCallback getAccelerometerDataCallback,
        api::MoveServoCallback moveServoCallback,
        api::RequestCameraSnapshotCallback requestCameraSnapshotCallback
    );
    
	SPEC_API void RoboUpdate(float dt);
    
	SPEC_API void RoboCameraSnapshot(int width, int height, int dataLength, void* data);
}

#endif
