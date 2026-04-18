function(add_hlsl_shaders TARGET_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs FILES)
    cmake_parse_arguments(HLSL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(GENERATED_HEADERS "")

    foreach(FILE ${HLSL_FILES})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(SHADER_MODEL ${FILE} ShaderModel)
        get_source_file_property(BYTECODE_NAME ${FILE} ByteCodeName)
        get_source_file_property(DXR_EXPORTS ${FILE} DxrExports)

        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(COMPILE_FLAGS -Zi -Od -Qembed_debug)
        else()
            set(COMPILE_FLAGS -O3 -Qstrip_debug -Qstrip_reflect)
        endif()

        if(DXR_EXPORTS)
            set(OUTPUT_H ${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders/${FILE_WE}_DXR.h)
            set(DEP_FILE ${OUTPUT_H}.d)
            set(VARIABLE_NAME SHADER_BYTECODE_${BYTECODE_NAME}_DXR)

            add_custom_command(
                    OUTPUT ${OUTPUT_H} ${DEP_FILE}

                    COMMAND dxc.exe
                        -T lib_${SHADER_MODEL}
                        ${COMPILE_FLAGS}
                        -Fh ${OUTPUT_H}
                        -Vn ${VARIABLE_NAME}
                        ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
                    COMMAND dxc.exe
                        -T lib_${SHADER_MODEL}
                        ${COMPILE_FLAGS}
                        -MD -MF ${DEP_FILE}
                        -Fo nul
                        ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
                    DEPFILE ${DEP_FILE}
                    COMMENT "[DXC] ${FILE}::DXR_EXPORTS [lib_${SHADER_MODEL}] -> ${OUTPUT_H}"
                    VERBATIM
            )

            list(APPEND GENERATED_HEADERS ${OUTPUT_H})
            continue()
        endif()

        if(NOT SHADER_MODEL OR NOT BYTECODE_NAME)
            continue()
        endif()

        foreach(ENTRY_PROP VSMain PSMain CSMain GSMain MSMain)
            get_source_file_property(ENTRY_FUNC ${FILE} ${ENTRY_PROP})
            if(NOT ENTRY_FUNC)
                continue()
            endif()

            string(REGEX REPLACE "Main$" "" ENTRY_TYPE ${ENTRY_PROP})
            string(TOUPPER ${ENTRY_TYPE} ENTRY_TYPE_UPPER)
            string(TOLOWER ${ENTRY_TYPE} ENTRY_TYPE_LOWER)

            set(OUTPUT_H ${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders/${FILE_WE}_${ENTRY_TYPE_UPPER}.h)
            set(DEP_FILE ${OUTPUT_H}.d)
            set(VARIABLE_NAME SHADER_BYTECODE_${BYTECODE_NAME}_${ENTRY_TYPE_UPPER})

            add_custom_command(
                    OUTPUT ${OUTPUT_H} ${DEP_FILE}
                    COMMAND dxc.exe
                        -T ${ENTRY_TYPE_LOWER}_${SHADER_MODEL}
                        -E ${ENTRY_FUNC}
                        ${COMPILE_FLAGS}
                        -Fh ${OUTPUT_H}
                        -Vn ${VARIABLE_NAME}
                        ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
                    COMMAND dxc.exe
                        -T ${ENTRY_TYPE_LOWER}_${SHADER_MODEL}
                        -E ${ENTRY_FUNC}
                        ${COMPILE_FLAGS}
                        -MD -MF ${DEP_FILE}
                        -Fo nul
                    ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
                    DEPFILE ${DEP_FILE}
                    COMMENT "[DXC] ${FILE}::${ENTRY_FUNC} [${ENTRY_TYPE_LOWER}_${SHADER_MODEL}] -> ${OUTPUT_H}"
                    VERBATIM
            )


            list(APPEND GENERATED_HEADERS ${OUTPUT_H})
        endforeach()
    endforeach()

    add_custom_target(${TARGET_NAME}_Shaders DEPENDS ${GENERATED_HEADERS})
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_Shaders)
    target_sources(${TARGET_NAME}
            PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders
            FILES ${GENERATED_HEADERS})
endfunction()