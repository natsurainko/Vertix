using Silk.NET.OpenGL;
using System;

namespace Vertix.OpenGL.Graphics;

public partial class GLGraphicsDevice : IDisposable
{
    public GL GL { get; }

    internal GLGraphicsDevice(GL gL) => GL = gL;

    public void Dispose()
    {
        GL.Dispose();
    }
}