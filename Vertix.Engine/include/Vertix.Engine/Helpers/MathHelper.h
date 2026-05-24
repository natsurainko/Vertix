//
// Created by Natsurainko on 2025/12/28.
//

#ifndef VERTIX_FLOATEXTENSIONS_H
#define VERTIX_FLOATEXTENSIONS_H

#include <numbers>
#include <DirectXTK12/SimpleMath.h>

namespace Vertix::Engine {
    inline float DegreesToRadians(const float degrees) noexcept {
        return degrees * std::numbers::pi_v<float> / 180.0f;
    }

    inline void ExtractFrustumPlanes(
        const DirectX::SimpleMath::Matrix &viewProjection,
        DirectX::SimpleMath::Vector4 planes[6])
    {
        const auto c0 = DirectX::SimpleMath::Vector4(viewProjection._11, viewProjection._21, viewProjection._31, viewProjection._41);
        const auto c1 = DirectX::SimpleMath::Vector4(viewProjection._12, viewProjection._22, viewProjection._32, viewProjection._42);
        const auto c2 = DirectX::SimpleMath::Vector4(viewProjection._13, viewProjection._23, viewProjection._33, viewProjection._43);
        const auto c3 = DirectX::SimpleMath::Vector4(viewProjection._14, viewProjection._24, viewProjection._34, viewProjection._44);

        planes[0] = c3 + c0;  // Left
        planes[1] = c3 - c0;  // Right
        planes[2] = c3 + c1;  // Bottom
        planes[3] = c3 - c1;  // Top
        planes[4] = c2;       // Near
        planes[5] = c3 - c2;  // Far

        for (int i = 0; i < 6; ++i) {
            const float len = std::sqrt(
                planes[i].x * planes[i].x +
                planes[i].y * planes[i].y +
                planes[i].z * planes[i].z);
            planes[i] /= len;
        }
    }

    inline void CalculateCameraFrustumCorners(
        const DirectX::SimpleMath::Matrix &cameraViewProjection,
        DirectX::SimpleMath::Vector3 frustumCorners[8])
    {
        DirectX::SimpleMath::Matrix viewProjectionInverse;
        cameraViewProjection.Invert(viewProjectionInverse);

        frustumCorners[0] = DirectX::SimpleMath::Vector3 { -1.0f,  1.0f, -1.0f };
        frustumCorners[1] = DirectX::SimpleMath::Vector3 {  1.0f,  1.0f, -1.0f };
        frustumCorners[2] = DirectX::SimpleMath::Vector3 {  1.0f, -1.0f, -1.0f };
        frustumCorners[3] = DirectX::SimpleMath::Vector3 { -1.0f, -1.0f, -1.0f };
        frustumCorners[4] = DirectX::SimpleMath::Vector3 { -1.0f,  1.0f,  1.0f };
        frustumCorners[5] = DirectX::SimpleMath::Vector3 {  1.0f,  1.0f,  1.0f };
        frustumCorners[6] = DirectX::SimpleMath::Vector3 {  1.0f, -1.0f,  1.0f };
        frustumCorners[7] = DirectX::SimpleMath::Vector3 { -1.0f, -1.0f,  1.0f };

        for (uint32_t i = 0; i < 8; ++i) {
            auto &frustumCorner = frustumCorners[i];
            auto inverseCorner = DirectX::SimpleMath::Vector4::Transform({
                frustumCorner.x,
                frustumCorner.y,
                frustumCorner.z,
                1.0
            }, viewProjectionInverse);
            inverseCorner /= inverseCorner.w;
            frustumCorner = DirectX::SimpleMath::Vector3 { inverseCorner.x, inverseCorner.y, inverseCorner.z };
        }
    }

    inline DirectX::SimpleMath::Vector3 CalculateFrustumCenter(const DirectX::SimpleMath::Vector3 frustumCorners[8]) {
        auto frustumSum = DirectX::SimpleMath::Vector3::Zero;
        for (uint32_t i = 0; i < 8; ++i) {
            frustumSum += frustumCorners[i];
        }

        return frustumSum / 8.0f;
    }
}

#endif //VERTIX_FLOATEXTENSIONS_H
