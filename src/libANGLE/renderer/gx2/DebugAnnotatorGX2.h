#ifndef LIBANGLE_RENDERER_GX2_DEBUGANNOTATORGX2_H_
#define LIBANGLE_RENDERER_GX2_DEBUGANNOTATORGX2_H_

#include "libANGLE/LoggingAnnotator.h"

namespace rx
{

// This debug annotator only avoids logging to stdout twice, which the default
// LoggingAnnotator currently does for some reason
class DebugAnnotatorGX2 : public angle::LoggingAnnotator
{
  public:
    DebugAnnotatorGX2();
    ~DebugAnnotatorGX2() override;

    void logMessage(const gl::LogMessage &msg) const override;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_DEBUGANNOTATORGX2_H_
