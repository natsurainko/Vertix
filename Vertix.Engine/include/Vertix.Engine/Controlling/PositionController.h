//
// Created by Natsurainko on 2026/1/27.
//

#pragma once

#include <vector>
#include <DirectXTK12/SimpleMath.h>

#include "IControllerInput.h"
#include "Vertix.Engine/GameObject3D.h"

namespace Vertix::Engine {
    class PositionController : public IControllerInputListener<DirectX::SimpleMath::Vector3> {
        std::vector<GameObject3D*> gameObjects;

    public:
        float Speed             = 1.0f;
        bool  MultiplyDeltaTime = true;

        void AttachObject(GameObject3D* object) {
            gameObjects.push_back(object);
        }

    private:
        void OnInputUpdated(const DirectX::SimpleMath::Vector3 &value, double deltaTime) override {
            if (deltaTime <= 0 || value == DirectX::SimpleMath::Vector3::Zero) return;

            deltaTime                                 = std::pow(deltaTime, MultiplyDeltaTime ? 1.0 : 0.0);
            const DirectX::SimpleMath::Vector3 vector = value * Speed * static_cast<float>(deltaTime);

            for (GameObject3D* object : gameObjects) {
                object->Move(vector);
            }
        }
    };
}
