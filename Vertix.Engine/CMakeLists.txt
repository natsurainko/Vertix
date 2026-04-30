add_library(Vertix.Engine SHARED)
set_target_properties(Vertix.Engine PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(Vertix.Engine PRIVATE Vertix_Engine_EXPORTS)
target_sources(Vertix.Engine
    PRIVATE
        src/Camera/PerspectiveCamera.cpp
        src/Content/ModelLoader.cpp
        src/Content/TextureLoader.cpp
        src/Content/TextureMipmapGenerator.cpp
        src/Input/GameInputInterface.cpp
        src/Input/KeyboardDevice.cpp
        src/Input/MouseDevice.cpp
        src/Primitive/DefaultPBRMaterial.cpp
        src/GameObject3D.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/Vertix.Engine/VERTIX_ENGINE_EXPORT.h

            include/Vertix.Engine/Camera/PerspectiveCamera.h
            include/Vertix.Engine/Content/ModelLoader.h
            include/Vertix.Engine/Content/TextureLoader.h
            include/Vertix.Engine/Content/TextureMipmapGenerator.h
            include/Vertix.Engine/Controlling/DefaultPositionController.hpp
            include/Vertix.Engine/Controlling/DefaultRotationController.hpp
            include/Vertix.Engine/Controlling/IControllerInput.hpp
            include/Vertix.Engine/Effect/Shadow/CascadeShadowMapping.h
            include/Vertix.Engine/Helpers/MathHelper.h
            include/Vertix.Engine/Helpers/VectorHelper.h
            include/Vertix.Engine/Helpers/FrameCounter.hpp
            include/Vertix.Engine/Input/GeneralKeyboardDevice.hpp
            include/Vertix.Engine/Input/GeneralMouseDevice.hpp
            include/Vertix.Engine/Input/GameInputInterface.h
            include/Vertix.Engine/Input/KeyboardDevice.h
            include/Vertix.Engine/Input/MouseDevice.h
            include/Vertix.Engine/Input/InputDevice.hpp
            include/Vertix.Engine/Pool/DefaultMaterialPool.hpp
            include/Vertix.Engine/Primitive/DefaultPBRMaterial.h
            include/Vertix.Engine/Scene/DirectionalLight.h
            include/Vertix.Engine/Scene/SceneObject3D.hpp
            include/Vertix.Engine/GameObject3D.h
)

set(HLSL_SHADER_FILES shaders/GenerateMipmap.hlsl)
set_source_files_properties(shaders/GenerateMipmap.hlsl PROPERTIES
        ShaderModel  "6_0"
        CSMain       "CSMain"
        ByteCodeName "GENERATE_MIPMAP_PASS"
)
add_hlsl_shaders(Vertix.Engine FILES ${HLSL_SHADER_FILES})

target_include_directories(Vertix.Engine BEFORE PUBLIC ${INCLUDES_DIR})
target_link_libraries(Vertix.Engine PUBLIC Vertix GameInput assimp)
