using GLRenderPipelineDemo.Controlling;
using GLRenderPipelineDemo.Rendering;
using Silk.NET.Input;
using Silk.NET.Windowing;
using System.Numerics;
using Vertix.Engine.Content;
using Vertix.Engine.Controller;
using Vertix.Engine.Scene;
using Vertix.Graphics;
using Vertix.Graphics.Primitives;
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

        Model model = assetImporter.LoadModel(@"Assets/Models/Pool.fbx");
        model.InitializeMeshesVertexArray(Graphics);
        renderContext.SceneManager.AddObject(new SceneObject3D() { Model = model, Scale = Vector3.One * 0.025f });

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
