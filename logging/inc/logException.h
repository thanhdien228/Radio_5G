#pragma once

/*
 * This file is part of Logging package.
 *
 * Usage for LogException:
 * 1. Wrap sample code into try-catch like `catch (LogException error)`
 * 2. Check if console have printed any exceptions
 * 
 * This could missing some exceptions type, recommend using catch (...)
 * to catch unchecked exceptions in Logger.
 */

#include <exception>
#include <string>

enum class LogExceptionType
{
    /// @brief The initialization of the database can not be completed properly
    FAIL_TO_OPEN_FILE,
    /// @brief The given log extensions is not contained in the `VALID_EXTENSIONS = [".log"]` set
    INVALID_LOG_FILE_EXTENSION
};

/**
 *  @brief Class for all Logger exceptions.
 */
class LogException : public std::exception
{
public:
    LogExceptionType m_exceptionType;

    /**
     * @brief Constructor to create exclusive type for each LogException
     *
     * @param p_filename - name of output log file
     */
    LogException(const LogExceptionType p_type);
    
    /**
     * @brief Returns a C-style character string describing the general cause
     *  of the current error based on LogException.
     */
    const char *what() const noexcept override;
};
