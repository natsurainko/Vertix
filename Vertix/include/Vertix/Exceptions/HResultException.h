//
// Created by Natsurainko on 2025/12/23.
//

#pragma once

#include <intsafe.h>
#include <source_location>
#include <stdexcept>
#include <string>

namespace Vertix {
    class HResultException : public std::runtime_error {
        const HRESULT              hr;
        const std::source_location location;

    public:
        explicit HResultException(
            const HRESULT               hr,
            const std::source_location &loc = std::source_location::current())
        : std::runtime_error(FormatHResultMessage(hr, loc)),
          hr(hr),
          location(loc) {}

        [[nodiscard]] HRESULT                     GetHResult() const noexcept { return hr; }
        [[nodiscard]] const std::source_location& GetLocation() const noexcept { return location; }

    private:
        [[nodiscard]]
        static std::string FormatHResultMessage(const HRESULT hr, const std::source_location &loc) {
            char buf[256] = {};
            sprintf_s(
                buf,
                "[%s:%u] HResult: 0x%08X",
                loc.file_name(),
                loc.line(),
                static_cast<UINT>(hr)
            );
            return buf;
        }
    };

    inline void ThrowIfFailed(
        const HRESULT               hr,
        const std::source_location &loc = std::source_location::current()) {
        if (FAILED(hr)) throw HResultException(hr, loc);
    }
}
