using Silk.NET.Core.Native;
using Silk.NET.Direct3D11;
using Vertix.Graphics;

namespace Vertix.Direct3D11.Graphics;

public abstract class D3D11Texture(ComPtr<ID3D11Device5> device, ComPtr<ID3D11DeviceContext4> context) : ITexture
{
    protected readonly ComPtr<ID3D11Device5> _device = device;
    protected readonly ComPtr<ID3D11DeviceContext4> _context = context;

    internal ComPtr<ID3D11Resource> _textureResource;

    public bool Initialized { get; protected set; }

    public TextureFormat TextureFormat { get; protected set; }

    public uint MipmapLevels { get; protected set; }

    public abstract void Dispose();
}
