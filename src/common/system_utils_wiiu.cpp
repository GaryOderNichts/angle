#include "system_utils.h"

#include <sys/stat.h>
#include <unistd.h>

#include <coreinit/time.h>

namespace angle
{

std::string GetExecutablePath()
{
    return "";
}

std::string GetExecutableDirectory()
{
    return "/vol/external01/wiiu/angle/";
}

std::string GetModuleDirectory()
{
    return "";
}

const char *GetSharedLibraryExtension()
{
    return "";
}

const char *GetExecutableExtension()
{
    return "";
}

char GetPathSeparator()
{
    return '/';
}

Optional<std::string> GetCWD()
{
    std::array<char, 4096> pathBuf;
    char *result = getcwd(pathBuf.data(), pathBuf.size());
    if (result == nullptr)
    {
        return Optional<std::string>::Invalid();
    }
    return std::string(pathBuf.data());
}

bool SetCWD(const char *dirName)
{
    return (chdir(dirName) == 0);
}

bool SetEnvironmentVar(const char *variableName, const char *value)
{
    return false;
}

bool UnsetEnvironmentVar(const char *variableName)
{
    return false;
}

std::string GetEnvironmentVar(const char *variableName)
{
    return "";
}

const char *GetPathSeparatorForEnvironmentVar()
{
    return ":";
}

Optional<std::string> GetTempDirectory()
{
    return Optional<std::string>::Invalid();
}

Optional<std::string> CreateTemporaryFileInDirectory(const std::string &directory)
{
    return Optional<std::string>::Invalid();
}

double GetCurrentSystemTime()
{
    return static_cast<double>(OSTicksToNanoseconds(OSGetSystemTime())) * 1e-9;
}

double GetCurrentProcessCpuTime()
{
    return 0.0;
}

void *OpenSystemLibraryWithExtensionAndGetError(const char *libraryName,
                                                SearchType searchType,
                                                std::string *errorOut)
{
    return nullptr;
}

void CloseSystemLibrary(void *libraryHandle) {}

bool IsDebuggerAttached()
{
    return false;
}

void BreakDebugger()
{
    abort();
}

bool IsDirectory(const char *filename)
{
    struct stat st;
    int result = stat(filename, &st);
    return result == 0 && ((st.st_mode & S_IFDIR) == S_IFDIR);
}

}  // namespace angle
