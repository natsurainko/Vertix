using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;
using Vertix.Rendering;

namespace Vertix.OpenGL.Graphics;

public partial class GLGraphicsDevice : IDisposable
{
    private IShaderProgram? _currentShaderProgram = null;
    private IRenderTarget? _currentRenderTarget = null;

    public GL GL { get; }

    public bool Disposed { get; private set; }

    internal GLGraphicsDevice(GL gL)
    {
        GL = gL;

        EnableFaceCulling = false;
        EnableDepthTest = false;

        CullFace = CullFaceMode.BackFace;
        FaceWindingOrder = FaceWindingOrder.CounterClockwise;
    }

    public void Dispose()
    {
        GL.Dispose();
        Disposed = true;
    }
}