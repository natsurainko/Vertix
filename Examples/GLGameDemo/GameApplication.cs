using Silk.NET.OpenGL;
using Vertix.Graphics.Resources;
using Vertix.Graphics.Text;

namespace GLGameDemo;

internal class GameApplication(IServiceProvider sp) : Vertix.Host.GameApplication(sp)
{
    public static FontFamily? Unifont;
    public static FontFamily? Msyh;

    public static FontFamily? JetbrainsMono;

    protected override void OnLoading()
    {
        Unifont = new FontFamily("Assets/Fonts/unifont-17.0.02.otf");
        Msyh = new FontFamily("Assets/Fonts/MSYH.TTC");

        JetbrainsMono = new FontFamily("Assets/Fonts/JetBrainsMono-Regular.ttf");
    }

    internal static readonly (ShaderType, string)[] _2D_FONT_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/shader2D.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/shader2D_font.frag"),
    ];

    internal static readonly (ShaderType, string)[] _2D_BASIC_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/shader2D.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/shader2D.frag"),
    ];

    internal static readonly (ShaderType, string)[] _3D_BASIC_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/shader3D.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/shader3D.frag"),
    ];

    internal static readonly uint[] RectangleIndices = [0, 1, 3, 1, 2, 3];
    internal static readonly Vertex2D[] RectangleVertices =
    [
        new() { Position = new(-1, -1, 0), TextureCoord = new(0, 0) },
        new() { Position = new(-1, 1, 0), TextureCoord = new(0, 1) },
        new() { Position = new(1, 1, 0), TextureCoord = new(1, 1) },
        new() { Position = new(1, -1, 0), TextureCoord = new(1, 0) },
    ];
}
