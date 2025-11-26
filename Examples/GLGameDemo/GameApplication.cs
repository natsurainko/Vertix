using Vertix.Graphics.Text;

namespace GLGameDemo;

internal class GameApplication(IServiceProvider sp) : Vertix.Host.GameApplication(sp)
{
    public static FontFamily? Unifont;
    public static FontFamily? Msyh;

    public static FontFamily? JetbrainsMono;
    public static FontFamily? JetBrainsMono_Italic;
    public static FontFamily? JetBrainsMono_Bold;

    protected override void OnLoading()
    {
        Unifont = new FontFamily("Assets/Fonts/unifont-17.0.02.otf");
        Msyh = new FontFamily("Assets/Fonts/MSYH.TTC");

        JetbrainsMono = new FontFamily("Assets/Fonts/JetBrainsMono-Regular.ttf");
        JetBrainsMono_Italic = new FontFamily("Assets/Fonts/JetBrainsMono-Italic.ttf");
        JetBrainsMono_Bold = new FontFamily("Assets/Fonts/JetBrainsMono-Bold.ttf");
    }
}
