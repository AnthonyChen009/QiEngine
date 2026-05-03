cmake_minimum_required(VERSION 3.21)
project(QiEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(QiEngine STATIC)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(renderer)
add_subdirectory(app)

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
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)


target_link_libraries(QiEngine
    PUBLIC
        glfw
        Vulkan::Vulkan
        spdlog::spdlog
        glm::glm
)
