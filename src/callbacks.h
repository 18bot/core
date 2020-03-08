//
// Created by Desertkun on 3/8/20.
//

#ifndef HEXBOT_CALLBACKS_H
#define HEXBOT_CALLBACKS_H

#include <cstdint>

namespace api
{
    typedef void (*LogCallback) (const char*);
    typedef bool (*MoveServoCallback) (int servo, float angle, uint32_t time);
};

#endif //HEXBOT_CALLBACKS_H
