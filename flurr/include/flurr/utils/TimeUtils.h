#pragma once

#include "flurr/FlurrDefines.h"

#include <string>

namespace flurr
{

/** Get time in milliseconds since first call to CurrentTime. */
FLURR_DLL_EXPORT double CurrentTime();

/** Get current time as string formatted as MM:SS.ms. */
FLURR_DLL_EXPORT std::string CurrentTimeAsStr();

} // namespace flurr
