//
// Created by Natsurainko on 2026/5/30.
//

#include "Vertix/Graphics/Hlsl/DXCompiler.h"

Vertix::DXCompiler::DXCompiler() {
    ThrowIfFailed(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library)));
    ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils)));
    ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
    ThrowIfFailed(library->CreateIncludeHandler(&defaultIncludeHandler));
}

Microsoft::WRL::ComPtr<IDxcBlob> Vertix::DXCompiler::Compile(
    const std::filesystem::path &filePath,
    const std::wstring_view &    entryName,
    const std::wstring_view &    shaderModel,
    IDxcIncludeHandler*          includeHandler) const {
    Microsoft::WRL::ComPtr<IDxcBlob>            dxcShaderBlob;
    Microsoft::WRL::ComPtr<IDxcBlobEncoding>    sourceBlob;
    Microsoft::WRL::ComPtr<IDxcBlobEncoding>    errorBlob;
    Microsoft::WRL::ComPtr<IDxcOperationResult> compileResult;

    ThrowIfFailed(
        dxcUtils->LoadFile(
            filePath.wstring().c_str(),
            nullptr,
            &sourceBlob
        )
    );

    ThrowIfFailed(
        compiler->Compile(
            sourceBlob.Get(),
            filePath.wstring().c_str(),
            entryName.data(),
            shaderModel.data(),
            nullptr,
            0,
            nullptr,
            0,
            includeHandler ? includeHandler : defaultIncludeHandler.Get(),
            &compileResult
        )
    );

    HRESULT hr;
    ThrowIfFailed(compileResult->GetErrorBuffer(&errorBlob));
    ThrowIfFailed(compileResult->GetStatus(&hr));

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
            throw HResultException(E_FAIL);
        }
    }

    ThrowIfFailed(compileResult->GetResult(&dxcShaderBlob));
    return dxcShaderBlob;
}
