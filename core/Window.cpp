#include "qipch.hpp"
#include "Window.hpp"

#ifdef QI_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

#ifdef QI_PLATFORM_LINUX
    #include "linux/LinuxWindow.hpp"
#endif

namespace Qi
{
	Scope<Window> Window::create(const WindowProps& props) {
	#ifdef HZ_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
	#else
	    return CreateScope<LinuxWindow>(props);
	#endif
	}

}
