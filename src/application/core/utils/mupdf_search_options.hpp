#pragma once
#include <QString>

namespace application::core::utils
{

struct MupdfSearchOptions
{
    bool fromStart = false;
    bool wholeWords = false;
    bool caseSensitive = false;
};

}  // namespace application::core::utils
