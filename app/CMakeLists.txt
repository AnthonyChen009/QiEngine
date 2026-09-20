add_executable(QiApp
    SandboxApp.cpp
    SandboxScene.cpp
    game/CameraController.cpp
)



target_link_libraries(QiApp PRIVATE QiEngine)

target_compile_definitions(QiApp PRIVATE
    QI_ASSET_PATH="assets"
)

find_program(GLSLC glslc REQUIRED)



file(GLOB_RECURSE SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/assets/shaders/*.vert
    ${CMAKE_SOURCE_DIR}/assets/shaders/*.frag
    ${CMAKE_SOURCE_DIR}/assets/shaders/RT/*.vert
    ${CMAKE_SOURCE_DIR}/assets/shaders/RT/*.frag
    ${CMAKE_SOURCE_DIR}/assets/shaders/RT/*.rgen
    ${CMAKE_SOURCE_DIR}/assets/shaders/RT/*.rmiss
    ${CMAKE_SOURCE_DIR}/assets/shaders/RT/*.rchit
    ${CMAKE_SOURCE_DIR}/assets/shaders/RT/*.rahit
    ${CMAKE_SOURCE_DIR}/assets/shaders/RT/*.rint
)

set(SHADER_OUTPUTS "")
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME_WE ${SHADER} NAME_WE)
    get_filename_component(SHADER_DIR ${SHADER} DIRECTORY)
    set(SPIRV "${SHADER_DIR}/${SHADER_NAME_WE}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLC} --target-env=vulkan1.2 ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${SHADER_NAME_WE}"
        VERBATIM
    )
    list(APPEND SHADER_OUTPUTS ${SPIRV})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(QiApp CompileShaders)

file(GLOB_RECURSE ASSET_FILES ${CMAKE_SOURCE_DIR}/assets/*)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/asset_copy_stamp
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:QiApp>/assets
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/asset_copy_stamp
    DEPENDS ${ASSET_FILES} ${SHADER_OUTPUTS}
    COMMENT "Copying assets"
)
add_custom_target(CopyAssets ALL DEPENDS ${CMAKE_BINARY_DIR}/asset_copy_stamp)
add_dependencies(QiApp CopyAssets)

if(WIN32)
    add_custom_command(TARGET QiApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:QiApp>
    )
endif()
