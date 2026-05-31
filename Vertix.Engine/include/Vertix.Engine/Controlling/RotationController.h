//
// Created by Natsurainko on 2026/2/3.
//

#pragma once

#include <vector>
#include <DirectXTK12/SimpleMath.h>

#include "IControllerInput.h"
#include "Vertix.Engine/GameObject3D.h"

namespace Vertix::Engine {
    class RotationController : IControllerInputListener<DirectX::SimpleMath::Vector3> {
        std::vector<GameObject3D*> gameObjects;

    public:
        float Sensitivity = 0.001f;

        void AttachObject(GameObject3D* object) {
            gameObjects.push_back(object);
        }

    private:
        void OnInputUpdated(const DirectX::SimpleMath::Vector3 &value, const double deltaTime) override {
            if (deltaTime <= 0 || value == DirectX::SimpleMath::Vector3::Zero) return;
            const DirectX::SimpleMath::Vector3 vector = value * Sensitivity;
            for (GameObject3D* object : gameObjects) object->Rotate(vector);
        }
    };
}
