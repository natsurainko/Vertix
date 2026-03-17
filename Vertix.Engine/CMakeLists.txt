add_library(Vertix.Engine SHARED)
set_target_properties(Vertix.Engine PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(Vertix.Engine PRIVATE Vertix_Engine_EXPORTS)
target_sources(Vertix.Engine
    PRIVATE
        src/Camera/PerspectiveCamera.cpp
        src/Content/ModelLoader.cpp
        src/Content/TextureLoader.cpp
        src/Input/GameInputInterface.cpp
        src/Input/KeyboardDevice.cpp
        src/Input/MouseDevice.cpp
        src/GameObject3D.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/VERTIX_ENGINE_EXPORT.h

            include/Camera/PerspectiveCamera.h
            include/Content/ModelLoader.h
            include/Content/TextureLoader.h
            include/Controlling/DefaultPositionController.hpp
            include/Controlling/DefaultRotationController.hpp
            include/Controlling/IControllerInput.hpp
            include/Math/FloatExtensions.h
            include/Math/MatrixExtensions.h
            include/Math/VectorExtensions.h
            include/Helpers/FrameCounter.hpp
            include/Input/GeneralKeyboardDevice.hpp
            include/Input/GeneralMouseDevice.hpp
            include/Input/GameInputInterface.h
            include/Input/KeyboardDevice.h
            include/Input/MouseDevice.h
            include/Input/InputDevice.hpp
            include/Scene/DirectionalLight.h
            include/Scene/SceneObject3D.hpp
            include/GameObject3D.h
)

target_include_directories(Vertix BEFORE PUBLIC ${INCLUDES_DIR})
target_link_libraries(Vertix.Engine PUBLIC Vertix GameInput assimp)
