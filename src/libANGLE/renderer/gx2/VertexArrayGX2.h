#ifndef LIBANGLE_RENDERER_GX2_VERTEXARRAYGX2_H_
#define LIBANGLE_RENDERER_GX2_VERTEXARRAYGX2_H_

#include "libANGLE/renderer/VertexArrayImpl.h"

#include <gx2/shaders.h>

namespace rx
{

class VertexArrayGX2 : public VertexArrayImpl
{
  public:
    VertexArrayGX2(const gl::VertexArrayState &data);
    ~VertexArrayGX2() override;

    void destroy(const gl::Context *context) override;

    angle::Result syncState(const gl::Context *context,
                            const gl::VertexArray::DirtyBits &dirtyBits,
                            gl::VertexArray::DirtyAttribBitsArray *attribBits,
                            gl::VertexArray::DirtyBindingBitsArray *bindingBits) override;

    angle::Result syncStateForDraw(const gl::Context *context,
                                   GLint firstVertex,
                                   GLsizei vertexOrIndexCount,
                                   GLsizei instanceCount,
                                   gl::DrawElementsType indexTypeOrInvalid,
                                   const void *indices);

    const gl::AttribArray<GX2AttribStream> &getAttribStreams() const { return mAttribStreams; }

  private:
    angle::Result syncDirtyElementArrayBufferData(const gl::Context *context);
    angle::Result syncDirtyAttrib(const gl::Context *context,
                                  const gl::VertexAttribute &attrib,
                                  const gl::VertexBinding &binding,
                                  size_t attribIndex,
                                  bool bufferOnly);
    angle::Result syncDirtyBufferData(const gl::Context *context, size_t bindingIndex);

    gl::AttribArray<GX2AttribStream> mAttribStreams;
    bool mAttribStreamDirty;

    GX2FetchShader mFetchShader;
    bool mHasFetchShader;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_VERTEXARRAYGX2_H_
