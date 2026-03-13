//
// Created by Natsurainko on 2025/12/28.
//

#ifndef VERTIX_FLOATEXTENSIONS_H
#define VERTIX_FLOATEXTENSIONS_H

#include <numbers>

namespace Vertix::Engine {
    inline float DegreesToRadians(const float degrees) noexcept {
        return degrees * std::numbers::pi_v<float> / 180.0f;
    }
}

#endif //VERTIX_FLOATEXTENSIONS_H