using System;
using System.Collections.Generic;
using System.Numerics;

namespace Vertix.Engine.Controller;

public class PositionController
{
    private readonly IControllerInput<Vector3> _controllerInput;
    private readonly List<GameObject3D> _object3Ds = [];

    public float Speed { get; set; } = 1.0f;

    public bool MultiplyDeltaTime { get; set; } = true;

    public PositionController(IControllerInput<Vector3> controllerInput)
    {
        _controllerInput = controllerInput;
        _controllerInput.OnUpdate += OnUpdate;
    }

    public void AttachObject(GameObject3D gameObject3D) => _object3Ds.Add(gameObject3D);

    public bool DettachObject(GameObject3D gameObject3D) => _object3Ds.Remove(gameObject3D);

    protected void OnUpdate(object? sender, double deltaTime)
    {
        if (deltaTime <= 0 || _controllerInput.Value == Vector3.Zero)
            return;

        deltaTime = Math.Pow(deltaTime, MultiplyDeltaTime ? 1.0 : 0.0);
        Vector3 vector = _controllerInput.Value * Speed * (float)deltaTime;

        for (int i = 0; i < _object3Ds.Count; i++)
            _object3Ds[i].Move(vector);
    }
}
