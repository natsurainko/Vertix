//
// Created by Natsurainko on 2025/12/28.
//

#include "Camera/PerspectiveCamera.h"

#include "Math/FloatExtensions.h"

Vertix::Engine::PerspectiveCamera::PerspectiveCamera(const float aspect, const float fov, const float nearPlane, const float farPlane)
    : aspect(aspect) , fov(fov) , nearPlane(nearPlane) , farPlane(farPlane) {
    projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

    forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, orientation);
    right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, orientation);
    up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, orientation);
}

Vertix::Engine::PerspectiveCamera::PerspectiveCamera()
    : aspect(4.0 / 3.0) , fov(DegreesToRadians(45)) , nearPlane(0.1f) , farPlane(100) {
    projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

    forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, orientation);
    right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, orientation);
    up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, orientation);
}

void Vertix::Engine::PerspectiveCamera::Move(const DirectX::SimpleMath::Vector3 &offset, const bool relative, const bool allowRoll) {
    if (!relative) {
        position = position + offset;
        return;
    }

    position += forward * offset.x;
    position += allowRoll ? up : DirectX::SimpleMath::Vector3::Up * offset.y;
    position += right * offset.z;
}

void Vertix::Engine::PerspectiveCamera::Rotate(const DirectX::SimpleMath::Vector3 &angles, const bool allowRoll) {
    GameObject3D::Rotate(angles, allowRoll);
    forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, orientation);
    right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, orientation);
    up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, orientation);
}