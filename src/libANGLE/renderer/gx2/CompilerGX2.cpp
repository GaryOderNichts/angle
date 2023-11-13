#include "libANGLE/renderer/gx2/CompilerGX2.h"

namespace rx
{

CompilerGX2::CompilerGX2() : CompilerImpl() {}

CompilerGX2::~CompilerGX2() {}

ShShaderOutput CompilerGX2::getTranslatorOutputType() const
{
    // TODO figure out what works best with whatever mesa based shader compiler we get
    return SH_GLSL_450_CORE_OUTPUT;
}

}  // namespace rx
