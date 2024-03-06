#include "GLSLCompiler.h"
#include <coreinit/dynload.h>
#include <stdio.h>

#ifdef GLSL_COMPILER_CAFE_RPL
OSDynLoad_Module s_glslCompilerModule = nullptr;
#endif

GLSL_CompileVertexShaderFn GLSL_CompileVertexShader;
GLSL_CompilePixelShaderFn GLSL_CompilePixelShader;
GLSL_FreeVertexShaderFn GLSL_FreeVertexShader;
GLSL_FreePixelShaderFn GLSL_FreePixelShader;
void (*__GLSL_DestroyGLSLCompiler)();

bool GLSL_Init()
{
    void (*_InitGLSLCompiler)() = nullptr;
#if defined(__WUT__) || defined(__WIIU__)
    // TODO FIXME we currently just return true for angle here since shutdown tends to fail
    if (s_glslCompilerModule != nullptr)
        return true;

    OSDynLoad_Error r = OSDynLoad_Acquire("glslcompiler", &s_glslCompilerModule);
    if (r != OS_DYNLOAD_OK)
    {
        // try alternate path
        r = OSDynLoad_Acquire("~/wiiu/libs/glslcompiler.rpl", &s_glslCompilerModule);
    }
    if (r != OS_DYNLOAD_OK)
    {
        // try MochaPayload path directly (for tiramisu)
        // TODO remove this eventually
        r = OSDynLoad_Acquire("~|wiiu|libs|glslcompiler.rpl", &s_glslCompilerModule);
    }
    if (r != OS_DYNLOAD_OK)
    {
        printf("glslcompiler.rpl not found\n");
        return false;
    }
    // find exports
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "InitGLSLCompiler",
                         (void **)&_InitGLSLCompiler);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "CompileVertexShader",
                         (void **)&GLSL_CompileVertexShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "CompilePixelShader",
                         (void **)&GLSL_CompilePixelShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "FreeVertexShader",
                         (void **)&GLSL_FreeVertexShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "FreePixelShader",
                         (void **)&GLSL_FreePixelShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "DestroyGLSLCompiler",
                         (void **)&__GLSL_DestroyGLSLCompiler);
#else
    _InitGLSLCompiler          = InitGLSLCompiler;
    GLSL_CompileVertexShader   = CompileVertexShader;
    GLSL_CompilePixelShader    = CompilePixelShader;
    GLSL_FreeVertexShader      = FreeVertexShader;
    GLSL_FreePixelShader       = FreePixelShader;
    __GLSL_DestroyGLSLCompiler = DestroyGLSLCompiler;
#endif
    _InitGLSLCompiler();
    return true;
}

bool GLSL_Shutdown()
{
#ifdef GLSL_COMPILER_CAFE_RPL
    if (s_glslCompilerModule == nullptr)
        return false;
    __GLSL_DestroyGLSLCompiler();
    OSDynLoad_Release(s_glslCompilerModule);
    s_glslCompilerModule = nullptr;
#else
    __GLSL_DestroyGLSLCompiler();
#endif
    return true;
}
