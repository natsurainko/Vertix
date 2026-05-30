//
// Created by Natsurainko on 2026/5/30.
//

#pragma once

#include <filesystem>
#include <d3d12/d3dcommon.h>
#include <wrl/client.h>

namespace Vertix {
    class D3DCompiler {
    public:
        [[nodiscard]] VERTIX_API static Microsoft::WRL::ComPtr<ID3DBlob> Compile(
            const std::filesystem::path &filePath,
            const std::string_view &     entryName,
            const std::string_view &     shaderModel,
            UINT                         compileFlags = 0,
            const D3D_SHADER_MACRO*      defines      = nullptr,
            ID3DInclude*                 include      = nullptr);
    };
}
