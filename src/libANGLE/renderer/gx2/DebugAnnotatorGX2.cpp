#include "libANGLE/renderer/gx2/DebugAnnotatorGX2.h"

namespace rx
{

DebugAnnotatorGX2::DebugAnnotatorGX2() {}

DebugAnnotatorGX2::~DebugAnnotatorGX2() {}

void DebugAnnotatorGX2::logMessage(const gl::LogMessage &msg) const
{
    gl::Trace(msg.getSeverity(), msg.getMessage().c_str());
}

}  // namespace rx
