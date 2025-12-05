using Silk.NET.Maths;
using Silk.NET.Windowing;
using Vertix.Extensions;

namespace GLGameDemo;

internal class RenderContext
{
    private readonly IWindow CoreWindow;

    public Rectangle<float> WindowRectangle;
    public Matrix4X4<float> WindowMatrix;
    public Matrix4X4<float> WindowProjectionMatrix;

    public RenderContext(IWindow window)
    {
        CoreWindow = window;
        CoreWindow.Resize += Window_Resize;

        WindowRectangle = new Rectangle<float>(0, 0, CoreWindow.Size.X, CoreWindow.Size.Y);
        WindowMatrix = WindowRectangle.ToScreenMatrix();
        WindowProjectionMatrix = Matrix4X4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);
    }

    private void Window_Resize(Vector2D<int> obj)
    {
        WindowRectangle = new Rectangle<float>(0, 0, CoreWindow.Size.X, CoreWindow.Size.Y);
        WindowMatrix = WindowRectangle.ToScreenMatrix();
        WindowProjectionMatrix = Matrix4X4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);
    }
}
