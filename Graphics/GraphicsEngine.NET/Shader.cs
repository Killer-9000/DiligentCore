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

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using SharpGen.Runtime;

namespace Diligent.Core;

public partial struct ShaderMacro
{
    public ShaderMacro(string name, string definition)
    {
        Name = name;
        Definition = definition;
    }
}

public partial class ShaderCreateInfo
{
    public string FilePath;
    public IShaderSourceInputStreamFactory ShaderSourceStreamFactory;
    public string Source;
    public byte[] ByteCode;
    public string EntryPoint;
    public ShaderMacro[] Macros;
    public ShaderDesc Desc;
    public ShaderSourceLanguage SourceLanguage;
    public ShaderCompiler ShaderCompiler;
    public Version HLSLVersion;
    public Version GLSLVersion;
    public Version GLESSLVersion;
    public Version MSLVersion;
    public ShaderCompileFlags CompileFlags;
    public bool LoadConstantBufferReflection;

    public ShaderCreateInfo()
    {
        EntryPoint = "main";
        Desc = new ();
    }

    #region Marshal

    [StructLayout(LayoutKind.Sequential, Pack = 0)]
    internal unsafe struct __Native
    {
        public IntPtr FilePath;
        public IntPtr ShaderSourceStreamFactory;
        public IntPtr ConversionStream; // Not implemented
        public IntPtr Source;
        public IntPtr ByteCode;
        public PointerSize ByteCodeSize;
        public IntPtr EntryPoint;
        public ShaderMacro.__Native* Macros;
        public ShaderDesc.__Native Desc;
        public ShaderSourceLanguage SourceLanguage;
        public ShaderCompiler ShaderCompiler;
        public Version HLSLVersion;
        public Version GLSLVersion;
        public Version GLESSLVersion;
        public Version MSLVersion;
        public ShaderCompileFlags CompileFlags;
        public byte LoadConstantBufferReflection;
        public IntPtr* CompilerOutput;
    }

    internal unsafe void __MarshalFree(ref __Native @ref)
    {
        GC.KeepAlive(ShaderSourceStreamFactory);
        Marshal.FreeHGlobal(@ref.FilePath);
        Marshal.FreeHGlobal(@ref.Source);
        NativeMemory.Free(@ref.ByteCode.ToPointer());
        Marshal.FreeHGlobal(@ref.EntryPoint);
        for (var i = 0; i < Macros?.Length; i++)
            Macros[i].__MarshalFree(ref @ref.Macros[i]);
        NativeMemory.Free(@ref.Macros);
        Desc.__MarshalFree(ref @ref.Desc);
        NativeMemory.Free(@ref.CompilerOutput);
    }

    internal unsafe void __MarshalTo(ref __Native @ref)
    {
        @ref.FilePath = Marshal.StringToHGlobalAnsi(FilePath);
        @ref.ShaderSourceStreamFactory = ShaderSourceStreamFactory?.NativePointer ?? IntPtr.Zero;
        @ref.ConversionStream = IntPtr.Zero;
        @ref.Source = Marshal.StringToHGlobalAnsi(Source);
        if (ByteCode != null)
        {
            var dataPtr = NativeMemory.Alloc((nuint)ByteCode.Length);
            MemoryHelpers.CopyMemory<byte>(dataPtr, ByteCode);
            @ref.ByteCodeSize = ByteCode.Length;
        }
        @ref.EntryPoint = Marshal.StringToHGlobalAnsi(EntryPoint);

        if (Macros != null)
        {
            var dstPtr = (ShaderMacro.__Native*)NativeMemory.AllocZeroed((nuint)(Unsafe.SizeOf<ShaderMacro.__Native>() * (Macros.Length + 1)));
            for (var i = 0; i < Macros.Length; i++)
                Macros[i].__MarshalTo(ref dstPtr[i]);
        }

        Desc.__MarshalTo(ref @ref.Desc);
        @ref.SourceLanguage = SourceLanguage;
        @ref.ShaderCompiler = ShaderCompiler;
        @ref.HLSLVersion = HLSLVersion;
        @ref.GLSLVersion = GLSLVersion;
        @ref.GLESSLVersion = GLESSLVersion;
        @ref.MSLVersion = MSLVersion;
        @ref.CompileFlags = CompileFlags;
        @ref.LoadConstantBufferReflection = Convert.ToByte(LoadConstantBufferReflection);
        @ref.CompilerOutput = (IntPtr*)NativeMemory.AllocZeroed((nuint)Unsafe.SizeOf<IntPtr>());
    }

    #endregion
}

public partial class IShader
{
    public unsafe ReadOnlySpan<byte> GetBytecode()
    {
        void* dataPtr;
        ulong dataSize;
        GetBytecode(&dataPtr, &dataSize);
        return new ReadOnlySpan<byte>(dataPtr, (int)dataSize);
    }
}
