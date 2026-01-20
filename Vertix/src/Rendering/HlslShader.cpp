//
// Created by Natsurainko on 2026/1/10.
//

#include "Rendering/HlslShader.h"

#include <d3dcompiler.h>
#include <utility>

#include "Exceptions/HResultException.h"

using Microsoft::WRL::ComPtr;

Vertix::HlslShader::HlslShader(std::wstring filePath, const UINT compileFlags)
    : filePath(std::move(filePath)), compileFlags(compileFlags) {}

void Vertix::HlslShader::Compile(const std::string &entryName, const std::string &shaderModel,
    const D3D_SHADER_MACRO* defines, ID3DInclude* include) {
    ComPtr<ID3DBlob> errorBlob;

    const HRESULT hr = D3DCompileFromFile(
        filePath.c_str(),
        defines,
        include,
        entryName.c_str(),
        shaderModel.c_str(),
        compileFlags,
        0,
        &shaderBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA(static_cast<char *>(errorBlob->GetBufferPointer()));
        }

        throw HResultException(hr);
    }
}
