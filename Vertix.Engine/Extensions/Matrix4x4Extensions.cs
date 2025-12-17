using System;
using System.Numerics;

namespace Vertix.Engine.Extensions;

public static class Matrix4x4Extensions
{
    extension (Matrix4x4)
    {
        public static Matrix4x4 CreateLightViewProjectionForFrustum(Vector3 lightDirection, Matrix4x4 viewProjectionMatrix, float zMult = 10)
        {
            Matrix4x4.Invert(viewProjectionMatrix, out var inv);
            Vector3 center = Vector3.Zero;

            Span<Vector4> vector4Ds = stackalloc Vector4[8];

            int index = 0;
            for (int x = 0; x < 2; ++x)
            {
                for (int y = 0; y < 2; ++y)
                {
                    for (int z = 0; z < 2; ++z)
                    {
                        Vector4 pt = Vector4.Transform(new Vector4
                        (
                            2.0f * x - 1.0f,
                            2.0f * y - 1.0f,
                            z, // 2.0f * z - 1.0f
                            1.0f
                        ), inv);

                        pt /= pt.W;
                        center += new Vector3(pt.X, pt.Y, pt.Z);

                        vector4Ds[index++] = pt;
                    }
                }
            }

            center /= 8;
            Matrix4x4 lightViewMatrix = Matrix4x4.CreateLookAt(center, center + lightDirection, Vector3.UnitY);

            float minX = float.MaxValue;
            float minY = float.MaxValue;
            float minZ = float.MaxValue;

            float maxX = float.MinValue;
            float maxY = float.MinValue;
            float maxZ = float.MinValue;

            for (int i = 0; i < 8; i++)
            {
                Vector4 transform = Vector4.Transform(vector4Ds[i], lightViewMatrix);
                minX = MathF.Min(minX, transform.X);
                minY = MathF.Min(minY, transform.Y);
                minZ = MathF.Min(minZ, transform.Z);

                maxX = MathF.Max(maxX, transform.X);
                maxY = MathF.Max(maxY, transform.Y);
                maxZ = MathF.Max(maxZ, transform.Z);
            }

            if (minZ < 0)
                minZ *= zMult;
            else
                minZ /= zMult;

            if (maxZ < 0)
                maxZ /= zMult;
            else
                maxZ *= zMult;

            return lightViewMatrix * Matrix4x4.CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
        }
    }
}
