/*
 *  Copyright 2019-2023 Diligent Graphics LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

using Diligent.Core;
using SharpGen.Runtime;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System;
using System.Drawing;
using System.Drawing.Imaging;
using Xunit;
using System.Numerics;
using System.Runtime.CompilerServices;
using Xunit.Abstractions;
using System.IO;
using IDeviceContext = Diligent.Core.IDeviceContext;

namespace DiligentCoreTest.NET;

public class DiligentGraphicsEngineTests
{
    struct Vertex
    {
        public Vector3 Position;
        public Vector4 Color;

        public Vertex(Vector3 position, Vector4 color)
        {
            Position = position;
            Color = color;
        }
    }

    private const string PathToAssets = "./Assets";
    private const int RenderTargetWidth = 512;
    private const int RenderTargetHeight = 512;
    private const TextureFormat ColorBufferFormat = TextureFormat.BGRA8_UNorm;
    private const TextureFormat DepthBufferFormat = TextureFormat.D32_Float;

    private readonly ITestOutputHelper m_Output;
    private readonly IEngineFactory m_EngineFactory;
    private readonly IRenderDevice m_RenderDevice;
    private readonly IDeviceContext m_DeviceContext;

    public DiligentGraphicsEngineTests(ITestOutputHelper output)
    {
        m_Output = output;

        m_EngineFactory = Native.CreateEngineFactory<IEngineFactoryD3D12>();
        Assert.NotNull(m_EngineFactory);

        m_EngineFactory.SetMessageCallback((severity, message, function, file, line) =>
        {
            switch (severity)
            {
                case DebugMessageSeverity.Warning:
                case DebugMessageSeverity.Error:
                case DebugMessageSeverity.FatalError:
                    m_Output.WriteLine($"Diligent Engine: {severity} in {function}() ({file}, {line}): {message}");
                    break;
                case DebugMessageSeverity.Info:
                    m_Output.WriteLine($"Diligent Engine: {severity} {message}");
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(severity), severity, null);
            }
        });

        ((IEngineFactoryD3D12)m_EngineFactory).CreateDeviceAndContextsD3D12(new()
        {
            EnableValidation = true
        }, out var renderDeviceOut, out var contextsOut);

        Assert.NotNull(renderDeviceOut);
        Assert.NotNull(contextsOut);

        m_RenderDevice = renderDeviceOut;
        m_DeviceContext = contextsOut[0];
    }

    private static Type FindType(string qualifiedTypeName)
    {
        var type = Type.GetType(qualifiedTypeName);

        if (type != null)
            return type;

        foreach (var assembly in AppDomain.CurrentDomain.GetAssemblies())
        {
            type = assembly.GetType(qualifiedTypeName);
            if (type != null)
                return type;
        }
        return null;
    }

    private static void CheckStructSizes(in APIInfo info)
    {
        int GetManagedSize(Type type)
        {
            var native = Array.Find(type.GetNestedTypes(BindingFlags.NonPublic), (e) => e.Name == "__Native");
            return Marshal.SizeOf(native ?? type);
        }

        var apiInfoType = typeof(APIInfo);
        var fields = apiInfoType.GetFields();

        foreach (var field in fields.Skip(2))
        {
            var type = FindType($"Diligent.Core.{field.Name.Replace("Size", "")}");
            var nativeSize = (PointerSize)field.GetValue(info)!;
            var managedSize = GetManagedSize(type);
            Assert.True(nativeSize == managedSize, $"{type.Name}: {nativeSize} -> {managedSize}");
        }
    }
    private static byte[] LoadImage(string fileName)
    {
        var bitmap = new Bitmap(fileName);
        var bitmapData = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height),
            ImageLockMode.ReadOnly,
            PixelFormat.Format32bppArgb);

        var totalBytes = bitmapData.Stride * bitmapData.Height;
        var pixelData = new byte[totalBytes];
        Marshal.Copy(bitmapData.Scan0, pixelData, 0, totalBytes);
        bitmap.UnlockBits(bitmapData);
        return pixelData;
    }

    private static void SaveImage(string fileName, byte[] data, int width, int height)
    {
        var bitmap = new Bitmap(width, height, PixelFormat.Format32bppArgb);
        var bitmapData = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height),
            ImageLockMode.ReadOnly,
            PixelFormat.Format32bppArgb);
        Marshal.Copy(data, 0, bitmapData.Scan0, 4 * width * height);
        bitmap.UnlockBits(bitmapData);
        bitmap.Save(fileName, ImageFormat.Png);
    }

    private static void CompareImages(MappedTextureSubresource mappedResource, string fileName)
    {
        var imageData = LoadImage(fileName);
        unsafe
        {
            var pData = (byte*)mappedResource.Data.ToPointer();
            var isEqual = true;
            for (var i = 0ul; i < RenderTargetWidth * RenderTargetHeight; i += 4)
            {
                if (pData[i + 0] != imageData[i + 0] || pData[i + 1] != imageData[i + 1] || pData[i + 2] != imageData[i + 2])
                {
                    isEqual = false;
                    break;
                }
            }

            if (!isEqual)
            {
                var stride = 2 * RenderTargetWidth;
                var combinedImage = new byte[4 * RenderTargetHeight * stride];
                for (var x = 0; x < RenderTargetWidth; x++)
                {
                    for (var y = 0; y < RenderTargetHeight; y++)
                    {
                        combinedImage[4 * (x + stride * y) + 0] = imageData[4 * (x + RenderTargetWidth * y) + 0];
                        combinedImage[4 * (x + stride * y) + 1] = imageData[4 * (x + RenderTargetWidth * y) + 1];
                        combinedImage[4 * (x + stride * y) + 2] = imageData[4 * (x + RenderTargetWidth * y) + 2];
                        combinedImage[4 * (x + stride * y) + 3] = 255;

                        combinedImage[4 * ((x + RenderTargetWidth - 1) + stride * y) + 0] = pData[4 * (x + RenderTargetWidth * y) + 0];
                        combinedImage[4 * ((x + RenderTargetWidth - 1) + stride * y) + 1] = pData[4 * (x + RenderTargetWidth * y) + 1];
                        combinedImage[4 * ((x + RenderTargetWidth - 1) + stride * y) + 2] = pData[4 * (x + RenderTargetWidth * y) + 2];
                        combinedImage[4 * ((x + RenderTargetWidth - 1) + stride * y) + 3] = 255;
                    }
                }

                var dumpFileName = Path.GetFileNameWithoutExtension(fileName) + "_Dump" + Path.GetExtension(fileName);
                SaveImage(dumpFileName, combinedImage, 2 * RenderTargetWidth, RenderTargetHeight);
                Assert.True(false, $"The rendered image will not match {fileName}");
            }
        }
    }

    private static Matrix4x4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
    {
        if (fieldOfView <= 0.0f || fieldOfView >= MathF.PI)
            throw new ArgumentOutOfRangeException(nameof(fieldOfView));

        if (nearPlaneDistance <= 0.0f)
            throw new ArgumentOutOfRangeException(nameof(nearPlaneDistance));

        if (farPlaneDistance <= 0.0f)
            throw new ArgumentOutOfRangeException(nameof(farPlaneDistance));

        if (nearPlaneDistance >= farPlaneDistance)
            throw new ArgumentOutOfRangeException(nameof(nearPlaneDistance));

        float yScale = 1.0f / MathF.Tan(fieldOfView * 0.5f);
        float xScale = yScale / aspectRatio;

        Matrix4x4 result;

        result.M11 = xScale;
        result.M12 = result.M13 = result.M14 = 0.0f;

        result.M22 = yScale;
        result.M21 = result.M23 = result.M24 = 0.0f;

        result.M31 = result.M32 = 0.0f;

        result.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
        result.M34 = 1.0f;

        result.M41 = result.M42 = result.M44 = 0.0f;
        result.M43 = -nearPlaneDistance * farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
        return result;
    }

    [Fact]
    public void ValidateLoadEngineFactory()
    {
        using var engineFactory0 = Native.CreateEngineFactory<IEngineFactoryD3D11>();
        Assert.NotNull(engineFactory0);

        using var engineFactory1 = Native.CreateEngineFactory<IEngineFactoryD3D12>();
        Assert.NotNull(engineFactory1);

        using var engineFactory2 = Native.CreateEngineFactory<IEngineFactoryVk>();
        Assert.NotNull(engineFactory2);

        using var engineFactory3 = Native.CreateEngineFactory<IEngineFactoryOpenGL>();
        Assert.NotNull(engineFactory3);
    }

    [Fact]
    public void ValidateApiVersion()
    {
        using var engineFactory = Native.CreateEngineFactory<IEngineFactoryD3D12>();
        Assert.NotNull(engineFactory);
        var apiVersion = engineFactory.GetAPIInfo();
        Assert.Equal(Native.ApiVersion, apiVersion.APIVersion);
    }

    [Fact]
    public void CheckStructureSizes()
    {
        using var engineFactory = Native.CreateEngineFactory<IEngineFactoryD3D12>();
        Assert.NotNull(engineFactory);
        CheckStructSizes(engineFactory.GetAPIInfo());
    }

    [Fact]
    public void CompareRenderedCubeImage()
    {
        using var renderTexture = m_RenderDevice.CreateTexture(new()
        {
            Name = "Color buffer",
            Type = ResourceDimension.Tex2d,
            Width = RenderTargetWidth,
            Height = RenderTargetHeight,
            SampleCount = 1,
            Usage = Usage.Default,
            Format = ColorBufferFormat,
            BindFlags = BindFlags.RenderTarget
        });
        Assert.NotNull(renderTexture);

        using var stagingTexture = m_RenderDevice.CreateTexture(new()
        {
            Name = "Staging buffer",
            Type = ResourceDimension.Tex2d,
            Width = RenderTargetWidth,
            Height = RenderTargetHeight,
            SampleCount = 1,
            Usage = Usage.Staging,
            Format = ColorBufferFormat,
            BindFlags = BindFlags.None,
            CPUAccessFlags = CpuAccessFlags.Read
        });
        Assert.NotNull(stagingTexture);

        using var depthTexture = m_RenderDevice.CreateTexture(new()
        {
            Name = "Depth buffer",
            Type = ResourceDimension.Tex2d,
            Width = RenderTargetWidth,
            Height = RenderTargetHeight,
            SampleCount = 1,
            Usage = Usage.Default,
            Format = DepthBufferFormat,
            BindFlags = BindFlags.DepthStencil
        });
        Assert.NotNull(depthTexture);

        var cubeVertices = new Vertex[] {
            new (new(-1, -1, -1), new(1, 0, 0, 1)),
            new (new(-1, +1, -1), new(0, 1, 0, 1)),
            new (new(+1, +1, -1), new(0, 0, 1, 1)),
            new (new(+1, -1, -1), new(1, 1, 1, 1)),

            new (new(-1, -1, +1), new(1, 1, 0, 1)),
            new (new(-1, +1, +1), new(0, 1, 1, 1)),
            new (new(+1, +1, +1), new(1, 0, 1, 1)),
            new (new(+1, -1, +1), new(0.2f, 0.2f, 0.2f, 1))
        };

        var cubeIndices = new uint[]
        {
            2,0,1, 2,3,0,
            4,6,5, 4,7,6,
            0,7,4, 0,3,7,
            1,0,4, 1,4,5,
            1,5,2, 5,6,2,
            3,6,7, 3,2,6
        };

        using var vertexBuffer = m_RenderDevice.CreateBuffer(new()
        {
            Name = "Cube vertex buffer",
            Usage = Usage.Immutable,
            BindFlags = BindFlags.VertexBuffer,
            Size = (ulong)(Unsafe.SizeOf<Vertex>() * cubeVertices.Length)
        }, cubeVertices);
        Assert.NotNull(vertexBuffer);

        using var indexBuffer = m_RenderDevice.CreateBuffer(new()
        {
            Name = "Cube index buffer",
            Usage = Usage.Immutable,
            BindFlags = BindFlags.IndexBuffer,
            Size = (ulong)(Unsafe.SizeOf<uint>() * cubeIndices.Length)
        }, cubeIndices);
        Assert.NotNull(indexBuffer);

        using var uniformBuffer = m_RenderDevice.CreateBuffer(new()
        {
            Name = "Uniform buffer",
            Size = (ulong)Unsafe.SizeOf<Matrix4x4>(),
            Usage = Usage.Dynamic,
            BindFlags = BindFlags.UniformBuffer,
            CPUAccessFlags = CpuAccessFlags.Write
        });
        Assert.NotNull(uniformBuffer);

        using var shaderSourceFactory = m_EngineFactory.CreateDefaultShaderSourceStreamFactory(PathToAssets);
        Assert.NotNull(shaderSourceFactory);

        using var vs = m_RenderDevice.CreateShader(new()
        {
            FilePath = "DotNetCube.vsh",
            ShaderSourceStreamFactory = shaderSourceFactory,
            Desc = new()
            {
                Name = "Cube vertex shader",
                ShaderType = ShaderType.Vertex,
                UseCombinedTextureSamplers = true
            },
            SourceLanguage = ShaderSourceLanguage.Hlsl
        }, out _);
        Assert.NotNull(vs);

        using var ps = m_RenderDevice.CreateShader(new()
        {
            FilePath = "DotNetCube.psh",
            ShaderSourceStreamFactory = shaderSourceFactory,
            Desc = new()
            {
                Name = "Cube pixel shader",
                ShaderType = ShaderType.Pixel,
                UseCombinedTextureSamplers = true
            },
            SourceLanguage = ShaderSourceLanguage.Hlsl
        }, out _);
        Assert.NotNull(ps);

        using var pipeline = m_RenderDevice.CreateGraphicsPipelineState(new ()
        {
            PSODesc = new() { Name = "Cube Graphics PSO" },
            Vs = vs,
            Ps = ps,
            GraphicsPipeline = new()
            {
                InputLayout = new()
                {
                    LayoutElements = new[]
                    {
                        new LayoutElement
                        {
                            InputIndex = 0,
                            NumComponents = 3,
                            ValueType = Diligent.Core.ValueType.Float32,
                            IsNormalized = false,
                        },
                        new LayoutElement
                        {
                            InputIndex = 1,
                            NumComponents = 4,
                            ValueType = Diligent.Core.ValueType.Float32,
                            IsNormalized = false,
                        }
                    }
                },
                PrimitiveTopology = PrimitiveTopology.TriangleList,
                RasterizerDesc = new() { CullMode = CullMode.Back },
                DepthStencilDesc = new() { DepthEnable = true },
                NumRenderTargets = 1,
                RTVFormats = new[] { TextureFormat.RGBA8_UNorm },
                DSVFormat = TextureFormat.D32_Float
            }
        });
        Assert.NotNull(pipeline);

        var variable = pipeline.GetStaticVariableByName(ShaderType.Vertex, "Constants");
        Assert.NotNull(variable);

        variable.Set(uniformBuffer, SetShaderResourceFlags.None);

        using var shaderResourceBinding = pipeline.CreateShaderResourceBinding(true);
        Assert.NotNull(shaderResourceBinding);

        var worldMatrix = Matrix4x4.CreateRotationY(MathF.PI / 4.0f) * Matrix4x4.CreateRotationX(-MathF.PI * 0.1f);
        var viewMatrix = Matrix4x4.CreateTranslation(0.0f, 0.0f, 5.0f);
        var projMatrix = CreatePerspectiveFieldOfView(MathF.PI / 4.0f, RenderTargetWidth / (float)RenderTargetHeight, 0.01f, 100.0f);
        var wvpMatrix = Matrix4x4.Transpose(worldMatrix * viewMatrix * projMatrix);

        var mapUniformBuffer = m_DeviceContext.MapBuffer<Matrix4x4>(uniformBuffer, MapType.Write, MapFlags.Discard);
        mapUniformBuffer[0] = wvpMatrix;
        m_DeviceContext.UnmapBuffer(uniformBuffer, MapType.Write);

        var rtv = renderTexture.GetDefaultView(TextureViewType.RenderTarget);
        var dsv = depthTexture.GetDefaultView(TextureViewType.DepthStencil);
        Assert.NotNull(rtv);
        Assert.NotNull(dsv);

        m_DeviceContext.SetRenderTargets(new[] { rtv }, dsv, ResourceStateTransitionMode.Transition);
        m_DeviceContext.ClearRenderTarget(rtv, new(0.35f, 0.35f, 0.35f, 1.0f), ResourceStateTransitionMode.Transition);
        m_DeviceContext.ClearDepthStencil(dsv, ClearDepthStencilFlags.Depth, 1.0f, 0, ResourceStateTransitionMode.Transition);
        m_DeviceContext.SetPipelineState(pipeline);
        m_DeviceContext.SetVertexBuffers(0, new[] { vertexBuffer }, new[] { 0ul }, ResourceStateTransitionMode.Transition);
        m_DeviceContext.SetIndexBuffer(indexBuffer, 0, ResourceStateTransitionMode.Transition);
        m_DeviceContext.CommitShaderResources(shaderResourceBinding, ResourceStateTransitionMode.Transition);
        m_DeviceContext.DrawIndexed(new()
        {
            IndexType = Diligent.Core.ValueType.UInt32,
            NumIndices = 36,
            Flags = DrawFlags.VerifyAll
        });
        m_DeviceContext.SetRenderTargets(null, null, ResourceStateTransitionMode.None);
        m_DeviceContext.CopyTexture(new()
        {
            SrcTexture = renderTexture,
            SrcTextureTransitionMode = ResourceStateTransitionMode.Transition,
            DstTexture = stagingTexture,
            DstTextureTransitionMode = ResourceStateTransitionMode.Transition
        });

        m_DeviceContext.WaitForIdle();
        var mapStagingTexture = m_DeviceContext.MapTextureSubresource(stagingTexture, 0, 0, MapType.Read, MapFlags.DoNotWait, null);
        CompareImages(mapStagingTexture, "./Assets/DotNetCubeTexture.png");
        m_DeviceContext.UnmapTextureSubresource(stagingTexture, 0, 0);
        m_DeviceContext.InvalidateState();
    }

    [Fact]
    public void DearchiveCubeGraphicsAssets()
    {
        using var dearchiver = m_EngineFactory.CreateDearchiver(new());
        Assert.NotNull(dearchiver);

        var binaryData = File.ReadAllBytes("Assets/DotNetArchive.bin");
        Assert.NotNull(binaryData);

        using var dataBlob = m_EngineFactory.CreateDataBlob(binaryData);
        Assert.NotNull(dataBlob);

        var result = dearchiver.LoadArchive(dataBlob, false);
        Assert.True(result);

        using var pipeline = dearchiver.UnpackPipelineState(new()
        {
            Device = m_RenderDevice,
            Name = "Cube Graphics PSO",
            PipelineType = PipelineType.Graphics,

        });
        Assert.NotNull(pipeline);
        Assert.Equal("Cube Graphics PSO", pipeline.GetDesc().Name);

        using var vs = dearchiver.UnpackShader(new()
        {
            Device = m_RenderDevice,
            Name = "Cube vertex shader",
        });
        Assert.NotNull(vs);
        Assert.Equal("Cube vertex shader", vs.GetDesc().Name);

        using var shaderModified = dearchiver.UnpackShader(new()
        {
            Device = m_RenderDevice,
            Name = "Cube pixel shader",
            ModifyShaderDesc = (ref ShaderDesc desc) =>
            {
                desc.Name = "ModifiedName";
            }
        });
        Assert.NotNull(shaderModified);
        Assert.Equal("ModifiedName", shaderModified.GetDesc().Name);

        using var pipelineModified = dearchiver.UnpackPipelineState(new()
        {
            Device = m_RenderDevice,
            Name = "Cube Graphics PSO",
            PipelineType = PipelineType.Graphics,
            ModifyPipelineStateCreateInfo = (createInfo) =>
            {
                createInfo.PSODesc.Name = "ModifiedName";
            }
        });
        Assert.NotNull(pipelineModified);
        Assert.Equal("ModifiedName", pipelineModified.GetDesc().Name);
    }

    [Fact]
    public void CheckBrokenShader()
    {
        const string shaderCode = @"
            struct PSInput
            {
                float4 Pos : SV_POSITION;
                float4 Color : COLOR0;
            };
            
            struct PSOutput
            {
                float4 Color : SV_TARGET;
            };
            
            PSOutput main(in PSInput PSIn)
            {
                //PSOut.Color = PSIn.Color;
            }
        ";

        using var shader = m_RenderDevice.CreateShader(new ()
        {
            Source = shaderCode,
            Desc = new()
            {
                Name = "Broken shader",
                ShaderType = ShaderType.Pixel,
                UseCombinedTextureSamplers = true
            },
            SourceLanguage = ShaderSourceLanguage.Hlsl
        },out var compilerError);
        Assert.Null(shader);
        Assert.Contains("Broken shader(13,25-42): error X3507: 'main': ", compilerError);
        m_Output.WriteLine(compilerError);
    }
}
