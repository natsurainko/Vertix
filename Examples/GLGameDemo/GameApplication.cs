using Vertix.Graphics.Text;

namespace GLGameDemo;

internal class GameApplication(IServiceProvider sp) : Vertix.Host.GameApplication(sp)
{
    public static FontFamily Unifont;

    protected override void OnLoading()
    {
        Unifont = new FontFamily("Assets/unifont-17.0.02.otf");
    }
}
