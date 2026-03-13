//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_POSITIONCONTROLLER_H
#define VERTIX_POSITIONCONTROLLER_H

#include <vector>
#include <DirectXTK12/SimpleMath.h>

#include "GameObject3D.h"
#include "IControllerInput.hpp"

namespace Vertix::Engine {
    class DefaultPositionController {
    public:
        explicit DefaultPositionController(IControllerInput<DirectX::SimpleMath::Vector3>* controllerInput) : controllerInput(controllerInput) {
            controllerInput->OnUpdate(std::bind(&DefaultPositionController::OnUpdate, this, std::placeholders::_1));
        }

        void AttachObject(GameObject3D* object) {
            gameObjects.push_back(object);
        }

        float Speed = 1.0f;
        bool MultiplyDeltaTime = true;

    private:
        IControllerInput<DirectX::SimpleMath::Vector3>* controllerInput;
        std::vector<GameObject3D*> gameObjects;

        void OnUpdate(double deltaTime) const {
            const DirectX::SimpleMath::Vector3 value = controllerInput->GetValue();
            if (deltaTime <= 0 || value == DirectX::SimpleMath::Vector3::Zero) return;

            deltaTime = std::pow(deltaTime, MultiplyDeltaTime ? 1.0 : 0.0);
            const DirectX::SimpleMath::Vector3 vector = value * Speed * static_cast<float>(deltaTime);

            for (GameObject3D* object : gameObjects) {
                object->Move(vector);
            }
        }
    };
}

#endif //VERTIX_POSITIONCONTROLLER_H