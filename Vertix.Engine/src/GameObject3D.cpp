//
// Created by Natsurainko on 2025/12/29.
//

#include "Vertix.Engine/GameObject3D.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

void Vertix::Engine::GameObject3D::Move(
    const Vector3 &offset,
    const bool     relative,
    const bool     allowRoll) noexcept {
    if (!relative) {
        position = position + offset;
        return;
    }

    const auto forward = Vector3::Transform(Vector3::Forward, orientation);
    const auto right   = Vector3::Transform(Vector3::Right, orientation);

    position += forward * offset.x;
    position += allowRoll
                    ? Vector3::Transform(Vector3::Up, orientation)
                    : Vector3::Up * offset.y;
    position += right * offset.z;
}

void Vertix::Engine::GameObject3D::Rotate(
    const Vector3 &angles,
    const bool     allowRoll) noexcept {
    orientation = orientation * DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
        allowRoll
            ? Vector3::Transform(Vector3::Up, orientation)
            : Vector3::Up,
        angles.y
    );
    orientation = orientation * DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
        Vector3::Transform(Vector3::Right, orientation),
        angles.x
    );
    orientation = orientation * DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
        Vector3::Transform(Vector3::Forward, orientation),
        angles.z
    );
}
