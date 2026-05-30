//
// Created by Natsurainko on 2026/5/30.
//

#include "Vertix/Graphics/Hlsl/D3DCompiler.h"

#include <d3dcompiler.h>

Microsoft::WRL::ComPtr<ID3DBlob> Vertix::D3DCompiler::Compile(
    const std::filesystem::path &filePath,
    const std::string_view &     entryName,
    const std::string_view &     shaderModel,
    const UINT                   compileFlags,
    const D3D_SHADER_MACRO*      defines,
    ID3DInclude*                 include) {
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

    const HRESULT hr = D3DCompileFromFile(
        filePath.wstring().c_str(),
        defines,
        include,
        entryName.data(),
        shaderModel.data(),
        compileFlags,
        0,
        &shaderBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
        }

        throw HResultException(hr);
    }

    return shaderBlob;
}
