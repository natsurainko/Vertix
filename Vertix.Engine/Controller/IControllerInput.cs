using Silk.NET.Maths;
using System;

namespace Vertix.Engine.Controller;

public interface IControllerInput
{
    Vector3D<float> MovingOffset { get; }

    Vector3D<float> RotatingOffset { get; }

    event EventHandler<double> OnUpdate;
}
