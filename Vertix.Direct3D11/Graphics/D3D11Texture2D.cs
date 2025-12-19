using Silk.NET.Core.Native;
using Silk.NET.Direct3D11;
using Silk.NET.Maths;
using System;
using Vertix.Direct3D11.Helpers;
using Vertix.Graphics;

namespace Vertix.Direct3D11.Graphics;

public unsafe class D3D11Texture2D : D3D11Texture, ITexture2D
{
    internal Texture2DDesc _texture2DDesc;
    internal ComPtr<ID3D11Texture2D> _texture2D;

    public Vector2D<uint> Size { get; private set; }

    public D3D11Texture2D(D3D11GraphicsDevice d3D11GraphicsDevice) : base(d3D11GraphicsDevice.Device, d3D11GraphicsDevice.DeviceContext) { }

    public D3D11Texture2D(D3D11GraphicsDevice d3D11GraphicsDevice, ComPtr<ID3D11Texture2D> texturePtr) : base(d3D11GraphicsDevice.Device, d3D11GraphicsDevice.DeviceContext) 
    {
        _texture2D = texturePtr;
        _texture2D.GetDesc(ref _texture2DDesc);

        Size = new Vector2D<uint>(_texture2DDesc.Width, _texture2DDesc.Height);
        TextureFormat = _texture2DDesc.Format.ToTextureFormat();
        MipmapLevels = _texture2DDesc.MipLevels;

        _textureResource = ComPtr.Downcast<ID3D11Texture2D, ID3D11Resource>(_texture2D);
        Initialized = true;
    }

    public void Initialize(Vector2D<uint> size, TextureFormat format, uint mipmapLevels = 1)
    {
        Size = size;
        TextureFormat = format;
        MipmapLevels = mipmapLevels;

        _texture2DDesc.Width = size.X;
        _texture2DDesc.Height = size.Y;
        _texture2DDesc.MipLevels = mipmapLevels;
        _texture2DDesc.ArraySize = 1;
        _texture2DDesc.SampleDesc.Count = 1;
        _texture2DDesc.SampleDesc.Quality = 0;
        _texture2DDesc.Format = format.ToDXGIFormat();
        _texture2DDesc.Usage = Usage.Default;

        if (format == TextureFormat.Depth16 ||
            format == TextureFormat.Depth32f ||
            format == TextureFormat.Depth24Stencil8 ||
            format == TextureFormat.Depth32fStencil8)
            _texture2DDesc.BindFlags = (uint)BindFlag.DepthStencil;
        else
            _texture2DDesc.BindFlags = (uint)(BindFlag.RenderTarget | BindFlag.ShaderResource);

        SilkMarshal.ThrowHResult(_device.CreateTexture2D(in _texture2DDesc, (SubresourceData*)null, ref _texture2D));
        _textureResource = ComPtr.Downcast<ID3D11Texture2D, ID3D11Resource>(_texture2D);
        Initialized = true;
    }

    public void SetData<TData>(Vector2D<uint> size, Vector2D<int> offset, ReadOnlySpan<TData> data, int mipmapLevel = 0) where TData : unmanaged
    {
        uint bytesPerPixel = TextureFormat.GetBytesPerPixel();
        uint srcRowPitch = size.X * bytesPerPixel;
        uint srcDepthPitch = srcRowPitch * size.Y;

        uint subresourceIndex = D3D11CalcSubresource((uint)mipmapLevel, 0, MipmapLevels);

        Box box = new()
        {
            Left = (uint)offset.X,
            Top = (uint)offset.Y,
            Front = 0,
            Right = (uint)(offset.X + (int)size.X),
            Bottom = (uint)(offset.Y + (int)size.Y),
            Back = 1
        };

        fixed (TData* dataPtr = data)
        {
            _context.UpdateSubresource(
                _texture2D,
                subresourceIndex,
                &box,
                dataPtr,
                srcRowPitch,
                srcDepthPitch
            );
        }
    }

    public override void Dispose()
    {
        if ((IntPtr)_texture2D.Handle == IntPtr.Zero) return;

        _textureResource.Dispose();
        _textureResource = default;

        _texture2D.Dispose();
        _texture2D = default;
    }

    private static uint D3D11CalcSubresource(uint mipSlice, uint arraySlice, uint mipLevels) => mipSlice + arraySlice * mipLevels;
}
