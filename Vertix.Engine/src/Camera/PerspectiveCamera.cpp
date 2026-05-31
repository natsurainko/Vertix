//
// Created by Natsurainko on 2025/12/28.
//

#include "Vertix.Engine/Camera/PerspectiveCamera.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

Vertix::Engine::PerspectiveCamera::PerspectiveCamera(
    const float aspect,
    const float fov,
    const float nearPlane,
    const float farPlane)
: aspect(aspect),
  fov(fov),
  nearPlane(nearPlane),
  farPlane(farPlane) {
    projectionMatrix = Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

    forward = Vector3::Transform(Vector3::Forward, orientation);
    right   = Vector3::Transform(Vector3::Right, orientation);
    up      = Vector3::Transform(Vector3::Up, orientation);
}

Vertix::Engine::PerspectiveCamera::PerspectiveCamera()
: aspect(4.0f / 3.0f),
  fov(DegreesToRadians(45.0f)),
  nearPlane(0.1f),
  farPlane(500.0f) {
    projectionMatrix = Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearPlane, farPlane);

    forward = Vector3::Transform(Vector3::Forward, orientation);
    right   = Vector3::Transform(Vector3::Right, orientation);
    up      = Vector3::Transform(Vector3::Up, orientation);
}

void Vertix::Engine::PerspectiveCamera::Move(
    const Vector3 &offset,
    const bool     relative,
    const bool     allowRoll) noexcept {
    if (!relative) {
        position = position + offset;
        return;
    }

    position += forward * offset.x;
    position += allowRoll ? up : Vector3::Up * offset.y;
    position += right * offset.z;
}

void Vertix::Engine::PerspectiveCamera::Rotate(
    const Vector3 &angles,
    const bool     allowRoll) noexcept {
    GameObject3D::Rotate(angles, allowRoll);
    forward = Vector3::Transform(Vector3::Forward, orientation);
    right   = Vector3::Transform(Vector3::Right, orientation);
    up      = Vector3::Transform(Vector3::Up, orientation);
}
