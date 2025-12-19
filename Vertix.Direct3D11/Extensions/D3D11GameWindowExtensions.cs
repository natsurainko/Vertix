using Silk.NET.Core.Native;
using Silk.NET.DXGI;
using Vertix.Direct3D11.Windowing;

namespace Vertix.Direct3D11.Extensions;

public static class D3D11GameWindowExtensions
{
    extension(D3D11GameWindow window)
    {
        public void SwapBuffers(bool vsync = true)
        {
            if (vsync)
                SilkMarshal.ThrowHResult(window._swapChain.Present(1, 0));
            else
                SilkMarshal.ThrowHResult(window._swapChain.Present(0, DXGI.PresentAllowTearing));
        }
    }
}
