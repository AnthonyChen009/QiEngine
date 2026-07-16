add_executable(QiApp
    SandboxApp.cpp
    SandboxScene.cpp
    game/CameraController.cpp
)



target_link_libraries(QiApp PRIVATE QiEngine)

target_compile_definitions(QiApp PRIVATE
    QI_ASSET_PATH="assets"
)

if(MSVC)
    add_custom_command(TARGET QiApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:QiApp>/assets
    )
else()
    add_custom_command(TARGET QiApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:QiApp>/assets
    )
endif()