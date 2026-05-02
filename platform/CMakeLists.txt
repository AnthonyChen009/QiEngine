add_library(QiPlatform
    linux/LinuxWindow.cpp
    linux/LinuxWindow.hpp
)

target_include_directories(QiPlatform
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(glfw3 REQUIRED)

target_precompile_headers(QiPlatform PRIVATE
    ${CMAKE_SOURCE_DIR}/include/qipch.hpp
)

target_link_libraries(QiPlatform PUBLIC QiCore glfw)
