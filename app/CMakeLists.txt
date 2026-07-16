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
    file(COPY ${CMAKE_SOURCE_DIR}/assets
         DESTINATION ${CMAKE_BINARY_DIR}/app/Debug)
else()
    file(COPY ${CMAKE_SOURCE_DIR}/assets
         DESTINATION ${CMAKE_BINARY_DIR}/app)
endif()
