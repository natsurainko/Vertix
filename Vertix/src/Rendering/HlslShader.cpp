//
// Created by Natsurainko on 2026/1/10.
//

#include "Vertix/Rendering/HlslShader.h"

#include "Vertix/Exceptions/HResultException.h"

using Microsoft::WRL::ComPtr;

void Vertix::HlslShader::Compile(
    const std::wstring &entryName,
    const std::wstring &shaderModel,
    IDxcIncludeHandler* includeHandler)
{
    ComPtr<IDxcLibrary> library;
    ComPtr<IDxcUtils> dxcUtils;
    ComPtr<IDxcCompiler> compiler;
    ComPtr<IDxcIncludeHandler> defaultIncludeHandler;

    ThrowIfFailed(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library)));
    ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils)));
    ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));

    if (!includeHandler) {
        ThrowIfFailed(library->CreateIncludeHandler(&defaultIncludeHandler));
    }

    ComPtr<IDxcBlobEncoding> sourceBlob;
    ComPtr<IDxcBlobEncoding> errorBlob;
    ComPtr<IDxcOperationResult> compileResult;

    ThrowIfFailed(dxcUtils->LoadFile(filePath.c_str(), nullptr, &sourceBlob));
    ThrowIfFailed(compiler->Compile(
        sourceBlob.Get(),
        filePath.c_str(),
        entryName.c_str(),
        shaderModel.c_str(),
        nullptr,
        0,
        nullptr,
        0,
        includeHandler ? includeHandler : defaultIncludeHandler.Get(),
        &compileResult
    ));

    HRESULT hr;
    ThrowIfFailed(compileResult->GetErrorBuffer(&errorBlob));
    ThrowIfFailed(compileResult->GetStatus(&hr));

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA(static_cast<char *>(errorBlob->GetBufferPointer()));
            throw HResultException(E_FAIL);
        }
    }

    compiledByDxc = true;
    ThrowIfFailed(compileResult->GetResult(&dxcShaderBlob));
}

void Vertix::HlslShader::Compile(
    const std::string &entryName,
    const std::string &shaderModel,
    const D3D_SHADER_MACRO* defines,
    ID3DInclude* include)
{
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
