#ifndef LIBANGLE_RENDERER_GX2_PROGRAMEXECUTABLEGX2_H_
#define LIBANGLE_RENDERER_GX2_PROGRAMEXECUTABLEGX2_H_

#include "libANGLE/ProgramExecutable.h"
#include "libANGLE/renderer/ProgramExecutableImpl.h"
#include "libANGLE/renderer/gx2/gx2_buffer_utils.h"

#include <gx2/shaders.h>

namespace rx
{

struct DefaultUniformBlock : angle::NonCopyable
{
    DefaultUniformBlock()  = default;
    ~DefaultUniformBlock() = default;

    // Buffer containing the uniform block
    gx2::BufferHelper buffer;

    // This tells us where to write on a call to a setUniform method.
    std::map<GLint, GX2UniformVar> uniformVarLayout;
};

class ProgramExecutableGX2 : public ProgramExecutableImpl
{
  public:
    ProgramExecutableGX2(const gl::ProgramExecutable *executable, RendererGX2 *renderer);
    ~ProgramExecutableGX2() override;

    void destroy(const gl::Context *context) override;

    void setUniform1fv(GLint location, GLsizei count, const GLfloat *v) override;
    void setUniform2fv(GLint location, GLsizei count, const GLfloat *v) override;
    void setUniform3fv(GLint location, GLsizei count, const GLfloat *v) override;
    void setUniform4fv(GLint location, GLsizei count, const GLfloat *v) override;
    void setUniform1iv(GLint location, GLsizei count, const GLint *v) override;
    void setUniform2iv(GLint location, GLsizei count, const GLint *v) override;
    void setUniform3iv(GLint location, GLsizei count, const GLint *v) override;
    void setUniform4iv(GLint location, GLsizei count, const GLint *v) override;
    void setUniform1uiv(GLint location, GLsizei count, const GLuint *v) override;
    void setUniform2uiv(GLint location, GLsizei count, const GLuint *v) override;
    void setUniform3uiv(GLint location, GLsizei count, const GLuint *v) override;
    void setUniform4uiv(GLint location, GLsizei count, const GLuint *v) override;
    void setUniformMatrix2fv(GLint location,
                             GLsizei count,
                             GLboolean transpose,
                             const GLfloat *value) override;
    void setUniformMatrix3fv(GLint location,
                             GLsizei count,
                             GLboolean transpose,
                             const GLfloat *value) override;
    void setUniformMatrix4fv(GLint location,
                             GLsizei count,
                             GLboolean transpose,
                             const GLfloat *value) override;
    void setUniformMatrix2x3fv(GLint location,
                               GLsizei count,
                               GLboolean transpose,
                               const GLfloat *value) override;
    void setUniformMatrix3x2fv(GLint location,
                               GLsizei count,
                               GLboolean transpose,
                               const GLfloat *value) override;
    void setUniformMatrix2x4fv(GLint location,
                               GLsizei count,
                               GLboolean transpose,
                               const GLfloat *value) override;
    void setUniformMatrix4x2fv(GLint location,
                               GLsizei count,
                               GLboolean transpose,
                               const GLfloat *value) override;
    void setUniformMatrix3x4fv(GLint location,
                               GLsizei count,
                               GLboolean transpose,
                               const GLfloat *value) override;
    void setUniformMatrix4x3fv(GLint location,
                               GLsizei count,
                               GLboolean transpose,
                               const GLfloat *value) override;

    void getUniformfv(const gl::Context *context, GLint location, GLfloat *params) const override;
    void getUniformiv(const gl::Context *context, GLint location, GLint *params) const override;
    void getUniformuiv(const gl::Context *context, GLint location, GLuint *params) const override;

    void syncShaders(const gl::Context *context) const;

    void syncUniformBlocks(const gl::Context *context);
    void notifyDraw(const gl::Context *context);

  private:
    friend class ProgramGX2;

    angle::Result compileShaders(const gl::ShaderMap<std::string> &shaderSources,
                                 gl::InfoLog &infoLog);

    size_t getDefaultUniformBlockSize(gl::ShaderType shaderType) const;

    angle::Result initDefaultUniformBlocks();

    angle::Result initDefaultUniformBlockLayout();

    template <typename T>
    void setUniformImpl(GLint location, GLsizei count, const T *v, GLenum entryPointType);

    template <typename T>
    void getUniformImpl(GLint location, T *v, GLenum entryPointType) const;

    template <int cols, int rows>
    void setUniformMatrixfv(GLint location,
                            GLsizei count,
                            GLboolean transpose,
                            const GLfloat *value);

    RendererGX2 *const mRenderer;

    GX2VertexShader *mVertexShader;
    GX2PixelShader *mPixelShader;

    gl::ShaderMap<std::vector<GX2UniformVar>> mUniformVars;

    gl::ShaderMap<DefaultUniformBlock> mDefaultUniformBlocks;
    gl::ShaderBitSet mDefaultUniformBlocksDirty;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_PROGRAMEXECUTABLEGX2_H_
