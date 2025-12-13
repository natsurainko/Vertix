using Silk.NET.Input;
using System.Numerics;
using Vertix.Engine.Controller;

namespace GLRenderPipelineDemo.Controlling;

internal class KeyboradControllerInput : IControllerInput<Vector3>
{
    private readonly IInputContext _inputContext;
    private readonly List<Key> _pressingKeys = [];
    private Vector3 _movingOffset;

    public event EventHandler<double>? OnUpdate;

    public IKeyboard? Keyboard { get; private set; }

    public Vector3 Value => _movingOffset;

    public KeyboradControllerInput(IInputContext inputContext)
    {
        _inputContext = inputContext;

        ConfigureDeivces();
    }

    public void Update(double deltaTime)
    {
        foreach (var key in _pressingKeys)
        {
            _movingOffset.X += key switch
            {
                Key.W => 1,
                Key.S => -1,
                _ => 0
            };
            _movingOffset.Y += key switch
            {
                Key.Space => 1,
                Key.ShiftLeft => -1,
                _ => 0
            };
            _movingOffset.Z += key switch
            {
                Key.A => -1,
                Key.D => 1,
                _ => 0
            };
        }

        OnUpdate?.Invoke(this, deltaTime);
        _movingOffset = Vector3.Zero;
    }

    private void ConfigureDeivces()
    {
        Keyboard?.KeyDown -= OnKeyDown;
        Keyboard?.KeyUp -= OnKeyUp;
        Keyboard = _inputContext.Keyboards.FirstOrDefault(k => k.IsConnected);

        if (Keyboard != null)
        {
            Keyboard.KeyDown += OnKeyDown;
            Keyboard.KeyUp += OnKeyUp;
        }
    }

    private void OnKeyUp(IKeyboard arg1, Key arg2, int arg3) => _pressingKeys.Remove(arg2);

    private void OnKeyDown(IKeyboard arg1, Key arg2, int arg3)
    {
        if (!_pressingKeys.Contains(arg2))
            _pressingKeys.Add(arg2);
    }
}
