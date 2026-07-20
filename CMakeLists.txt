cmake_minimum_required(VERSION 3.21)
project(QiEngine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(QiEngine STATIC
    ${CMAKE_SOURCE_DIR}/include/external/stb/stb_image.cpp
    ${CMAKE_SOURCE_DIR}/include/external/tiny_obj_loader/tiny_obj_loader.cpp
)

add_subdirectory(src/core)
add_subdirectory(src/platform)
add_subdirectory(src/renderer)
add_subdirectory(src/os)
add_subdirectory(src/scene)
add_subdirectory(src/utils)
add_subdirectory(app)
add_subdirectory(src/servers)

target_include_directories(QiEngine
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/internal
        ${CMAKE_SOURCE_DIR}/include/external
)

target_precompile_headers(QiEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/include/internal/qipch.hpp
)

target_compile_definitions(QiEngine
    PUBLIC
        $<$<CONFIG:Debug>:QI_DEBUG>
        $<$<CONFIG:Debug>:QI_ENABLE_ASSERTS>
)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/include/external)

find_package(Vulkan REQUIRED)


if(WIN32)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.17.0
    )

    message(STATUS "Fetching spdlog...")
    FetchContent_MakeAvailable(spdlog)
    message(STATUS "spdlog ready")

elseif(UNIX)
    find_package(spdlog REQUIRED)
endif()

if(WIN32)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
    )

    message(STATUS "Fetching glm...")
    FetchContent_MakeAvailable(glm)
    message(STATUS "glm ready")

elseif(UNIX)
    find_package(glm REQUIRED)
endif()



# EnTT
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.16.0
)
message(STATUS "Fetching EnTT...")
FetchContent_MakeAvailable(entt)
message(STATUS "EnTT ready")

#sdl3
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.12
)
message(STATUS "Fetching SDL3...")
FetchContent_MakeAvailable(SDL3)
message(STATUS "SDL3 ready")

FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG v1.0.4  # check for latest tag
)
message(STATUS "Fetching cpptrace...")
FetchContent_MakeAvailable(cpptrace)
message(STATUS "cpptrace ready")

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)

message(STATUS "Fetching ImGui...")
FetchContent_MakeAvailable(imgui)
message(STATUS "ImGui ready")

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    Vulkan::Vulkan
)

target_link_libraries(QiEngine
    PUBLIC
        SDL3::SDL3
        Vulkan::Vulkan
        spdlog::spdlog
        imgui
        cpptrace::cpptrace
)

target_link_libraries(QiEngine
    PUBLIC
        EnTT::EnTT
        glm::glm
)
