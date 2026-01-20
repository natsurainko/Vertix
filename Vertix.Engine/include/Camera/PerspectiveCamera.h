//
// Created by Natsurainko on 2025/12/28.
//

#ifndef VERTIX_PERSPECTIVECAMERA_H
#define VERTIX_PERSPECTIVECAMERA_H

#include "GameObject3D.h"

namespace Vertix::Engine {
    class PerspectiveCamera : public GameObject3D {
    public:
        PerspectiveCamera(float aspect, float fov, float nearPlane, float farPlane);
        PerspectiveCamera();

        void Move(const DirectX::SimpleMath::Vector3 &offset, bool relative = true, bool allowRoll = false);
        void Rotate(const DirectX::SimpleMath::Vector3 &angles, bool allowRoll = false);

        void GetViewMatrix(DirectX::SimpleMath::Matrix& view) const {
            view = DirectX::SimpleMath::Matrix::CreateLookAt(position, position + forward, up);
        }

        void GetProjectionMatrix(DirectX::SimpleMath::Matrix& projection) const {
            projection = projectionMatrix;
        }

        void SetAspect(const float newAspect) {
            aspect = newAspect;
            projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);
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