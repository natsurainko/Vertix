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