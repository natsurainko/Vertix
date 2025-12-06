using Silk.NET.Maths;
using System.Collections.Generic;

namespace Vertix.Engine.Controller;

public class RotationController
{
    private readonly IControllerInput _controllerInput;
    private readonly List<GameObject3D> _object3Ds = [];

    public float Sensitivity { get; set; } = 0.001f;

    public RotationController(IControllerInput controllerInput)
    {
        _controllerInput = controllerInput;
        _controllerInput.OnUpdate += OnUpdate;
    }

    public void AttachObject(GameObject3D gameObject3D) => _object3Ds.Add(gameObject3D);

    public bool DettachObject(GameObject3D gameObject3D) => _object3Ds.Remove(gameObject3D);

    private void OnUpdate(object? sender, double deltaTime)
    {
        if (_controllerInput.RotatingOffset == Vector3D<float>.Zero)
            return;

        Vector3D<float> vector = _controllerInput.RotatingOffset * Sensitivity;

        for (int i = 0; i < _object3Ds.Count; i++)
            _object3Ds[i].Rotate(vector);
    }
}
