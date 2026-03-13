//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_HLSLSHADER_H
#define VERTIX_HLSLSHADER_H

#include <d3dcompiler.h>
#include <dxcapi.h>
#include <string>
#include <d3d12/d3dcommon.h>
#include <wrl/client.h>

#include "VERTIX_EXPORT.h"

namespace Vertix {
    class VERTIX_API HlslShader {
    public:
        explicit HlslShader(
            std::wstring filePath,
            const UINT compileFlags = 0) : filePath(std::move(filePath)), compileFlags(compileFlags) {}

        void Compile(
            const std::wstring &entryName,
            const std::wstring &shaderModel,
            IDxcIncludeHandler* includeHandler = nullptr);

        void Compile(
            const std::string &entryName,
            const std::string &shaderModel,
            const D3D_SHADER_MACRO* defines = nullptr,
            ID3DInclude* include = nullptr);

        [[nodiscard]]
        ID3DBlob* GetShaderBlob() const noexcept {
            if (compiledByDxc) {
                return reinterpret_cast<ID3DBlob *>(dxcShaderBlob.Get());
            }

            return shaderBlob.Get();
        }

    private:
        std::wstring filePath;
        UINT compileFlags;
        bool compiledByDxc = false;

        Microsoft::WRL::ComPtr<IDxcBlob> dxcShaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
    };
}

#endif //VERTIX_HLSLSHADER_H