#ifndef ZEAL_LOG_H_HEADER_GUARD
#define ZEAL_LOG_H_HEADER_GUARD

#include <stdarg.h>

namespace zeal
{
/// Enumerates log levels.
///
/// @ingroup Device
struct LogSeverity
{
    enum Enum
    {
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,

        COUNT
    };
};

namespace log_internal
{
struct System
{
    const char *name;
};

void vlogx(LogSeverity::Enum sev, System system, const char *msg, va_list args);
void logx(LogSeverity::Enum sev, System system, const char *msg, ...);

} // namespace log_internal

} // namespace zeal

#define vlogi(system, msg, va_list) \
    zeal::log_internal::vlogx(zeal::LogSeverity::LOG_INFO, system, msg, va_list)
#define vloge(system, msg, va_list) \
    zeal::log_internal::vlogx(zeal::LogSeverity::LOG_ERROR, system, msg, va_list)
#define vlogw(system, msg, va_list) \
    zeal::log_internal::vlogx(zeal::LogSeverity::LOG_WARN, system, msg, va_list)
#define logi(system, msg, ...)                                         \
    zeal::log_internal::logx(zeal::LogSeverity::LOG_INFO, system, msg, \
                             ##__VA_ARGS__)
#define loge(system, msg, ...)                                          \
    zeal::log_internal::logx(zeal::LogSeverity::LOG_ERROR, system, msg, \
                             ##__VA_ARGS__)
#define logw(system, msg, ...)                                         \
    zeal::log_internal::logx(zeal::LogSeverity::LOG_WARN, system, msg, \
                             ##__VA_ARGS__)

#define LOG_SYSTEM(ident, name)                      \
    namespace                                        \
    {                                                \
    const zeal::log_internal::System ident = {name}; \
    }

#endif // ZEAL_LOG_H_HEADER_GUARD