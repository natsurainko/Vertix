//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_HLSLSHADER_H
#define VERTIX_HLSLSHADER_H

#include <string>
#include <d3d12/d3dcommon.h>
#include <wrl/client.h>

namespace Vertix {
    class HlslShader {
    public:
        explicit HlslShader(std::wstring filePath, UINT compileFlags = 0);

        void Compile(const std::string &entryName,
                     const std::string &shaderModel,
                     const D3D_SHADER_MACRO* defines = nullptr,
                     ID3DInclude* include = nullptr);

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3DBlob>& GetShaderBlob() const {
            return shaderBlob;
        }

    private:
        std::wstring filePath;
        UINT compileFlags;

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
    };
}

#endif //VERTIX_HLSLSHADER_H