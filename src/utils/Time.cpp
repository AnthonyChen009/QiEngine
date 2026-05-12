#include "Time.hpp"
#include <GLFW/glfw3.h>

namespace Qi {

float Time::getTime() {
    return (float)glfwGetTime();
}

}
