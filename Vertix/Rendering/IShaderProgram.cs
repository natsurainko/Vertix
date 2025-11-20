using System;
using System.Collections.Generic;

namespace Vertix.Rendering;

public interface IShaderProgram : IDisposable
{
    bool IsCompiled { get;}

    Dictionary<string, IShaderParameter> Parameters { get; } 

    void Compile();

    void Use();
}
