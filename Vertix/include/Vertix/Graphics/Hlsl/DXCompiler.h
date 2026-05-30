//
// Created by Natsurainko on 2026/5/30.
//

#pragma once

#include <dxcapi.h>
#include <filesystem>
#include <wrl/client.h>

namespace Vertix {
    class DXCompiler {
        Microsoft::WRL::ComPtr<IDxcLibrary>        library;
        Microsoft::WRL::ComPtr<IDxcUtils>          dxcUtils;
        Microsoft::WRL::ComPtr<IDxcCompiler>       compiler;
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> defaultIncludeHandler;

    public:
        VERTIX_API DXCompiler();

        [[nodiscard]] VERTIX_API Microsoft::WRL::ComPtr<IDxcBlob> Compile(
            const std::filesystem::path &filePath,
            const std::wstring_view &    entryName,
            const std::wstring_view &    shaderModel,
            IDxcIncludeHandler*          includeHandler = nullptr) const;
    };
}
