//
// Created by Natsurainko on 2026/1/24.
//

#ifndef VERTIX_WINDOWOPTIONS_H
#define VERTIX_WINDOWOPTIONS_H

#include <d3d12/dxgiformat.h>

#include "Math/Vector2D.h"

namespace Vertix {
    struct WindowOptions {
        UINT swapChainFrameCount;
        DXGI_FORMAT swapChainFormat;
        Vector2D<UINT> windowSize;
        std::wstring windowTitle;
        std::wstring windowClassName;
        bool enableVSync{};

        [[nodiscard]]
        static WindowOptions GetDefaultWindowOptions() {
            WindowOptions options;
            options.swapChainFrameCount = 2;
            options.swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            options.windowSize = Vector2D<UINT>(800, 600);
            options.windowTitle = std::wstring(L"Vertix.GameWindow");
            options.windowClassName = std::wstring(L"Vertix_GameWindow");
            options.enableVSync = true;
            return options;
        }
    };
}

#endif //VERTIX_WINDOWOPTIONS_H