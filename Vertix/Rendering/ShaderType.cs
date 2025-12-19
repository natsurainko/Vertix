namespace Vertix.Rendering;

public enum ShaderType
{
    // Common Naming
    VertexShader = 0,
    GeometryShader = 2,
    ComputeShader = 3,

    // OpenGL Naming
    FragmentShader = 1,

    TessellationControlShader = 4,
    TessellationEvaluationShader = 5,

    // D3D11 Naming
    PixelShader = FragmentShader,

    HullShader = TessellationControlShader,
    DomainShader = TessellationEvaluationShader
}
