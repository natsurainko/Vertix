using Vertix.Graphics.Text;

namespace GLGameDemo;

internal class GameApplication(IServiceProvider sp) : Vertix.Host.GameApplication(sp)
{
    public static FontFamily Unifont;
    public static FontFamily JetbrainsMono;
    public static FontFamily Msyh;

    protected override void OnLoading()
    {
        Unifont = new FontFamily("Assets/Fonts/unifont-17.0.02.otf");
        JetbrainsMono = new FontFamily("Assets/Fonts/JetBrainsMono-Regular.ttf");
        Msyh = new FontFamily("Assets/Fonts/MSYH.TTC");
    }
}
