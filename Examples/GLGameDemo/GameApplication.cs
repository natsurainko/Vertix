using Silk.NET.OpenGL;
using Vertix.Graphics.Resources;
using Vertix.Graphics.Text;

namespace GLGameDemo;

internal class GameApplication(IServiceProvider sp) : Vertix.Host.GameApplication(sp)
{
    public static FontFamily? Unifont;
    public static FontFamily? Msyh;

    public static FontFamily? JetbrainsMono;
    //public static FontFamily? JetBrainsMono_Italic;
    //public static FontFamily? JetBrainsMono_Bold;

    protected override void OnLoading()
    {
        Unifont = new FontFamily("Assets/Fonts/unifont-17.0.02.otf");
        Msyh = new FontFamily("Assets/Fonts/MSYH.TTC");

        JetbrainsMono = new FontFamily("Assets/Fonts/JetBrainsMono-Regular.ttf");
        //JetBrainsMono_Italic = new FontFamily("Assets/Fonts/JetBrainsMono-Italic.ttf");
        //JetBrainsMono_Bold = new FontFamily("Assets/Fonts/JetBrainsMono-Bold.ttf");
    }

    internal static readonly (ShaderType, string)[] _2D_FONT_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/2D/sdf_font.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/2D/sdf_font.frag"),
    ];

    internal static readonly (ShaderType, string)[] _2D_BASIC_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/2D/rectangle.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/2D/rectangle.frag"),
    ];

    internal static readonly (ShaderType, string)[] _3D_BASIC_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/3D/triangles.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/3D/triangles.frag"),
    ];

    internal static readonly uint[] RectIndices = [0, 1, 3, 1, 2, 3];
    internal static readonly Vertex2D[] RectVertices =
    [
        new() { Position = new(-1, -1) },
        new() { Position = new(-1, 1) },
        new() { Position = new(1, 1) },
        new() { Position = new(1, -1) },
    ];
}
