using Vertix.Graphics.Text;

namespace GLRenderPipelineDemo;

internal class GameApplication : Vertix.Host.GameApplication
{
    public static FontFamily? Unifont;

    public GameApplication()
    {
        Unifont = new FontFamily("Assets/Fonts/unifont-17.0.02.otf");
    }
}
