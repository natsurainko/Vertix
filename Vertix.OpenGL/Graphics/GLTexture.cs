using Silk.NET.OpenGL;
using Vertix.Graphics;

namespace Vertix.OpenGL.Graphics;

public abstract class GLTexture(GL gL, uint handle) : ITexture
{
    protected readonly GL _gL = gL;

    public uint Handle { get; protected set; } = handle;

    public bool Initialized { get; protected set; }

    public TextureFormat TextureFormat { get; protected set; }

    public uint MipmapLevels { get; protected set; }

    public virtual void BindTexture(uint bindingIndex) => _gL.BindTextureUnit(bindingIndex, Handle);

    public virtual void Dispose() => _gL.DeleteTexture(Handle);
}