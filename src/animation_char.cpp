
#include "animation_char.h"

const std::shared_ptr<Json::CharReader> JSONCharReader::Reader =
    std::shared_ptr<Json::CharReader>(Json::CharReaderBuilder().newCharReader());

