//
// Created by Natsurainko on 2026/4/17.
//

#ifndef VERTIX_CASCADESHADOWMAPPING_H
#define VERTIX_CASCADESHADOWMAPPING_H

#include <DirectXTK12/SimpleMath.h>

#include "Vertix.Engine/Helpers/MathHelper.h"

namespace Vertix::Engine {
    struct CascadeData {
        DirectX::SimpleMath::Matrix LightViewProjection;

        float PlaneDistance;
        float ShadowMapTexelSize;
        float RadiusScale;
        float ZStartBiasScale;

        DirectX::SimpleMath::Vector4 DepthConvertToView;
    };

    // Ref: https://qiutang98.github.io/post/%E5%9B%BE%E5%BD%A2%E7%A1%AC%E4%BB%B6api/%E5%A4%A7%E5%9E%8B%E5%9C%BA%E6%99%AF%E4%B8%8B%E7%9A%84vulkan-cascadeshadowmap%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/
    inline void SpiltCascades(
        const uint32_t cascadesNum,
        const float nearPlane,
        const float farPlane,
        std::vector<float> &cascadeSplits,
        const float splitLambda = 0.95f)
    {
        cascadeSplits.clear();
        cascadeSplits.reserve(cascadesNum);

        const float clipRange = farPlane - nearPlane;
        const float minZ  = nearPlane;
        const float maxZ  = nearPlane + clipRange;
        const float range = maxZ - minZ;
        const float ratio = maxZ / minZ;

        // Ref: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
        for (uint32_t i = 0; i < cascadesNum; ++i) {
            const float p       = static_cast<float>(i + 1) / static_cast<float>(cascadesNum);
            const float log     = minZ * std::pow(ratio, p);
            const float uniform = minZ + range * p;
            const float d       = splitLambda * (log - uniform) + uniform;

            cascadeSplits.emplace_back((d - nearPlane) / clipRange);
        }
    }

    // Ref: https://qiutang98.github.io/post/%E5%9B%BE%E5%BD%A2%E7%A1%AC%E4%BB%B6api/%E5%A4%A7%E5%9E%8B%E5%9C%BA%E6%99%AF%E4%B8%8B%E7%9A%84vulkan-cascadeshadowmap%E5%8A%9F%E8%83%BD%E5%BC%80%E5%8F%91/
    template<uint32_t cascadesNum>
    void SetupCascades(
        CascadeData cascadeDatas[cascadesNum],
        const DirectX::SimpleMath::Matrix &cameraViewProjection,
        const float nearPlane,
        const float farPlane,
        const DirectX::SimpleMath::Vector3 lightDirection,
        const float shadowMapSize,
        const float splitLambda = 0.90f,
        const float radiusScaleFixed = 10.0f)
    {
        // STEP 1: Calculate camera frustum corners
        DirectX::SimpleMath::Vector3 frustumCorners[8] = {};
        CalculateCameraFrustumCorners(cameraViewProjection, frustumCorners);

        DirectX::SimpleMath::Vector3 frustumRays[4] = {};
        for (uint32_t i = 0; i < 4; ++i) {
            frustumRays[i] = frustumCorners[i + 4] - frustumCorners[i];
        }

        // STEP 2: Calculate cascade spilt distance
        std::vector<float> cascadeSplits;
        SpiltCascades(cascadesNum, nearPlane, farPlane, cascadeSplits, splitLambda);

        // STEP 3: Calculate cascade light view projection matrix
        float nearSplit = .0f;
        float clipRange = farPlane - nearPlane;
        const float shadowMapTexelSize = 1.0f / shadowMapSize;

        for (uint32_t i = 0; i < cascadesNum; ++i) {
            const float farSplit = cascadeSplits[i];

            DirectX::SimpleMath::Vector3 clippedFrustumCorners[8] = {};
            for (uint32_t j = 0; j < 4; ++j) {
                const auto cornerRay = frustumRays[j];

                const auto nearCornerRay = cornerRay * nearSplit;
                const auto farCornerRay  = cornerRay * farSplit;

                clippedFrustumCorners[j]     = frustumCorners[j] + nearCornerRay;
                clippedFrustumCorners[j + 4] = frustumCorners[j] + farCornerRay;
            }

            const DirectX::SimpleMath::Vector3 frustumCenter = CalculateFrustumCenter(clippedFrustumCorners);

            float radius = 0.0f;
            for (const auto &corner : clippedFrustumCorners) {
                float distance = (corner - frustumCenter).Length();
                radius = (std::max)(radius, distance);
            }
            radius = std::ceil(radius * 16.0f) / 16.0f;

            const auto lightViewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(
                frustumCenter - lightDirection * radius,
                frustumCenter,
                DirectX::SimpleMath::Vector3::Up
            );

            DirectX::SimpleMath::Matrix lightOrthoMatrix;
            {
                lightOrthoMatrix = DirectX::SimpleMath::Matrix::CreateOrthographicOffCenter(
                    -radius, radius,
                    -radius, radius,
                    0.0f,
                    radius * 2.0f
                );

                DirectX::SimpleMath::Vector4 shadowOrigin = DirectX::SimpleMath::Vector4::UnitW;
                shadowOrigin = DirectX::SimpleMath::Vector4::Transform(shadowOrigin, lightViewMatrix * lightOrthoMatrix);
                shadowOrigin *= shadowMapSize * 0.5f;

                const DirectX::SimpleMath::Vector4 roundOrign {
                    std::round(shadowOrigin.x),
                    std::round(shadowOrigin.y),
                    std::round(shadowOrigin.z),
                    std::round(shadowOrigin.w)
                };

                DirectX::SimpleMath::Vector4 roundOffset = (roundOrign - shadowOrigin) * 2.0f * shadowMapTexelSize;

                lightOrthoMatrix._41 += roundOffset.x;
                lightOrthoMatrix._42 += roundOffset.y;
            }

            float radiusScale = 10.0f * radiusScaleFixed / radius;
            radiusScale /= radiusScale + 1.0f;

            auto &cascadeData = cascadeDatas[i];
            cascadeData.LightViewProjection = lightViewMatrix * lightOrthoMatrix;
            cascadeData.DepthConvertToView  = DirectX::SimpleMath::Vector4 { lightOrthoMatrix._33, lightOrthoMatrix._43, 0, 0 };
            cascadeData.PlaneDistance       = nearPlane + farSplit * clipRange;
            cascadeData.ShadowMapTexelSize  = shadowMapTexelSize;
            cascadeData.RadiusScale         = radiusScale;
            cascadeData.ZStartBiasScale     = 0.25f;

            nearSplit = farSplit;
        }
    }
}

#endif //VERTIX_CASCADESHADOWMAPPING_H
