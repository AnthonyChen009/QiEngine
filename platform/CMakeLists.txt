if(WIN32)

elseif(UNIX)
    target_sources(QiEngine PRIVATE
        linux/LinuxWindow.cpp
    )
endif()
