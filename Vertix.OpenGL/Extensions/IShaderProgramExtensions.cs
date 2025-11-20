using Silk.NET.OpenGL;
using System;
using System.IO;
using Vertix.OpenGL.Rendering;
using Vertix.Rendering;

namespace Vertix.OpenGL.Extensions;

public static class IShaderProgramExtensions
{
    extension(IShaderProgram shaderProgram)
    {
        public void LoadGLSLShadersFromFiles((ShaderType, string)[] shaders)
        {
            if (shaderProgram is not GLShaderProgram gLShaderProgram)
                throw new InvalidOperationException("");

            gLShaderProgram.GLShaders.Clear();

            foreach (var shader in shaders)
            {
                gLShaderProgram.GLShaders.Add(new GLShader
                {
                    GLSL = File.ReadAllText(shader.Item2),
                    ShaderType = shader.Item1
                });
            }
        }
    }
}
