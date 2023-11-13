#include "util/test_utils.h"

#include <coreinit/thread.h>
#include <coreinit/time.h>

namespace angle
{

const char *GetNativeEGLLibraryNameWithExtension()
{
    return "unknown_libegl";
}

void Sleep(unsigned int milliseconds)
{
    OSSleepTicks(OSMillisecondsToTicks(milliseconds));
}

void WriteDebugMessage(const char *format, ...)
{
    va_list vararg;
    va_start(vararg, format);
    vfprintf(stderr, format, vararg);
    va_end(vararg);
}

int NumberOfProcessors()
{
    return 3;
}

// bunch of stubs just to get tests running
void PrintStackBacktrace() {}

void InitCrashHandler(CrashCallback *callback) {}

void TerminateCrashHandler() {}

Process *LaunchProcess(const std::vector<const char *> &args, ProcessOutputCapture captureOutput)
{
    return nullptr;
}

bool DeleteSystemFile(const char *path)
{
    return false;
}

}  // namespace angle
