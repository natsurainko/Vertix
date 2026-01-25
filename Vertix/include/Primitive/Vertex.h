//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_VERTEX_H
#define VERTIX_VERTEX_H

#include <directXTK/simpleMath/SimpleMath.h>

namespace Vertix {
    struct Vertex {
        DirectX::SimpleMath::Vector3 Position;
        DirectX::SimpleMath::Vector3 Normal;
        DirectX::SimpleMath::Vector2 TexCoord;
        DirectX::SimpleMath::Vector3 Tangent;
        DirectX::SimpleMath::Vector3 Bitangent;
    };
}

#endif //VERTIX_VERTEX_H