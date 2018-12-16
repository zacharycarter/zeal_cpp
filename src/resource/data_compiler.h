#ifndef ZEAL_DATA_COMPILER_H_HEADER_GUARD
#define ZEAL_DATA_COMPILER_H_HEADER_GUARD

#include "core/containers/types.h"
#include "core/fs/file_monitor.h"
#include "core/fs/filesystem_disk.h"
#include "device/console_server.h"
#include "device/device_options.h"
#include "resource/types.h"
#include <setjmp.h>

namespace zeal
{
int main_data_compiler(const DeviceOptions &opts);

} // namespace zeal

#endif // ZEAL_DATA_COMPILER_H_HEADER_GUARD