using Silk.NET.OpenGL;
using System;
using System.Collections.Generic;
using Vertix.Rendering;

namespace Vertix.OpenGL.Rendering;

public partial class GLShaderProgram(GL gL) : IShaderProgram
{
    private readonly uint _handle = gL.CreateProgram();

    public uint Handle => _handle;

    public virtual List<GLShader> GLShaders { get; } = [];

    public bool IsCompiled { get; protected set; } = false;

    public Dictionary<string, IShaderParameter> Parameters { get; } = [];

    public void Compile()
    {
        if (IsCompiled) return;

        List<uint> shaderHandles = [];

        foreach (GLShader shader in GLShaders)
        {
            uint shaderHandle = gL.CreateShader(shader.ShaderType);
            gL.ShaderSource(shaderHandle, shader.GLSL);

            gL.CompileShader(shaderHandle);
            if (gL.GetShader(shaderHandle, ShaderParameterName.CompileStatus) != 1)
            {
                string shaderCompilationLog = gL.GetShaderInfoLog(shaderHandle);
                throw new Exception(shaderCompilationLog);
            }

            gL.AttachShader(_handle, shaderHandle);
            shaderHandles.Add(shaderHandle);
        }

        gL.LinkProgram(_handle);
        if (gL.GetProgram(_handle, ProgramPropertyARB.LinkStatus) != 1)
        {
            string shaderProgramLog = gL.GetProgramInfoLog(_handle);
            throw new Exception(shaderProgramLog);
        }

        foreach (uint shaderHandle in shaderHandles)
        {
            gL.DetachShader(_handle, shaderHandle);
            gL.DeleteShader(shaderHandle);
        }

        for (int i = 0; i < gL.GetProgram(_handle, ProgramPropertyARB.ActiveUniforms); i++)
        {
            gL.GetActiveUniform(_handle, (uint)i, 256, out uint length, out int size, out UniformType type, out string name);
            Parameters[name] = new GLShaderParameter(gL, _handle, name, type);
        }

        IsCompiled = true;
    }

    public void Use() => gL.UseProgram(_handle);

    public void Dispose()
    {
        Parameters.Clear();
        gL.DeleteProgram(_handle);
    }
}