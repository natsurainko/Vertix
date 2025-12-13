using System.Collections.Generic;
using System.Numerics;

namespace Vertix.Engine.Controller;

public class RotationController
{
    private readonly IControllerInput<Vector3> _controllerInput;
    private readonly List<GameObject3D> _object3Ds = [];

    public float Sensitivity { get; set; } = 0.001f;

    public RotationController(IControllerInput<Vector3> controllerInput)
    {
        _controllerInput = controllerInput;
        _controllerInput.OnUpdate += OnUpdate;
    }

    public void AttachObject(GameObject3D gameObject3D) => _object3Ds.Add(gameObject3D);

    public bool DettachObject(GameObject3D gameObject3D) => _object3Ds.Remove(gameObject3D);

    private void OnUpdate(object? sender, double deltaTime)
    {
        if (_controllerInput.Value == Vector3.Zero)
            return;

        Vector3 vector = _controllerInput.Value * Sensitivity;

        for (int i = 0; i < _object3Ds.Count; i++)
            _object3Ds[i].Rotate(vector);
    }
}
