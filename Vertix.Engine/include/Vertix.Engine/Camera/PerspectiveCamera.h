//
// Created by Natsurainko on 2025/12/28.
//

#ifndef VERTIX_PERSPECTIVECAMERA_H
#define VERTIX_PERSPECTIVECAMERA_H

#include <DirectXTK12/SimpleMath.h>

#include "Vertix.Engine/GameObject3D.h"
#include "Vertix.Engine/VERTIX_ENGINE_EXPORT.h"

namespace Vertix::Engine {
    class PerspectiveCamera : public GameObject3D {
    public:
        VERTIX_ENGINE_API PerspectiveCamera(float aspect, float fov, float nearPlane, float farPlane);
        VERTIX_ENGINE_API PerspectiveCamera();

        VERTIX_ENGINE_API void Move(
            const DirectX::SimpleMath::Vector3 &offset,
            bool relative = true,
            bool allowRoll = false) override;

        VERTIX_ENGINE_API void Rotate(
            const DirectX::SimpleMath::Vector3 &angles,
            bool allowRoll = false) override;

        void GetViewMatrix(DirectX::SimpleMath::Matrix& view) const noexcept {
            view = DirectX::SimpleMath::Matrix::CreateLookAt(position, position + forward, up);
        }

        void GetProjectionMatrix(DirectX::SimpleMath::Matrix& projection) const noexcept {
            projection = projectionMatrix;
        }

        void SetAspect(const float newAspect) noexcept {
            aspect = newAspect;
            projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);
        }

        void SetFieldOfView(const float newFieldOfView) noexcept {
            fov = newFieldOfView;
            projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);
        }

        void SetOrientation(const DirectX::SimpleMath::Quaternion &newOrientation) noexcept override {
            GameObject3D::SetOrientation(newOrientation);
            forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, orientation);
            right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, orientation);
            up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, orientation);
        }

        [[nodiscard]]
        float GetAspect() const noexcept {
            return aspect;
        }

        [[nodiscard]]
        float GetFieldOfView() const noexcept {
            return fov;
        }

        [[nodiscard]]
        float GetNearPlane() const noexcept {
            return nearPlane;
        }

        [[nodiscard]]
        float GetFarPlane() const noexcept {
            return farPlane;
        }

    private:
        float aspect;
        float fov;
        float nearPlane;
        float farPlane;

        DirectX::SimpleMath::Vector3 forward;
        DirectX::SimpleMath::Vector3 right;
        DirectX::SimpleMath::Vector3 up;

        DirectX::SimpleMath::Matrix projectionMatrix;
    };
}


#endif //VERTIX_PERSPECTIVECAMERA_H
