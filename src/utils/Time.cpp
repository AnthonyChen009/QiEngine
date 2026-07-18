#include "Time.hpp"
#include <SDL3/SDL.h>

namespace Qi {

static uint64_t s_startCounter = 0;
static bool s_initialized = false;

float Time::getTime() {
    if (!s_initialized) {
        s_startCounter = SDL_GetPerformanceCounter();
        s_initialized = true;
    }
    double frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    double elapsed = static_cast<double>(SDL_GetPerformanceCounter() - s_startCounter) / frequency;
    return static_cast<float>(elapsed);
}

}
