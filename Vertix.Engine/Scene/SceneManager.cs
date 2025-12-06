using System.Collections.Generic;

namespace Vertix.Engine.Scene;

public class SceneManager
{
    private readonly List<GameObject3D> sceneObjects = [];

    public IReadOnlyList<GameObject3D> SceneObjects => sceneObjects;

    public void AddObject(GameObject3D obj) => sceneObjects.Add(obj);

    public void RemoveObject(GameObject3D obj) => sceneObjects.Remove(obj);

    public void ClearScene() => sceneObjects.Clear();
}
