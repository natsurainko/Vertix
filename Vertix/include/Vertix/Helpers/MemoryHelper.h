//
// Created by Natsurainko on 2026/5/24.
//

#ifndef VERTIX_MEMORYHELPER_H
#define VERTIX_MEMORYHELPER_H

#include <debugapi.h>
#include <string>

namespace Vertix {
    template<typename T>
    void hexdump(const T* obj) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(obj);
        std::string out;
        out.reserve(sizeof(T) * 3 + sizeof(T) / 16 + 1);

        for (size_t i = 0; i < sizeof(T); ++i) {
            char hex[4];
            snprintf(hex, sizeof(hex), "%02X ", bytes[i]);
            out += hex;
            if ((i + 1) % 16 == 0) out += '\n';
        }
        if (sizeof(T) % 16) out += '\n';

        OutputDebugStringA(out.c_str());
    }
}

#endif //VERTIX_MEMORYHELPER_H
