using System;

namespace Vertix.Graphics.Text.Helpers;

internal static class SDFHelper
{
    public static void GenerateSDF(ref Span<byte> bitmap, int width, int height, float spread = 8.0f)
    {
        if (bitmap.IsEmpty || width <= 0 || height <= 0)
            return;

        int length = width * height;

        Span<float> distInside = stackalloc float[length];
        Span<float> distOutside = stackalloc float[length];

        const float maxDist = 1e10f;
        for (int i = 0; i < distInside.Length; i++)
        {
            bool isInside = bitmap[i] > 127;
            distInside[i] = isInside ? 0 : maxDist;
            distOutside[i] = isInside ? maxDist : 0;
        }

        DeadReckoningPass(ref distInside, width, height);
        DeadReckoningPass(ref distOutside, width, height);

        for (int i = 0; i < length; i++)
        {
            float distIn = MathF.Sqrt(distInside[i]);
            float distOut = MathF.Sqrt(distOutside[i]);
            float signedDist = distOut - distIn;

            float normalized = (signedDist / spread) * 127.0f + 128.0f;
            bitmap[i] = (byte)Math.Clamp(normalized, 0, 255);
        }
    }

    private static void DeadReckoningPass(ref Span<float> dist, int width, int height)
    {
        for (int y = 1; y < height; y++)
        {
            for (int x = 1; x < width - 1; x++)
            {
                int i = y * width + x;
                float d = dist[i];

                d = MathF.Min(d, dist[i - width] + 1.0f);
                d = MathF.Min(d, dist[i - 1] + 1.0f);
                d = MathF.Min(d, dist[i - width - 1] + 1.414f);
                d = MathF.Min(d, dist[i - width + 1] + 1.414f);

                dist[i] = d;
            }
        }

        for (int y = height - 2; y >= 0; y--)
        {
            for (int x = width - 2; x >= 1; x--)
            {
                int i = y * width + x;
                float d = dist[i];

                d = MathF.Min(d, dist[i + width] + 1.0f);
                d = MathF.Min(d, dist[i + 1] + 1.0f);
                d = MathF.Min(d, dist[i + width + 1] + 1.414f);
                d = MathF.Min(d, dist[i + width - 1] + 1.414f);

                dist[i] = d;
            }
        }
    }
}
