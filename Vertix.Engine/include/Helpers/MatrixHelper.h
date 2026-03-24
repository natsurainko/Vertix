//
// Created by Natsurainko on 2026/1/29.
//

#ifndef VERTIX_MATRIXEXTENSIONS_H
#define VERTIX_MATRIXEXTENSIONS_H

#include <DirectXTK12/SimpleMath.h>

namespace Vertix::Engine {
    inline DirectX::SimpleMath::Matrix CreateLightViewProjectionForFrustum(const DirectX::SimpleMath::Vector3 &lightDirection,
                                                                           const DirectX::SimpleMath::Matrix &viewProjection,
                                                                           const float zMult = 10) {
        DirectX::SimpleMath::Matrix viewProjectionInverse;
        DirectX::SimpleMath::Vector3 center = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector4 vector4s[8] = {};

        viewProjection.Invert(viewProjectionInverse);

        int index = 0;
        for (int x = 0; x < 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                for (int z = 0; z < 2; ++z) {
                    auto pt = DirectX::SimpleMath::Vector4::Transform({
                        2.0f * static_cast<float>(x) - 1.0f,
                        2.0f * static_cast<float>(y) - 1.0f,
                        static_cast<float>(z),
                        1.0f
                    }, viewProjectionInverse);

                    pt /= pt.w;
                    center += {pt.x, pt.y, pt.z};
                    vector4s[index++] = pt;
                }
            }
        }

        center /= 8.0f;

        DirectX::SimpleMath::Vector3 upVector = DirectX::SimpleMath::Vector3::Up;
        if (std::abs(lightDirection.Dot(upVector)) > 0.999f) {
            upVector = DirectX::SimpleMath::Vector3::Forward;
        }

        const auto lightViewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(center, center + lightDirection, upVector);

        float minX = (std::numeric_limits<float>::max)();
        float minY = (std::numeric_limits<float>::max)();
        float minZ = (std::numeric_limits<float>::max)();

        float maxX = (std::numeric_limits<float>::lowest)();
        float maxY = (std::numeric_limits<float>::lowest)();
        float maxZ = (std::numeric_limits<float>::lowest)();

        for (const auto &vector4 : vector4s) {
            DirectX::SimpleMath::Vector4 transform = DirectX::SimpleMath::Vector4::Transform(vector4, lightViewMatrix);
            minX = (std::min)(minX, transform.x);
            minY = (std::min)(minY, transform.y);
            minZ = (std::min)(minZ, transform.z);

            maxX = (std::max)(maxX, transform.x);
            maxY = (std::max)(maxY, transform.y);
            maxZ = (std::max)(maxZ, transform.z);
        }

        if (minZ < 0)
            minZ *= zMult;
        else
            minZ /= zMult;

        if (maxZ < 0)
            maxZ /= zMult;
        else
            maxZ *= zMult;

        return lightViewMatrix * DirectX::SimpleMath::Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
    }
}

#endif //VERTIX_MATRIXEXTENSIONS_H