//
// Created by Desertkun on 3/8/20.
//

#ifndef HEXBOT_ANIMATION_CHAR_H
#define HEXBOT_ANIMATION_CHAR_H

#include "json/reader.h"
#include <memory>

class JSONCharReader
{
public:
    static const std::shared_ptr<Json::CharReader> Reader;
};

#endif //HEXBOT_ANIMATION_CHAR_H
