using GLRenderPipelineDemo.Controlling;
using GLRenderPipelineDemo.Rendering;
using Silk.NET.Input;
using Silk.NET.Windowing;
using System.Numerics;
using Vertix.Content;
using Vertix.Engine.Controller;
using Vertix.Engine.Scene;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Windowing;

namespace GLRenderPipelineDemo;

internal class MainWindow(IWindow w, IGraphicsDevice d,
    RenderContext renderContext, RenderPipeline renderPipeline, AssetImporter assetImporter) : GLGameWindow(w, d)
{
    MouseControllerInput? _mouseController;
    KeyboradControllerInput? _keyboradController;

    protected override void OnLoaded()
    {
        CoreWindow.WindowBorder = WindowBorder.Fixed;
        Graphics.EnableDepthTest = true;
        IInputContext inputContext = this.CoreWindow.CreateInput();

        _mouseController = new MouseControllerInput(inputContext);
        _keyboradController = new KeyboradControllerInput(inputContext);

        RotationController rotationController = new(_mouseController);
        PositionController positionController = new(_keyboradController) { Speed = 10f };

        rotationController.AttachObject(renderContext.PerspectiveCamera);
        positionController.AttachObject(renderContext.PerspectiveCamera);

        Model planeModel = assetImporter.LoadModel("Assets/Models/Plane.fbx");
        Model blockModel = assetImporter.LoadModel("Assets/Models/Block.fbx");

        Graphics.InitializeModelMeshesVertexArray(planeModel);
        Graphics.InitializeModelMeshesVertexArray(blockModel);

        renderContext.SceneManager.AddObject(new SceneObject3D() { Model = planeModel });
        renderContext.SceneManager.AddObject(new SceneObject3D() { Model = blockModel, Position = new(0, 0.5f, 0) });
        renderContext.SceneManager.AddObject(new SceneObject3D() { Model = blockModel, Position = new(2, 3, 2) });

        renderContext.DirectionalLight.FitToScene(Vector3.Zero, 7f);
    }

    protected override void OnRender(double delateTime)
    {
        renderPipeline.Execute();
        renderPipeline.FinalDraw();
    }

    protected override void OnUpdate(double deltaTime)
    {
        _keyboradController?.Update(deltaTime);
        renderContext.Update();
    }
}
