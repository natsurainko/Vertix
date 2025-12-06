using Silk.NET.OpenGL;
using System;
using Vertix.Rendering;

namespace Vertix.OpenGL.Graphics;

public partial class GLGraphicsDevice : IDisposable
{
    private IShaderProgram? _currentShaderProgram = null;
    private IRenderTarget? _currentRenderTarget = null;

    public GL GL { get; }

    public bool Disposed { get; private set; }

    internal GLGraphicsDevice(GL gL) => GL = gL;

    public void Dispose()
    {
        GL.Dispose();
        Disposed = true;
    }
}