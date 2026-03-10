//
// Created by Natsurainko on 2026/1/28.
//

#ifndef VERTIX_SCENEOBJECT3D_H
#define VERTIX_SCENEOBJECT3D_H

#include "GameObject3D.h"
#include "Primitive/Model.h"

namespace Vertix::Engine {
    class SceneObject3D : public GameObject3D {
    public:
        void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const override {
            if (SceneModel) {
                SceneModel->Draw(commandList);
            }
        }

        void DrawInstanced(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList, const UINT instanceCount) const override {
            if (SceneModel) {
                SceneModel->DrawInstanced(commandList, instanceCount);
            }
        }

        Model* SceneModel = nullptr;
    };
}

#endif //VERTIX_SCENEOBJECT3D_H