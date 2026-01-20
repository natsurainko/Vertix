//
// Created by Natsurainko on 2025/12/29.
//

#include "GameObject3D.h"

void Vertix::Engine::GameObject3D::Move(const DirectX::SimpleMath::Vector3 &offset, const bool relative, const bool allowRoll) {
    if (!relative) {
        position = position + offset;
        return;
    }

    const DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, orientation);
    const DirectX::SimpleMath::Vector3 right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, orientation);

    position += forward * offset.x;
    position += allowRoll ? DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, orientation) : DirectX::SimpleMath::Vector3::Up * offset.y;
    position += right * offset.z;
}

void Vertix::Engine::GameObject3D::Rotate(const DirectX::SimpleMath::Vector3 &angles, const bool allowRoll) {
    /*orientation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(allowRoll
        ? DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, orientation)
        : DirectX::SimpleMath::Vector3::Up, angles.y) * orientation;
    orientation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
        DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, orientation), angles.x) * orientation;
    orientation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
        DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, orientation), angles.z) * orientation;*/

    orientation = orientation * DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(allowRoll
        ? DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Up, orientation)
        : DirectX::SimpleMath::Vector3::Up, angles.y);
    orientation = orientation * DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
        DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Right, orientation), angles.x);
    orientation = orientation * DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
        DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Forward, orientation), angles.z);
}
