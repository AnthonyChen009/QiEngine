target_sources(QiEngine PRIVATE
    graphicsAPI/VulkanPlatform.cpp
)

if(WIN32)
    target_sources(QiEngine PRIVATE
        windows/WindowsWindow.cpp
    )
elseif(UNIX)
    target_sources(QiEngine PRIVATE
        linux/LinuxWindow.cpp
    )
endif()