using Silk.NET.Windowing;
using System.Drawing;
using Vertix.Direct3D11.Windowing;
using Vertix.Graphics;

namespace D3D11GameDemo;

internal class MainWindow(IWindow w, IGraphicsDevice d) : D3D11GameWindow(w, d)
{
    protected override void OnLoaded()
    {
        
    }

    protected override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color, Color.CornflowerBlue);
    }

    protected override void OnUpdate(double deltaTime)
    {
        
    }
}
