using System;
using System.Collections.Generic;

namespace Vertix.Rendering;

public partial interface IShaderProgram : IDisposable
{
    bool IsCompiled { get;}

    Dictionary<string, IShaderParameter> Parameters { get; } 

    void Compile();

    void Use();
}

public partial interface IShaderProgram
{
    public static IShaderProgram? DefaultShader2D { get; set; }

    public static IShaderProgram? DefaultShader3D { get; set; }
}
