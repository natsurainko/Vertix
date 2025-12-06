using Silk.NET.Maths;
using System.Collections.Generic;

namespace Vertix.Engine.Controller;

public class PositionController
{
    private readonly IControllerInput _controllerInput;
    private readonly List<GameObject3D> _object3Ds = [];

    public float Speed { get; set; } = 1.0f;

    public bool MultiplyDeltaTime { get; set; } = true;

    public PositionController(IControllerInput controllerInput)
    {
        _controllerInput = controllerInput;
        _controllerInput.OnUpdate += OnUpdate;
    }

    public void AttachObject(GameObject3D gameObject3D) => _object3Ds.Add(gameObject3D);

    public bool DettachObject(GameObject3D gameObject3D) => _object3Ds.Remove(gameObject3D);

    private void OnUpdate(object? sender, double deltaTime)
    {
        if (deltaTime <= 0 || _controllerInput.MovingOffset == Vector3D<float>.Zero)
            return;

        deltaTime = Scalar.Pow(deltaTime, MultiplyDeltaTime ? 1.0 : 0.0);
        Vector3D<float> vector = _controllerInput.MovingOffset * Speed * (float)deltaTime;

        for (int i = 0; i < _object3Ds.Count; i++)
            _object3Ds[i].Move(vector);
    }
}
