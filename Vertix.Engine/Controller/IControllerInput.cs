using System;

namespace Vertix.Engine.Controller;

public interface IControllerInput<TValue> where TValue : unmanaged
{
    TValue Value { get; }

    event EventHandler<double> OnUpdate;
}
