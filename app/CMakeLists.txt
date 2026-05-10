add_executable(QiApp
    SandboxApp.cpp
    SandboxLayer.cpp
    SandboxScene.cpp
)



target_link_libraries(QiApp PRIVATE QiEngine)

target_compile_definitions(QiApp PRIVATE
    QI_ASSET_PATH="assets"
)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/app)
