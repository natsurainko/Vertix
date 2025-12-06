using Vertix.Graphics;
using Vertix.Graphics.Resources;

namespace Vertix.Engine.Scene;

public class SceneObject3D : GameObject3D
{
    public Model? Model { get; set; }

    public override void Draw(IGraphicsDevice graphicsDevice)
    {
        Model?.Draw(graphicsDevice);
    }
}
