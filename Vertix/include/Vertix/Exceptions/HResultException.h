//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_HRESULTEXCEPTION_H
#define VERTIX_HRESULTEXCEPTION_H

#include <intsafe.h>
#include <stdexcept>
#include <string>

inline std::string HResultToString(const HRESULT hr) {
    char s_str[64] = {};
    sprintf_s(s_str, "HResult: 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

class HResultException : public std::runtime_error {
public:
    explicit HResultException(const HRESULT hr) : std::runtime_error(HResultToString(hr)), hr(hr) {}

    [[nodiscard]]
    HRESULT GetHResult() const noexcept {
        return hr;
    }
private:
    const HRESULT hr;
};

inline void ThrowIfFailed(const HRESULT hr) {
    if (FAILED(hr)) {
        throw HResultException(hr);
    }
}

#endif //VERTIX_HRESULTEXCEPTION_H