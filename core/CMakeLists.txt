add_library(QiCore
    Application.cpp
    Application.hpp
    Base.hpp
    Log.hpp
    Log.cpp
    PlatformDetection.hpp
    events/Event.hpp
    Window.cpp
    Window.hpp
    events/ApplicationEvent.hpp
    Assert.hpp
)

target_include_directories(QiCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_precompile_headers(QiCore PRIVATE
    ${CMAKE_SOURCE_DIR}/include/qipch.hpp
)

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(QiCore PUBLIC QiPlatform glm::glm spdlog::spdlog)
