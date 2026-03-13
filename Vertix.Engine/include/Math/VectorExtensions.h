//
// Created by Natsurainko on 2026/2/12.
//

#ifndef VERTIX_VECTOREXTENSIONS_H
#define VERTIX_VECTOREXTENSIONS_H

#include <DirectXTK12/SimpleMath.h>

namespace Vertix::Engine {
    inline void FillVector4(DirectX::SimpleMath::Vector4 &ref, const DirectX::SimpleMath::Vector3 &vector3) noexcept {
        ref.x = vector3.x;
        ref.y = vector3.y;
        ref.z = vector3.z;
        ref.w = 0.f;
    }

    inline void FillVector4(DirectX::SimpleMath::Vector4 &ref, const DirectX::SimpleMath::Vector2 &vector2) noexcept {
        ref.x = vector2.x;
        ref.y = vector2.y;
        ref.z = 0.f;
        ref.w = 0.f;
    }

    inline DirectX::SimpleMath::Vector4 PaddingVector3(const DirectX::SimpleMath::Vector3 &vector3) noexcept {
        return {vector3.x, vector3.y, vector3.z, 0.f};
    }

    inline DirectX::SimpleMath::Vector4 PaddingVector2(const DirectX::SimpleMath::Vector2 &vector2) noexcept {
        return {vector2.x, vector2.y, 0.f, 0.f};
    }
}

#endif //VERTIX_VECTOREXTENSIONS_H