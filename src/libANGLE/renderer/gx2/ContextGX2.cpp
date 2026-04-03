#include "libANGLE/renderer/gx2/ContextGX2.h"

#include "libANGLE/Display.h"
#include "libANGLE/renderer/gx2/BufferGX2.h"
#include "libANGLE/renderer/gx2/CompilerGX2.h"
#include "libANGLE/renderer/gx2/FramebufferGX2.h"
#include "libANGLE/renderer/gx2/ProgramExecutableGX2.h"
#include "libANGLE/renderer/gx2/ProgramGX2.h"
#include "libANGLE/renderer/gx2/RenderTargetGX2.h"
#include "libANGLE/renderer/gx2/RenderbufferGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"
#include "libANGLE/renderer/gx2/SamplerGX2.h"
#include "libANGLE/renderer/gx2/ShaderGX2.h"
#include "libANGLE/renderer/gx2/TextureGX2.h"
#include "libANGLE/renderer/gx2/TransformFeedbackGX2.h"
#include "libANGLE/renderer/gx2/VertexArrayGX2.h"
#include "libANGLE/renderer/gx2/gx2_common.h"
#include "libANGLE/renderer/gx2/gx2_utils.h"

#include <malloc.h>  // for memalign

#include <gx2/context.h>
#include <gx2/display.h>
#include <gx2/draw.h>
#include <gx2/enum.h>
#include <gx2/mem.h>
#include <gx2/registers.h>

namespace rx
{

ContextGX2::ContextGX2(const gl::State &state, gl::ErrorSet *errorSet, RendererGX2 *renderer)
    : ContextImpl(state, errorSet),
      MultisampleTextureInitializer(),
      mCaps(),
      mTextureCaps(),
      mExtensions(),
      mLimitations(),
      mPLSOptions(),
      mRenderer(renderer),
      mInternalDirtyBits(),
      mViewportRect(),
      mNearZ(),
      mFarZ(),
      mScissorRect(),
      mCullFront(),
      mCullBack(),
      mFrontFace(),
      mPolygonOffsetEnable(),
      mPolygonOffset(),
      mPolygonScale(),
      mPolygonClamp(),
      mDepthTest(),
      mDepthWrite(),
      mDepthCompare(),
      mBlendEnabled(),
      mColorSrcBlend(),
      mColorDstBlend(),
      mColorCombine(),
      mAlphaSrcBlend(),
      mAlphaDstBlend(),
      mAlphaCombine(),
      mBlendColor(),
      mIncompleteTextures(),
      mDirtyDefaultAttribsMask(),
      mDefaultAttribsBuffer(),
      mContextState()
{
    // TODO these are just copied from the null backend and should be checked and adjusted
    mExtensions                               = gl::Extensions();
    mExtensions.copyCompressedTextureCHROMIUM = true;
    mExtensions.copyTextureCHROMIUM           = true;
    mExtensions.debugMarkerEXT                = true;
    mExtensions.drawBuffersIndexedOES         = true;
    mExtensions.fenceNV                       = true;
    mExtensions.framebufferBlitANGLE          = true;
    mExtensions.framebufferBlitNV             = true;
    mExtensions.instancedArraysANGLE          = true;
    mExtensions.instancedArraysEXT            = true;
    mExtensions.mapBufferRangeEXT             = true;
    mExtensions.mapbufferOES                  = true;
    mExtensions.pixelBufferObjectNV           = true;
    mExtensions.shaderPixelLocalStorageANGLE  = state.getClientVersion() >= gl::Version(3, 0);
    mExtensions.shaderPixelLocalStorageCoherentANGLE = mExtensions.shaderPixelLocalStorageANGLE;
    mExtensions.textureRectangleANGLE                = true;
    mExtensions.textureUsageANGLE                    = true;
    mExtensions.translatedShaderSourceANGLE          = true;
    mExtensions.vertexArrayObjectOES                 = true;

    mExtensions.textureStorageEXT               = true;
    mExtensions.rgb8Rgba8OES                    = true;
    mExtensions.textureCompressionDxt1EXT       = true;
    mExtensions.textureCompressionDxt3ANGLE     = true;
    mExtensions.textureCompressionDxt5ANGLE     = true;
    mExtensions.textureCompressionS3tcSrgbEXT   = true;
    mExtensions.textureCompressionAstcHdrKHR    = true;
    mExtensions.textureCompressionAstcLdrKHR    = true;
    mExtensions.textureCompressionAstcOES       = true;
    mExtensions.compressedETC1RGB8TextureOES    = true;
    mExtensions.compressedETC1RGB8SubTextureEXT = true;
    mExtensions.lossyEtcDecodeANGLE             = true;
    mExtensions.geometryShaderEXT               = true;
    mExtensions.geometryShaderOES               = true;
    mExtensions.multiDrawIndirectEXT            = true;

    mExtensions.EGLImageOES                 = true;
    mExtensions.EGLImageExternalOES         = true;
    mExtensions.EGLImageExternalEssl3OES    = true;
    mExtensions.EGLImageArrayEXT            = true;
    mExtensions.EGLStreamConsumerExternalNV = true;

    const gl::Version maxClientVersion(3, 2);
    mCaps = GenerateMinimumCaps(maxClientVersion, mExtensions);

    // TODO
    mCaps.maxVertexUniformVectors = 4096;

    // TODO Verify that this is the max texture size for GX2
    mCaps.max2DTextureSize = 8192;

    // TODO
    mExtensions.textureFilterAnisotropicEXT = true;
    mCaps.maxTextureAnisotropy              = 1.0f;

    InitMinimumTextureCapsMap(maxClientVersion, mExtensions, &mTextureCaps);

    if (mExtensions.shaderPixelLocalStorageANGLE)
    {
        mPLSOptions.type             = ShPixelLocalStorageType::FramebufferFetch;
        mPLSOptions.fragmentSyncType = ShFragmentSynchronizationType::Automatic;
    }
}

ContextGX2::~ContextGX2() {}

void ContextGX2::onDestroy(const gl::Context *context)
{
    if (mDefaultAttribsBuffer)
    {
        free(mDefaultAttribsBuffer);
        mDefaultAttribsBuffer = nullptr;
    }

    free(mContextState);
    mContextState = nullptr;
}

angle::Result ContextGX2::initialize(const angle::ImageLoadContext &imageLoadContext)
{
    mContextState = static_cast<GX2ContextState *>(
        memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState)));
    ASSERT(mContextState != nullptr);

    // TODO check impact of profiling enabled
    GX2SetupContextStateEx(mContextState, TRUE);

    return angle::Result::Continue;
}

angle::Result ContextGX2::flush(const gl::Context *context)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::finish(const gl::Context *context)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::drawArrays(const gl::Context *context,
                                     gl::PrimitiveMode mode,
                                     GLint first,
                                     GLsizei count)
{
    return drawArraysImpl(context, mode, first, count, 1);
}

angle::Result ContextGX2::drawArraysInstanced(const gl::Context *context,
                                              gl::PrimitiveMode mode,
                                              GLint first,
                                              GLsizei count,
                                              GLsizei instanceCount)
{
    return drawArraysImpl(context, mode, first, count, instanceCount);
}

angle::Result ContextGX2::drawArraysInstancedBaseInstance(const gl::Context *context,
                                                          gl::PrimitiveMode mode,
                                                          GLint first,
                                                          GLsizei count,
                                                          GLsizei instanceCount,
                                                          GLuint baseInstance)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::drawElements(const gl::Context *context,
                                       gl::PrimitiveMode mode,
                                       GLsizei count,
                                       gl::DrawElementsType type,
                                       const void *indices)
{
    return drawElementsImpl(context, mode, count, type, indices, 1, 0);
}

angle::Result ContextGX2::drawElementsBaseVertex(const gl::Context *context,
                                                 gl::PrimitiveMode mode,
                                                 GLsizei count,
                                                 gl::DrawElementsType type,
                                                 const void *indices,
                                                 GLint baseVertex)
{
    return drawElementsImpl(context, mode, count, type, indices, 1, baseVertex);
}

angle::Result ContextGX2::drawElementsInstanced(const gl::Context *context,
                                                gl::PrimitiveMode mode,
                                                GLsizei count,
                                                gl::DrawElementsType type,
                                                const void *indices,
                                                GLsizei instances)
{
    return drawElementsImpl(context, mode, count, type, indices, instances, 0);
}

angle::Result ContextGX2::drawElementsInstancedBaseVertex(const gl::Context *context,
                                                          gl::PrimitiveMode mode,
                                                          GLsizei count,
                                                          gl::DrawElementsType type,
                                                          const void *indices,
                                                          GLsizei instances,
                                                          GLint baseVertex)
{
    return drawElementsImpl(context, mode, count, type, indices, instances, baseVertex);
}

angle::Result ContextGX2::drawElementsInstancedBaseVertexBaseInstance(const gl::Context *context,
                                                                      gl::PrimitiveMode mode,
                                                                      GLsizei count,
                                                                      gl::DrawElementsType type,
                                                                      const void *indices,
                                                                      GLsizei instances,
                                                                      GLint baseVertex,
                                                                      GLuint baseInstance)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::drawRangeElements(const gl::Context *context,
                                            gl::PrimitiveMode mode,
                                            GLuint start,
                                            GLuint end,
                                            GLsizei count,
                                            gl::DrawElementsType type,
                                            const void *indices)
{
    return drawElements(context, mode, count, type, indices);
}

angle::Result ContextGX2::drawRangeElementsBaseVertex(const gl::Context *context,
                                                      gl::PrimitiveMode mode,
                                                      GLuint start,
                                                      GLuint end,
                                                      GLsizei count,
                                                      gl::DrawElementsType type,
                                                      const void *indices,
                                                      GLint baseVertex)
{
    return drawElementsBaseVertex(context, mode, count, type, indices, baseVertex);
}

angle::Result ContextGX2::drawArraysIndirect(const gl::Context *context,
                                             gl::PrimitiveMode mode,
                                             const void *indirect)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::drawElementsIndirect(const gl::Context *context,
                                               gl::PrimitiveMode mode,
                                               gl::DrawElementsType type,
                                               const void *indirect)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawArrays(const gl::Context *context,
                                          gl::PrimitiveMode mode,
                                          const GLint *firsts,
                                          const GLsizei *counts,
                                          GLsizei drawcount)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawArraysInstanced(const gl::Context *context,
                                                   gl::PrimitiveMode mode,
                                                   const GLint *firsts,
                                                   const GLsizei *counts,
                                                   const GLsizei *instanceCounts,
                                                   GLsizei drawcount)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawArraysIndirect(const gl::Context *context,
                                                  gl::PrimitiveMode mode,
                                                  const void *indirect,
                                                  GLsizei drawcount,
                                                  GLsizei stride)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawElements(const gl::Context *context,
                                            gl::PrimitiveMode mode,
                                            const GLsizei *counts,
                                            gl::DrawElementsType type,
                                            const GLvoid *const *indices,
                                            GLsizei drawcount)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawElementsInstanced(const gl::Context *context,
                                                     gl::PrimitiveMode mode,
                                                     const GLsizei *counts,
                                                     gl::DrawElementsType type,
                                                     const GLvoid *const *indices,
                                                     const GLsizei *instanceCounts,
                                                     GLsizei drawcount)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawElementsIndirect(const gl::Context *context,
                                                    gl::PrimitiveMode mode,
                                                    gl::DrawElementsType type,
                                                    const void *indirect,
                                                    GLsizei drawcount,
                                                    GLsizei stride)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawArraysInstancedBaseInstance(const gl::Context *context,
                                                               gl::PrimitiveMode mode,
                                                               const GLint *firsts,
                                                               const GLsizei *counts,
                                                               const GLsizei *instanceCounts,
                                                               const GLuint *baseInstances,
                                                               GLsizei drawcount)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result ContextGX2::multiDrawElementsInstancedBaseVertexBaseInstance(
    const gl::Context *context,
    gl::PrimitiveMode mode,
    const GLsizei *counts,
    gl::DrawElementsType type,
    const GLvoid *const *indices,
    const GLsizei *instanceCounts,
    const GLint *baseVertices,
    const GLuint *baseInstances,
    GLsizei drawcount)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

gl::GraphicsResetStatus ContextGX2::getResetStatus()
{
    return gl::GraphicsResetStatus::NoError;
}

angle::Result ContextGX2::insertEventMarker(GLsizei length, const char *marker)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::pushGroupMarker(GLsizei length, const char *marker)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::popGroupMarker()
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::pushDebugGroup(const gl::Context *context,
                                         GLenum source,
                                         GLuint id,
                                         const std::string &message)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::popDebugGroup(const gl::Context *context)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::syncState(const gl::Context *context,
                                    const gl::state::DirtyBits dirtyBits,
                                    const gl::state::DirtyBits bitMask,
                                    const gl::state::ExtendedDirtyBits extendedDirtyBits,
                                    const gl::state::ExtendedDirtyBits extendedBitMask,
                                    gl::Command command)
{
    for (auto iter = dirtyBits.begin(), endIter = dirtyBits.end(); iter != endIter; ++iter)
    {
        size_t dirtyBit = *iter;
        switch (dirtyBit)
        {
            case gl::state::DIRTY_BIT_DRAW_FRAMEBUFFER_BINDING:
            {
                updateDrawFramebufferBinding(context);
                break;
            }
            case gl::state::DIRTY_BIT_READ_FRAMEBUFFER_BINDING:
            {
                // TODO
                UNIMPLEMENTED();
                break;
            }
            case gl::state::DIRTY_BIT_SCISSOR_TEST_ENABLED:
            case gl::state::DIRTY_BIT_SCISSOR:
            {
                updateScissor();
                break;
            }
            case gl::state::DIRTY_BIT_VIEWPORT:
            {
                updateViewport();
                // Need to recalculate scissor when updating the viewport
                updateScissor();
                break;
            }
            case gl::state::DIRTY_BIT_DEPTH_RANGE:
            {
                mNearZ = mState.getNearPlane();
                mFarZ  = mState.getFarPlane();

                mInternalDirtyBits.set(DIRTY_BIT_GX2_VIEWPORT);
                break;
            }
            case gl::state::DIRTY_BIT_BLEND_ENABLED:
            {
                mBlendEnabled = mState.getBlendState().blend;

                mInternalDirtyBits.set(DIRTY_BIT_GX2_COLOR_CONTROL);
                break;
            }
            case gl::state::DIRTY_BIT_BLEND_COLOR:
            {
                mBlendColor = mState.getBlendColor();

                mInternalDirtyBits.set(DIRTY_BIT_GX2_BLEND_COLOR);
                break;
            }
            case gl::state::DIRTY_BIT_BLEND_FUNCS:
            {
                // TODO ext blend state?
                const gl::BlendState &blendState = mState.getBlendState();

                mColorSrcBlend = gl_gx2::GetBlendMode(blendState.sourceBlendRGB);
                mColorDstBlend = gl_gx2::GetBlendMode(blendState.destBlendRGB);
                mAlphaSrcBlend = gl_gx2::GetBlendMode(blendState.sourceBlendAlpha);
                mAlphaDstBlend = gl_gx2::GetBlendMode(blendState.destBlendAlpha);

                mInternalDirtyBits.set(DIRTY_BIT_GX2_BLEND);
                break;
            }
            case gl::state::DIRTY_BIT_BLEND_EQUATIONS:
            {
                const gl::BlendState &blendState = mState.getBlendState();

                mColorCombine = gl_gx2::GetBlendCombineMode(blendState.blendEquationRGB);
                mAlphaCombine = gl_gx2::GetBlendCombineMode(blendState.blendEquationAlpha);

                mInternalDirtyBits.set(DIRTY_BIT_GX2_BLEND);
                break;
            }
            case gl::state::DIRTY_BIT_DEPTH_TEST_ENABLED:
            {
                mDepthTest = mState.getDepthStencilState().depthTest;

                mInternalDirtyBits.set(DIRTY_BIT_GX2_DEPTH_STENCIL);
                break;
            }
            case gl::state::DIRTY_BIT_DEPTH_FUNC:
            {
                mDepthCompare = gl_gx2::GetCompareFunction(mState.getDepthStencilState().depthFunc);

                mInternalDirtyBits.set(DIRTY_BIT_GX2_DEPTH_STENCIL);
                break;
            }
            case gl::state::DIRTY_BIT_DEPTH_MASK:
            {
                mDepthWrite = mState.getDepthStencilState().depthMask;

                mInternalDirtyBits.set(DIRTY_BIT_GX2_DEPTH_STENCIL);
                break;
            }
            case gl::state::DIRTY_BIT_CULL_FACE_ENABLED:
            case gl::state::DIRTY_BIT_CULL_FACE:
            {
                const gl::RasterizerState &rasterState = mState.getRasterizerState();

                gl_gx2::GetCullMode(rasterState, &mCullFront, &mCullBack);
                mFrontFace = gl_gx2::GetFrontFace(rasterState.frontFace);

                mInternalDirtyBits.set(DIRTY_BIT_GX2_POLYGON_CONTROL);
                break;
            }
            case gl::state::DIRTY_BIT_POLYGON_OFFSET_FILL_ENABLED:
            {
                mPolygonOffsetEnable = mState.isPolygonOffsetFillEnabled();

                mInternalDirtyBits.set(DIRTY_BIT_GX2_POLYGON_CONTROL);
                break;
            }
            case gl::state::DIRTY_BIT_POLYGON_OFFSET:
            {
                const gl::RasterizerState &rasterState = mState.getRasterizerState();

                mPolygonOffset = rasterState.polygonOffsetUnits;
                mPolygonScale  = rasterState.polygonOffsetFactor;
                mPolygonClamp  = rasterState.polygonOffsetClamp;

                mInternalDirtyBits.set(DIRTY_BIT_GX2_POLYGON_OFFSET);
                break;
            }
            case gl::state::DIRTY_BIT_CLEAR_COLOR:
                // Clear color is read from state upon framebuffer clear
                break;
            case gl::state::DIRTY_BIT_CLEAR_STENCIL:
                // Stencil is read from state upon framebuffer clear
                break;
            case gl::state::DIRTY_BIT_UNPACK_STATE:
                // Unpack state is handled with PixelUnpackState during setImage
                break;
            case gl::state::DIRTY_BIT_VERTEX_ARRAY_BINDING:
            {
                // Need to reset default attributes if vertex array bindings change
                invalidateDefaultAttribs(context->getActiveDefaultAttribsMask());
                break;
            }
            case gl::state::DIRTY_BIT_PROGRAM_BINDING:
                static_assert(
                    gl::state::DIRTY_BIT_PROGRAM_EXECUTABLE > gl::state::DIRTY_BIT_PROGRAM_BINDING,
                    "Dirty bit order");
                iter.setLaterBit(gl::state::DIRTY_BIT_PROGRAM_EXECUTABLE);
                break;
            case gl::state::DIRTY_BIT_PROGRAM_EXECUTABLE:
            {
                mInternalDirtyBits.set(DIRTY_BIT_GX2_SHADERS);
                break;
            }
            case gl::state::DIRTY_BIT_TEXTURE_BINDINGS:
            {
                updateTextureBindings(context);
                break;
            }
            case gl::state::DIRTY_BIT_CURRENT_VALUES:
            {
                invalidateDefaultAttribs(mState.getAndResetDirtyCurrentValues());
                break;
            }
            default:
            {
                WARN() << "\t! Unimplemented dirty bit: " << dirtyBit;
                ASSERT(NOASSERT_UNIMPLEMENTED);
                break;
            }
        }
    }

    return angle::Result::Continue;
}

GLint ContextGX2::getGPUDisjoint()
{
    return 0;
}

GLint64 ContextGX2::getTimestamp()
{
    return 0;
}

angle::Result ContextGX2::onMakeCurrent(const gl::Context *context)
{
    // Set the GX2 state associated with this context
    applyContextState();

    return angle::Result::Continue;
}

gl::Caps ContextGX2::getNativeCaps() const
{
    return mCaps;
}

const gl::TextureCapsMap &ContextGX2::getNativeTextureCaps() const
{
    return mTextureCaps;
}

const gl::Extensions &ContextGX2::getNativeExtensions() const
{
    return mExtensions;
}

const gl::Limitations &ContextGX2::getNativeLimitations() const
{
    return mLimitations;
}

const ShPixelLocalStorageOptions &ContextGX2::getNativePixelLocalStorageOptions() const
{
    return mPLSOptions;
}

CompilerImpl *ContextGX2::createCompiler()
{
    return new CompilerGX2();
}

ShaderImpl *ContextGX2::createShader(const gl::ShaderState &data)
{
    return new ShaderGX2(data);
}

ProgramImpl *ContextGX2::createProgram(const gl::ProgramState &data)
{
    return new ProgramGX2(data);
}

ProgramExecutableImpl *ContextGX2::createProgramExecutable(const gl::ProgramExecutable *executable)
{
    return new ProgramExecutableGX2(executable);
}

FramebufferImpl *ContextGX2::createFramebuffer(const gl::FramebufferState &data)
{
    return new FramebufferGX2(data, mRenderer);
}

TextureImpl *ContextGX2::createTexture(const gl::TextureState &state)
{
    return new TextureGX2(state, mRenderer);
}

RenderbufferImpl *ContextGX2::createRenderbuffer(const gl::RenderbufferState &state)
{
    return new RenderbufferGX2(state);
}

BufferImpl *ContextGX2::createBuffer(const gl::BufferState &state)
{
    return new BufferGX2(state);
}

VertexArrayImpl *ContextGX2::createVertexArray(const gl::VertexArrayState &data,
                                               const gl::VertexArrayBuffers &vertexArrayBuffers)
{
    return new VertexArrayGX2(data, vertexArrayBuffers);
}

QueryImpl *ContextGX2::createQuery(gl::QueryType type)
{
    return nullptr;
}

FenceNVImpl *ContextGX2::createFenceNV()
{
    return nullptr;
}

SyncImpl *ContextGX2::createSync()
{
    return nullptr;
}

TransformFeedbackImpl *ContextGX2::createTransformFeedback(const gl::TransformFeedbackState &state)
{
    return new TransformFeedbackGX2(state);
}

SamplerImpl *ContextGX2::createSampler(const gl::SamplerState &state)
{
    return new SamplerGX2(state);
}

ProgramPipelineImpl *ContextGX2::createProgramPipeline(const gl::ProgramPipelineState &state)
{
    return nullptr;
}

MemoryObjectImpl *ContextGX2::createMemoryObject()
{
    return nullptr;
}

SemaphoreImpl *ContextGX2::createSemaphore()
{
    return nullptr;
}

OverlayImpl *ContextGX2::createOverlay(const gl::OverlayState &state)
{
    return nullptr;
}

angle::Result ContextGX2::dispatchCompute(const gl::Context *context,
                                          GLuint numGroupsX,
                                          GLuint numGroupsY,
                                          GLuint numGroupsZ)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::dispatchComputeIndirect(const gl::Context *context, GLintptr indirect)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::memoryBarrier(const gl::Context *context, GLbitfield barriers)
{
    return angle::Result::Continue;
}

angle::Result ContextGX2::memoryBarrierByRegion(const gl::Context *context, GLbitfield barriers)
{
    return angle::Result::Continue;
}

void ContextGX2::handleError(GLenum errorCode,
                             const char *message,
                             const char *file,
                             const char *function,
                             unsigned int line)
{
    UNIMPLEMENTED();
}

angle::ImageLoadContext ContextGX2::getImageLoadContext() const
{
    return getRenderer()->getDisplay()->getImageLoadContext();
}

angle::Result ContextGX2::initializeMultisampleTextureToBlack(const gl::Context *context,
                                                              gl::Texture *glTexture)
{
    ASSERT(glTexture->getType() == gl::TextureType::_2DMultisample);
    TextureGX2 *textureGX2 = GetImplAs<TextureGX2>(glTexture);

    // TODO
    UNIMPLEMENTED();
}

void ContextGX2::applyContextState()
{
    if (mRenderer->inForeground())
    {
        GX2SetContextState(mContextState);
    }
}

angle::Result ContextGX2::updateState(const gl::Context *context)
{
    for (auto iter = mInternalDirtyBits.begin(), end = mInternalDirtyBits.end(); iter != end;
         ++iter)
    {
        mInternalDirtyBits.reset(*iter);
        switch (*iter)
        {
            case DIRTY_BIT_GX2_VIEWPORT:
                GX2SetViewport(mViewportRect.x, mViewportRect.y, mViewportRect.width,
                               mViewportRect.height, mNearZ, mFarZ);
                break;
            case DIRTY_BIT_GX2_SCISSOR:
                GX2SetScissor(mScissorRect.x, mScissorRect.y, mScissorRect.width,
                              mScissorRect.height);
                break;
            case DIRTY_BIT_GX2_SHADERS:
            {
                ProgramExecutableGX2 *programExecutableGX2 =
                    GetImplAs<ProgramExecutableGX2>(mState.getProgramExecutable());

                programExecutableGX2->syncShaders(context);
                break;
            }
            case DIRTY_BIT_GX2_POLYGON_CONTROL:
            {
                GX2SetPolygonControl(mFrontFace, mCullFront, mCullBack, mPolygonOffsetEnable,
                                     GX2_POLYGON_MODE_TRIANGLE, GX2_POLYGON_MODE_TRIANGLE,
                                     mPolygonOffsetEnable, mPolygonOffsetEnable, FALSE);
                break;
            }
            case DIRTY_BIT_GX2_POLYGON_OFFSET:
            {
                GX2SetPolygonOffset(mPolygonOffset, mPolygonScale, mPolygonOffset, mPolygonScale,
                                    mPolygonClamp);
                break;
            }
            case DIRTY_BIT_GX2_DEPTH_STENCIL:
            {
                // GX2SetDepthStencilControl();
                GX2SetDepthOnlyControl(mDepthTest, mDepthWrite, mDepthCompare);
                break;
            }
            case DIRTY_BIT_GX2_COLOR_CONTROL:
            {
                GX2SetColorControl(GX2_LOGIC_OP_COPY, mBlendEnabled ? 0xFF : 0, FALSE, TRUE);
                break;
            }
            case DIRTY_BIT_GX2_BLEND:
            {
                GX2SetBlendControl(GX2_RENDER_TARGET_0, mColorSrcBlend, mColorDstBlend,
                                   mColorCombine, TRUE, mAlphaSrcBlend, mAlphaDstBlend,
                                   mAlphaCombine);
                break;
            }
            case DIRTY_BIT_GX2_BLEND_COLOR:
            {
                GX2SetBlendConstantColor(mBlendColor.red, mBlendColor.green, mBlendColor.blue,
                                         mBlendColor.alpha);
                break;
            }
            case DIRTY_BIT_GX2_DEFAULT_ATTRIBS:
            {
                ANGLE_TRY(handleDirtyDefaultAttribs(context));
                break;
            }
        }
    }

    return angle::Result::Continue;
}

angle::Result ContextGX2::setupDraw(const gl::Context *context,
                                    gl::PrimitiveMode mode,
                                    GLint firstVertex,
                                    GLsizei vertexOrIndexCount,
                                    GLsizei instanceCount,
                                    gl::DrawElementsType indexTypeOrInvalid,
                                    const void *indices)
{
    applyContextState();
    mInternalDirtyBits.set(DIRTY_BIT_GX2_SHADERS);  // Urgh, how do we handle this instead of
                                                    // setting shaders for every draw/frame?
    updateState(context);

    VertexArrayGX2 *vaoGX2 = GetImplAs<VertexArrayGX2>(mState.getVertexArray());
    ProgramExecutableGX2 *programExecutableGX2 =
        GetImplAs<ProgramExecutableGX2>(mState.getProgramExecutable());

    programExecutableGX2->syncUniformBlocks(context);

    ANGLE_TRY(vaoGX2->syncStateForDraw(context, firstVertex, vertexOrIndexCount, instanceCount,
                                       indexTypeOrInvalid, indices));

    programExecutableGX2->notifyDraw(context);

    vaoGX2->notifyDraw(context);

    return angle::Result::Continue;
}

angle::Result ContextGX2::drawArraysImpl(const gl::Context *context,
                                         gl::PrimitiveMode mode,
                                         GLint first,
                                         GLsizei count,
                                         GLsizei instances)
{
    ANGLE_TRY(
        setupDraw(context, mode, first, count, 1, gl::DrawElementsType::InvalidEnum, nullptr));

    GX2DrawEx(gl_gx2::GetPrimitiveMode(mode), count, first, instances);
    return angle::Result::Continue;
}

angle::Result ContextGX2::drawElementsImpl(const gl::Context *context,
                                           gl::PrimitiveMode mode,
                                           GLsizei count,
                                           gl::DrawElementsType type,
                                           const void *indices,
                                           GLsizei instances,
                                           GLint baseVertex)
{
    ASSERT(instances > 0);

    ANGLE_TRY(setupDraw(context, mode, baseVertex, count, instances, type, indices));

    BufferGX2 *elementArrayBufferGX2 =
        SafeGetImplAs<BufferGX2>(mState.getVertexArray()->getElementArrayBuffer());

    if (elementArrayBufferGX2)
    {
        // TODO not sure if we should just add the offset to the index buffer
        // might mess with alignment?
        size_t offset = reinterpret_cast<size_t>(indices);

        elementArrayBufferGX2->onUsed(context);

        GX2DrawIndexedEx(gl_gx2::GetPrimitiveMode(mode), count, gl_gx2::GetIndexType(type),
                         elementArrayBufferGX2->getDataPtr() + offset, baseVertex, instances);
    }
    else
    {
        GX2DrawIndexedImmediateEx(gl_gx2::GetPrimitiveMode(mode), count, gl_gx2::GetIndexType(type),
                                  indices, baseVertex, instances);
    }
    return angle::Result::Continue;
}

void ContextGX2::updateDrawFramebufferBinding(const gl::Context *context)
{
    const gl::FramebufferAttachment *colorAttachment =
        mState.getDrawFramebuffer()->getFirstColorAttachment();
    if (colorAttachment)
    {
        RenderTargetGX2 *renderTarget = nullptr;
        colorAttachment->getRenderTarget(context, colorAttachment->getRenderToTextureSamples(),
                                         &renderTarget);
        ASSERT(renderTarget != nullptr);

        ColorRenderTargetGX2 *colorTarget = GetAs<ColorRenderTargetGX2>(renderTarget);
        GX2SetColorBuffer(colorTarget->getColorBuffer(), GX2_RENDER_TARGET_0);
    }

    const gl::FramebufferAttachment *depthStencilAttachment =
        mState.getDrawFramebuffer()->getDepthOrStencilAttachment();
    if (depthStencilAttachment)
    {
        RenderTargetGX2 *renderTarget = nullptr;
        depthStencilAttachment->getRenderTarget(context, 0, &renderTarget);
        ASSERT(renderTarget != nullptr);

        DepthStencilRenderTargetGX2 *depthStencilTarget =
            GetAs<DepthStencilRenderTargetGX2>(renderTarget);
        GX2SetDepthBuffer(depthStencilTarget->getDepthBuffer());
    }

    // Recalculate viewport and scissor
    updateViewport();
    updateScissor();
}

void ContextGX2::updateViewport()
{
    // TODO clamp viewport?
    // TODO flip Y?
    mViewportRect = mState.getViewport();

    mInternalDirtyBits.set(DIRTY_BIT_GX2_VIEWPORT);
}

void ContextGX2::updateScissor()
{
    // Get the full render area
    const gl::Framebuffer *framebuffer  = mState.getDrawFramebuffer();
    const gl::Box framebufferDimensions = framebuffer->getState().getDimensions();
    gl::Rectangle renderArea{0, 0, framebufferDimensions.width, framebufferDimensions.height};

    // Clip viewport to render area
    gl::Rectangle viewportClippedRenderArea;
    if (!gl::ClipRectangle(renderArea, mState.getViewport(), &viewportClippedRenderArea))
    {
        viewportClippedRenderArea = gl::Rectangle();
    }

    // Get scissored area
    mScissorRect = ClipRectToScissor(getState(), viewportClippedRenderArea, false);

    // TODO flip Y?

    mInternalDirtyBits.set(DIRTY_BIT_GX2_SCISSOR);
}

void ContextGX2::updateTextureBindings(const gl::Context *context)
{
    const gl::ProgramExecutable *executable = mState.getProgramExecutable();
    ASSERT(executable);

    if (executable->hasTextures())
    {
        const gl::ActiveTexturesCache &textures        = mState.getActiveTexturesCache();
        const gl::ActiveTextureMask &activeTextures    = executable->getActiveSamplersMask();
        const gl::ActiveTextureTypeArray &textureTypes = executable->getActiveSamplerTypes();

        for (size_t textureUnit : activeTextures)
        {
            gl::Texture *texture = textures[textureUnit];

            // nullptr means incomplete texture
            if (texture == nullptr)
            {
                if (mIncompleteTextures.getIncompleteTexture(
                        context, textureTypes[textureUnit],
                        executable->getSamplerFormatForTextureUnitIndex(textureUnit), this,
                        &texture) != angle::Result::Continue)
                {
                    // Hmm this is not great
                    return;
                }
            }

            TextureGX2 *textureGX2 = GetImplAs<TextureGX2>(texture);

            GX2SetPixelTexture(textureGX2->getTexture(), textureUnit);
            GX2SetPixelSampler(textureGX2->getSampler(), textureUnit);
        }
    }
}

void ContextGX2::invalidateDefaultAttribs(const gl::AttributesMask &dirtyMask)
{
    if (dirtyMask.any())
    {
        mDirtyDefaultAttribsMask |= dirtyMask;
        mInternalDirtyBits.set(DIRTY_BIT_GX2_DEFAULT_ATTRIBS);
    }
}

angle::Result ContextGX2::handleDirtyDefaultAttribs(const gl::Context *context)
{
    // TODO handle if default attribs are still in use by GPU

    // Allocate default attribute buffer if it doesn't exist yet
    if (!mDefaultAttribsBuffer)
    {
        mDefaultAttribsBuffer = mRenderer->allocateMemory(
            GX2_VERTEX_BUFFER_ALIGNMENT, gx2::kDefaultAttributeSize * gl::MAX_VERTEX_ATTRIBS);
    }

    // Copy default values into buffer
    for (size_t attribIndex : mDirtyDefaultAttribsMask)
    {
        const gl::VertexAttribCurrentValueData &defaultValue =
            mState.getVertexAttribCurrentValues()[attribIndex];

        memcpy(static_cast<uint8_t *>(mDefaultAttribsBuffer) +
                   (attribIndex * gx2::kDefaultAttributeSize),
               &defaultValue.Values, gx2::kDefaultAttributeSize);
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, mDefaultAttribsBuffer,
                  gx2::kDefaultAttributeSize * gl::MAX_VERTEX_ATTRIBS);
    GX2SetAttribBuffer(gx2::kDefaultAttributesBuffer,
                       gx2::kDefaultAttributeSize * gl::MAX_VERTEX_ATTRIBS, 0,
                       mDefaultAttribsBuffer);

    mDirtyDefaultAttribsMask.reset();
    return angle::Result::Continue;
}

}  // namespace rx
