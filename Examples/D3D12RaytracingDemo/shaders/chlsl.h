#ifndef CHLSL_H
#define CHLSL_H

#ifdef __cplusplus

#include <DirectXTK12/SimpleMath.h>

#define IDENTITY = DirectX::SimpleMath::Matrix::Identity
#define ZERO     = 0
#define ONE      = 1.0f

using float4x4 = DirectX::SimpleMath::Matrix;
using float4   = DirectX::SimpleMath::Vector4;
using float3   = DirectX::SimpleMath::Vector3;
using float2   = DirectX::SimpleMath::Vector2;
using uint     = std::uint32_t;

#else

#define IDENTITY
#define ZERO
#define ONE

#endif
#endif // CHLSL_H