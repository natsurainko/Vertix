using Silk.NET.Input;
using System.Numerics;
using Vertix.Engine.Controller;

namespace GLRenderPipelineDemo.Controlling;

internal class MouseControllerInput : IControllerInput<Vector3>
{
    private readonly IInputContext _inputContext;
    private Vector3 _rotatingOffset;
    private Vector2 _lastPosition;

    public event EventHandler<double>? OnUpdate;

    public IMouse? Mouse { get; private set; }

    public Vector3 Value => _rotatingOffset;

    public MouseControllerInput(IInputContext inputContext)
    {
        _inputContext = inputContext;

        ConfigureDeivces();
    }

    private void ConfigureDeivces()
    {
        Mouse?.MouseMove -= OnMouseMove;
        Mouse = _inputContext.Mice.FirstOrDefault(m => m.IsConnected);

        if (Mouse != null)
        {
            Mouse.Cursor.CursorMode = CursorMode.Disabled;
            Mouse.MouseMove += OnMouseMove;

            _lastPosition = Mouse.Position;
        }
    }

    private void OnMouseMove(IMouse arg1, Vector2 arg2)
    {
        Vector2 delta = _lastPosition - arg2;
        _lastPosition = arg2;

        _rotatingOffset.X = delta.Y;
        _rotatingOffset.Y = delta.X;

        OnUpdate?.Invoke(this, 0);
        _rotatingOffset = Vector3.Zero;
    }
}
