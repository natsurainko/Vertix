add_library(Vertix.Engine SHARED
        include/Camera/PerspectiveCamera.h
        include/Content/ModelImporter.h
        include/Controlling/IControllerInput.h
        include/Controlling/DefaultPositionController.h
        include/Controlling/DefaultRotationController.h
        include/Math/FloatExtensions.h
        include/Math/MatrixExtensions.h
        include/Helpers/FrameCounter.h
        include/Input/GeneralKeyboardDevice.h
        include/Input/GeneralMouseDevice.h
        include/Input/GameInputInterface.h
        include/Input/KeyboardDevice.h
        include/Input/MouseDevice.h
        include/Input/InputDevice.h
        include/Scene/DirectionalLight.h
        include/Scene/SceneObject3D.h
        include/GameObject3D.h

        src/Camera/PerspectiveCamera.cpp
        src/Content/ModelImporter.cpp
        src/Input/GameInputInterface.cpp
        src/Input/KeyboardDevice.cpp
        src/Input/MouseDevice.cpp
        src/Scene/SceneObject3D.cpp
        src/GameObject3D.cpp
        src/SimpleMath.cpp
)

target_link_libraries(Vertix.Engine PUBLIC Vertix assimp-vc145-mt.lib GameInput.lib)

target_include_directories(Vertix.Engine BEFORE PUBLIC ${INCLUDES_DIR})
target_include_directories(Vertix.Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_command(TARGET Vertix.Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIBRARIES_DIR}/assimp-vc145-mt.dll"
        $<TARGET_FILE_DIR:Vertix.Engine>
        COMMENT "Copying assimp dll to output folder.."
)

if (MSVC)
    set_target_properties(Vertix.Engine PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif ()