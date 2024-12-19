#include "logException.h"    // Should be updated in the future

LogException::LogException(const LogExceptionType p_type)
{
    m_exceptionType = p_type;
}

char const *LogException::what() const noexcept
{
    switch (m_exceptionType)
    {
    case LogExceptionType::FAIL_TO_OPEN_FILE:
        return "Fail to open file.";
    case LogExceptionType::INVALID_LOG_FILE_EXTENSION:
        return "Invalid log file extension.";
    default:
        return "Unknown logger error.";
    }
}
