#pragma once
#include <stdint.h>

#if defined(__WUT__) || defined(__WIIU__)
#    include <gx2/shaders.h>

#    define GLSL_COMPILER_CAFE_RPL  // use compiler as rpl
#endif

enum GLSL_COMPILER_FLAG
{
    // reserved for future use
    GLSL_COMPILER_FLAG_NONE = 0,
    // write disassembly to stderr
    GLSL_COMPILER_FLAG_GENERATE_DISASSEMBLY = 1 << 0,
};

typedef GX2VertexShader *(*GLSL_CompileVertexShaderFn)(const char *shaderSource,
                                                       char *infoLogOut,
                                                       int infoLogMaxLength,
                                                       GLSL_COMPILER_FLAG flags);
typedef GX2PixelShader *(*GLSL_CompilePixelShaderFn)(const char *shaderSource,
                                                     char *infoLogOut,
                                                     int infoLogMaxLength,
                                                     GLSL_COMPILER_FLAG flags);
typedef void (*GLSL_FreeVertexShaderFn)(GX2VertexShader *shader);
typedef void (*GLSL_FreePixelShaderFn)(GX2PixelShader *shader);

#ifndef GLSL_COMPILER_CAFE_RPL
extern "C" {
void InitGLSLCompiler();
void DestroyGLSLCompiler();
GX2VertexShader *CompileVertexShader(const char *shaderSource,
                                     char *infoLogOut,
                                     int infoLogMaxLength,
                                     GLSL_COMPILER_FLAG flags);
GX2PixelShader *CompilePixelShader(const char *shaderSource,
                                   char *infoLogOut,
                                   int infoLogMaxLength,
                                   GLSL_COMPILER_FLAG flags);
void FreeVertexShader(GX2VertexShader *shader);
void FreePixelShader(GX2PixelShader *shader);
};
#endif

extern GLSL_CompileVertexShaderFn GLSL_CompileVertexShader;
extern GLSL_CompilePixelShaderFn GLSL_CompilePixelShader;
extern GLSL_FreeVertexShaderFn GLSL_FreeVertexShader;
extern GLSL_FreePixelShaderFn GLSL_FreePixelShader;

bool GLSL_Init();

bool GLSL_Shutdown();
