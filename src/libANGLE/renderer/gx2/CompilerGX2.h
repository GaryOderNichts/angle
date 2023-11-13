#ifndef LIBANGLE_RENDERER_GX2_COMPILERGX2_H_
#define LIBANGLE_RENDERER_GX2_COMPILERGX2_H_

#include "libANGLE/renderer/CompilerImpl.h"

namespace rx
{

class CompilerGX2 : public CompilerImpl
{
  public:
    CompilerGX2();
    ~CompilerGX2() override;

    ShShaderOutput getTranslatorOutputType() const override;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_COMPILERGX2_H_
