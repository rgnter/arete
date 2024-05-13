function(target_compile_shaders Target ShaderFolder)
    set(BuildShaderFolder "${PROJECT_BINARY_DIR}/${ShaderFolder}")
    file(MAKE_DIRECTORY ${BuildShaderFolder})
    file(GLOB GLSL_SOURCE_FILES
            "${ShaderFolder}/*.glsl"
    )

    foreach(GLSL_SOURCE_FILE ${GLSL_SOURCE_FILES})
        get_filename_component(GLSL_SOURCE_FILE_NAME ${GLSL_SOURCE_FILE} NAME_WE)
        set(SPIRV_BINARY_FILE "${BuildShaderFolder}/${GLSL_SOURCE_FILE_NAME}.spv")

        add_custom_command(
                OUTPUT ${SPIRV_BINARY_FILE}
                COMMAND glslc ${GLSL_SOURCE_FILE} -o ${SPIRV_BINARY_FILE}
                DEPENDS ${GLSL_SOURCE_FILE})

        list(APPEND SPIRV_BINARY_FILES ${SPIRV_BINARY_FILE})
    endforeach(GLSL_SOURCE_FILE)

    add_custom_target(Shaders
            DEPENDS ${SPIRV_BINARY_FILES})
    add_dependencies(engine Shaders)

    set(TARGET_BUILD_FOLDER )
    # Copy shaders from build directory to target build directory.
    add_custom_command(
            TARGET ${Target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${Target}>/${ShaderFolder}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${BuildShaderFolder}" "$<TARGET_FILE_DIR:${Target}>/${ShaderFolder}")
endfunction()




