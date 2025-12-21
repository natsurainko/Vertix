using Silk.NET.Windowing;
using System.Drawing;
using Vertix.Direct3D11.Windowing;
using Vertix.Graphics;

namespace D3D11GameDemo;

internal class MainWindow(IWindow w, IGraphicsDevice d) : D3D11GameWindow(w, d)
{
    protected override void OnLoaded()
    {
        CoreWindow.Title = "Direct3D 11 Game Demo";

        //Graphics.Viewport(new Rectangle<float>(Vector2D<float>.Zero, Vector2D<float>.One * 200));
    }

    protected override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth | ClearBufferMask.Stencil, Color.CornflowerBlue);
    }

    protected override void OnUpdate(double deltaTime)
    {
        
    }
}
