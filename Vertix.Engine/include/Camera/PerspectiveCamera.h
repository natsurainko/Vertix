//
// Created by Natsurainko on 2025/12/28.
//

#ifndef VERTIX_PERSPECTIVECAMERA_H
#define VERTIX_PERSPECTIVECAMERA_H

#include "GameObject3D.h"
#include "VERTIX_ENGINE_EXPORT.h"

namespace Vertix::Engine {
    class VERTIX_ENGINE_API PerspectiveCamera : public GameObject3D {
    public:
        PerspectiveCamera(float aspect, float fov, float nearPlane, float farPlane);
        PerspectiveCamera();

        void Move(
            const DirectX::SimpleMath::Vector3 &offset,
            bool relative = true,
            bool allowRoll = false) override;

        void Rotate(
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