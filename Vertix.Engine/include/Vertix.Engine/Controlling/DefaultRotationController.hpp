//
// Created by Natsurainko on 2026/2/3.
//

#ifndef VERTIX_DEFAULTROTATIONCONTROLLER_H
#define VERTIX_DEFAULTROTATIONCONTROLLER_H

#include <DirectXTK12/SimpleMath.h>

#include "Vertix.Engine/GameObject3D.h"
#include "Vertix.Engine/Controlling/IControllerInput.hpp"

namespace Vertix::Engine {
    class DefaultRotationController {
    public:
        explicit DefaultRotationController(IControllerInput<DirectX::SimpleMath::Vector3>* controllerInput) : controllerInput(controllerInput) {
            controllerInput->OnUpdate(std::bind(&DefaultRotationController::OnUpdate, this, std::placeholders::_1));
        }

        void AttachObject(GameObject3D* object) {
            gameObjects.push_back(object);
        }

        float Sensitivity = 0.001f;

    private:
        IControllerInput<DirectX::SimpleMath::Vector3>* controllerInput;
        std::vector<GameObject3D*> gameObjects;

        void OnUpdate(const double deltaTime) const {
            const DirectX::SimpleMath::Vector3 &value = controllerInput->GetValue();
            if (deltaTime <= 0 || value == DirectX::SimpleMath::Vector3::Zero) return;

            const DirectX::SimpleMath::Vector3 vector = value * Sensitivity;

            for (GameObject3D* object : gameObjects) {
                object->Rotate(vector);
            }
        }
    };
}

#endif //VERTIX_DEFAULTROTATIONCONTROLLER_H
