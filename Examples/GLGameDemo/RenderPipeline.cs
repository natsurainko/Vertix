using Vertix.Graphics;

namespace GLGameDemo;

internal class RenderPipeline : Vertix.Rendering.RenderPipeline<RenderContext>
{
    public RenderPipeline(IGraphicsDevice d, RenderContext c) : base(d,c)
    {
        //AddPass<>();
    }

    public override void FinalDraw()
    {
        
    }
}
