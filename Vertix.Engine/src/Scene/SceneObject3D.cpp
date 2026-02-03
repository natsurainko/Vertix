//
// Created by Natsurainko on 2026/1/28.
//

#include "Scene/SceneObject3D.h"

#include "Content/ModelImporter.h"

Vertix::Engine::SceneObject3D::~SceneObject3D() {
    delete sceneModel;
}

void Vertix::Engine::SceneObject3D::Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const {
    if (sceneModel) {
        sceneModel->Draw(commandList);
    }
}

bool Vertix::Engine::SceneObject3D::TryLoadModelFromFile(const std::string &filePath) {
    const auto model = new Model();
    if (ModelImporter::TryLoadFromFile(*model, filePath)) {
        sceneModel = model;
        return true;
    }

    delete model;
    return false;
}

void Vertix::Engine::SceneObject3D::UploadModelToGPU(const GraphicsDevice* graphicsDevice,
                                                     const GraphicsCommandList* graphicsCommandList,
                                                     TempGraphicsResourceHeap<Microsoft::WRL::ComPtr<ID3D12Resource>> &tempResourceHeap) const {
    if (!sceneModel) return;
    sceneModel->UploadToGPU(graphicsDevice, graphicsCommandList, tempResourceHeap);
}
