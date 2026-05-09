cmake_minimum_required(VERSION 3.21)
project(QiEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(QiEngine STATIC)
add_library(glad ${CMAKE_SOURCE_DIR}/include/external/glad/src/glad.c)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(renderer)
add_subdirectory(app)

target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/include/external/glad/include
)

target_include_directories(QiEngine
    PUBLIC
        ${CMAKE_SOURCE_DIR}/core
        ${CMAKE_SOURCE_DIR}/platform
        ${CMAKE_SOURCE_DIR}/renderer
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/internal
)

target_precompile_headers(QiEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/include/internal/qipch.hpp
)

target_compile_definitions(QiEngine
    PUBLIC
        $<$<CONFIG:Debug>:QI_ENABLE_ASSERTS>
)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)

include(FetchContent)
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/include/external/entt
)
FetchContent_MakeAvailable(entt)


target_link_libraries(QiEngine
    PUBLIC
        glfw
        Vulkan::Vulkan
        spdlog::spdlog
        glm::glm
        glad
        OpenGL::GL
        EnTT::EnTT
)
