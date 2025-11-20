using Silk.NET.OpenGL;

namespace Vertix.OpenGL.Rendering;

public struct GLShader
{
    public string GLSL { get; set; }

    public ShaderType ShaderType { get; set; }
}