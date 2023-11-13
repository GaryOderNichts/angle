#include "gpu_info_util/SystemInfo_internal.h"

namespace angle
{

bool GetSystemInfo(SystemInfo *info)
{
    // TODO stub
    info->machineModelVersion = "0.0";
    GPUDeviceInfo deviceInfo;
    info->gpus.push_back(deviceInfo);

    return true;
}

}  // namespace angle
