#pragma once
#include <cstdint>
#include <SDL3/SDL_mouse.h>

namespace Qi {
    using MouseCode = uint16_t;
    namespace Mouse {
       	enum : MouseCode {
      		Button0     = SDL_BUTTON_LEFT,   // 1
      		Button1     = SDL_BUTTON_MIDDLE, // 2
      		Button2     = SDL_BUTTON_RIGHT,  // 3
      		Button3     = SDL_BUTTON_X1,     // 4
      		Button4     = SDL_BUTTON_X2,     // 5
      		ButtonLast  = Button4,
      		ButtonLeft  = SDL_BUTTON_LEFT,
      		ButtonRight = SDL_BUTTON_RIGHT,
      		ButtonMiddle = SDL_BUTTON_MIDDLE
       	};
    }
    enum class CursorMode {
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};
}
