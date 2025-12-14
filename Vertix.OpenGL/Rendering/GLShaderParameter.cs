using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using System.Numerics;
using Vertix.Rendering;

namespace Vertix.OpenGL.Rendering;

public readonly struct GLShaderParameter : IShaderParameter
{
    private readonly GL _gL;
    private readonly uint _handle;
    private readonly int _location;
    private readonly Type? _type;

    public string Name { get; }

    public UniformType UniformType { get; }

    public GLShaderParameter(GL gL, uint handle, string name, UniformType uniformType)
    {
        _gL = gL;
        _handle = handle;

        Name = name;
        UniformType = uniformType;

        _location = _gL.GetUniformLocation(handle, Name);
        _type = uniformType switch
        {
            UniformType.Int => typeof(int),
            UniformType.Float => typeof(float),
            UniformType.Double => typeof(double),
            UniformType.Bool => typeof(bool),
            UniformType.UnsignedInt => typeof(uint),
            UniformType.IntVec2 => typeof(Vector2D<int>),
            UniformType.IntVec3 => typeof(Vector3D<int>),
            UniformType.IntVec4 => typeof(Vector4D<int>),
            UniformType.FloatVec2 => typeof(Vector2),
            UniformType.FloatVec3 => typeof(Vector3),
            UniformType.FloatVec4 => typeof(Vector4),
            UniformType.FloatMat2 => typeof(Matrix2X2<float>),
            UniformType.FloatMat3 => typeof(Matrix3X3<float>),
            UniformType.FloatMat4 => typeof(Matrix4x4),
            _ => null,
        };
    }

    public unsafe void SetValue<T>(T value) where T : unmanaged
    {
        if (_type == null) throw new InvalidOperationException();
        if (typeof(T) != _type) throw new ArgumentException($"The value should be {_type.Name} type");

        switch (UniformType)
        {
            case UniformType.Int:
                _gL.ProgramUniform1(_handle, _location, (int)(object)value);
                break;
            case UniformType.UnsignedInt:
                _gL.ProgramUniform1(_handle, _location, (uint)(object)value);
                break;
            case UniformType.Float:
                _gL.ProgramUniform1(_handle, _location, (float)(object)value);
                break;
            case UniformType.Double:
                _gL.ProgramUniform1(_handle, _location, (double)(object)value);
                break;
            case UniformType.Bool:
                _gL.ProgramUniform1(_handle, _location, ((bool)(object)value) ? 1 : 0);
                break;

            // Vector types
            case UniformType.FloatVec2:
                _gL.ProgramUniform2(_handle, _location, (Vector2)(object)value);
                break;
            case UniformType.FloatVec3:
                _gL.ProgramUniform3(_handle, _location, (Vector3)(object)value);
                break;
            case UniformType.FloatVec4:
                _gL.ProgramUniform4(_handle, _location, (Vector4)(object)value);
                break;
            case UniformType.IntVec2:
                Vector2D<int> v2i = (Vector2D<int>)(object)value;
                _gL.ProgramUniform2(_handle, _location, v2i.X, v2i.Y);
                break;
            case UniformType.IntVec3:
                Vector3D<int> v3i = (Vector3D<int>)(object)value;
                _gL.ProgramUniform3(_handle, _location, v3i.X, v3i.Y, v3i.Z);
                break;
            case UniformType.IntVec4:
                Vector4D<int> v4i = (Vector4D<int>)(object)value;
                _gL.ProgramUniform4(_handle, _location, v4i.X, v4i.Y, v4i.Z, v4i.W);
                break;
            case UniformType.UnsignedIntVec2:
                Vector2D<uint> v2ui = (Vector2D<uint>)(object)value;
                _gL.ProgramUniform2(_handle, _location, v2ui.X, v2ui.Y);
                break;
            case UniformType.UnsignedIntVec3:
                Vector3D<uint> v3ui = (Vector3D<uint>)(object)value;
                _gL.ProgramUniform3(_handle, _location, v3ui.X, v3ui.Y, v3ui.Z);
                break;
            case UniformType.UnsignedIntVec4:
                Vector4D<uint> v4ui = (Vector4D<uint>)(object)value;
                _gL.ProgramUniform4(_handle, _location, v4ui.X, v4ui.Y, v4ui.Z, v4ui.W);
                break;
            case UniformType.DoubleVec2:
                Vector2D<double> v2d = (Vector2D<double>)(object)value;
                _gL.ProgramUniform2(_handle, _location, v2d.X, v2d.Y);
                break;
            case UniformType.DoubleVec3:
                Vector3D<double> v3d = (Vector3D<double>)(object)value;
                _gL.ProgramUniform3(_handle, _location, v3d.X, v3d.Y, v3d.Z);
                break;
            case UniformType.DoubleVec4:
                Vector4D<double> v4d = (Vector4D<double>)(object)value;
                _gL.ProgramUniform4(_handle, _location, v4d.X, v4d.Y, v4d.Z, v4d.W);
                break;

            // Matrix types - float
            case UniformType.FloatMat2:
                Matrix2X2<float> m2f = (Matrix2X2<float>)(object)value;
                Span<float> floats = [m2f.M11, m2f.M12, m2f.M21, m2f.M22];
                _gL.ProgramUniformMatrix2(_handle, _location, 1, false, floats);
                break;

            case UniformType.FloatMat3:
                Matrix3X3<float> m3f = (Matrix3X3<float>)(object)value;
                Span<float> floats3 = [
                    m3f.M11, m3f.M12, m3f.M13,
                    m3f.M21, m3f.M22, m3f.M23,
                    m3f.M31, m3f.M32, m3f.M33
                ];
                _gL.ProgramUniformMatrix3(_handle, _location, 1, false, floats3);
                break;

            case UniformType.FloatMat4:
                Matrix4x4 m4f = (Matrix4x4)(object)value;
                float* ptr = &m4f.M11;
                _gL.ProgramUniformMatrix4(_handle, _location, 1, false, ptr);
                break;

            // Matrix types - double
            case UniformType.DoubleMat2:
                Matrix2X2<double> m2d = (Matrix2X2<double>)(object)value;
                Span<double> doubles2 = [m2d.M11, m2d.M12, m2d.M21, m2d.M22];
                _gL.ProgramUniformMatrix2(_handle, _location, 1, false, doubles2);
                break;

            case UniformType.DoubleMat3:
                Matrix3X3<double> m3d = (Matrix3X3<double>)(object)value;
                Span<double> doubles3 = [
                    m3d.M11, m3d.M12, m3d.M13,
                    m3d.M21, m3d.M22, m3d.M23,
                    m3d.M31, m3d.M32, m3d.M33
                ];
                _gL.ProgramUniformMatrix3(_handle, _location, 1, false, doubles3);
                break;

            case UniformType.DoubleMat4:
                Matrix4X4<double> m4d = (Matrix4X4<double>)(object)value;
                Span<double> doubles4 = [
                    m4d.M11, m4d.M12, m4d.M13, m4d.M14,
                    m4d.M21, m4d.M22, m4d.M23, m4d.M24,
                    m4d.M31, m4d.M32, m4d.M33, m4d.M34,
                    m4d.M41, m4d.M42, m4d.M43, m4d.M44
                ];
                _gL.ProgramUniformMatrix4(_handle, _location, 1, false, doubles4);
                break;

            /*
            case UniformType.FloatMat2x3:
            case UniformType.FloatMat2x4:
            case UniformType.FloatMat3x2:
            case UniformType.FloatMat3x4:
            case UniformType.FloatMat4x2:
            case UniformType.FloatMat4x3:
            case UniformType.DoubleMat2x3:
            case UniformType.DoubleMat2x4:
            case UniformType.DoubleMat3x2:
            case UniformType.DoubleMat3x4:
            case UniformType.DoubleMat4x2:
            case UniformType.DoubleMat4x3:
            case UniformType.BoolVec2:
            case UniformType.BoolVec3:
            case UniformType.BoolVec4:
            case UniformType.Sampler1D:
            case UniformType.Sampler2D:
            case UniformType.Sampler3D:
            case UniformType.SamplerCube:
            case UniformType.Sampler1DShadow:
            case UniformType.Sampler2DShadow:
            case UniformType.Sampler2DRect:
            case UniformType.Sampler2DRectShadow:
            case UniformType.Sampler1DArray:
            case UniformType.Sampler2DArray:
            case UniformType.SamplerBuffer:
            case UniformType.Sampler1DArrayShadow:
            case UniformType.Sampler2DArrayShadow:
            case UniformType.SamplerCubeShadow:
            case UniformType.IntSampler1D:
            case UniformType.IntSampler2D:
            case UniformType.IntSampler3D:
            case UniformType.IntSamplerCube:
            case UniformType.IntSampler2DRect:
            case UniformType.IntSampler1DArray:
            case UniformType.IntSampler2DArray:
            case UniformType.IntSamplerBuffer:
            case UniformType.UnsignedIntSampler1D:
            case UniformType.UnsignedIntSampler2D:
            case UniformType.UnsignedIntSampler3D:
            case UniformType.UnsignedIntSamplerCube:
            case UniformType.UnsignedIntSampler2DRect:
            case UniformType.UnsignedIntSampler1DArray:
            case UniformType.UnsignedIntSampler2DArray:
            case UniformType.UnsignedIntSamplerBuffer:
            case UniformType.SamplerCubeMapArray:
            case UniformType.SamplerCubeMapArrayShadow:
            case UniformType.IntSamplerCubeMapArray:
            case UniformType.UnsignedIntSamplerCubeMapArray:
            case UniformType.Sampler2DMultisample:
            case UniformType.IntSampler2DMultisample:
            case UniformType.UnsignedIntSampler2DMultisample:
            case UniformType.Sampler2DMultisampleArray:
            case UniformType.IntSampler2DMultisampleArray:
            case UniformType.UnsignedIntSampler2DMultisampleArray:
                break;
            */

            default:
                throw new NotSupportedException($"Uniform type {UniformType} is not supported");
        }
    }

    public unsafe void SetValues<T>(ReadOnlySpan<T> values, uint length) where T : unmanaged
    {
        if (_type == null) throw new InvalidOperationException();
        if (typeof(T) != _type) throw new ArgumentException($"The value should be {_type.Name} type");

        switch (UniformType)
        {
            case UniformType.Int:
                fixed (void* ptr = values)
                    _gL.ProgramUniform1(_handle, _location, length, (int*)ptr);
                break;
            case UniformType.UnsignedInt:
                fixed (void* ptr = values)
                    _gL.ProgramUniform1(_handle, _location, length, (uint*)ptr);
                break;
            case UniformType.Float:
                fixed (void* ptr = values)
                    _gL.ProgramUniform1(_handle, _location, length, (float*)ptr);
                break;
            case UniformType.Double:
                fixed (void* ptr = values)
                    _gL.ProgramUniform1(_handle, _location, length, (double*)ptr);
                break;

            case UniformType.FloatVec2:
                fixed (void* ptr = values)
                    _gL.ProgramUniform2(_handle, _location, length, (float*)ptr);
                break;

            case UniformType.FloatVec3:
                fixed (void* ptr = values)
                    _gL.ProgramUniform3(_handle, _location, length, (float*)ptr);
                break;

            case UniformType.FloatVec4:
                fixed (void* ptr = values)
                    _gL.ProgramUniform4(_handle, _location, length, (float*)ptr);
                break;

            default:
                throw new NotSupportedException($"Uniform type {UniformType} is not supported");
        }
    }
}
