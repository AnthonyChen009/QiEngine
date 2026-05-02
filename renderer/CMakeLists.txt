add_library(QiRenderer
    VulkanRenderer.cpp
    VulkanRenderer.hpp
)

target_include_directories(QiRenderer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Vulkan REQUIRED)

target_precompile_headers(QiRenderer PRIVATE
    ${CMAKE_SOURCE_DIR}/include/qipch.hpp
)

target_link_libraries(QiRenderer PUBLIC Vulkan::Vulkan QiPlatform)
