#pragma once
#include <iostream>
#include <fstream>
#include <experimental/source_location>
#include <sstream>
#include "logException.h"

/// @brief The information (file, line, location) of the executed function
using sourceInfo = std::experimental::source_location;

/// @brief The number of digits after decimal on the timestamp (display to microseconds)
constexpr int NUM_OF_DIGITS_AFTER_DECIMAL = 6;

/// @brief The base of exponential function 
constexpr int BASE = 10;

/// @brief The timestamp format in log messages
constexpr char TIMESTAMP_FORMAT[] = "%Y-%m-%d %H:%M:%S";

/// @brief The expected extension for log file
constexpr char LOG_EXTENSION[] = ".log";

/// @brief The list of level using for log messages
enum class LogPriority
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

/**
 *  @brief Class for Logging.
 */
class Logger
{
public:
    /// @brief The default constructor of the Logger class
    Logger() = default;
    /**
     * @brief Constructor to create exclusive log file for each Logger
     *
     * @param p_filename - name of output log file
     */
    Logger(const std::string &p_filename);

    /**
     * @brief Constructor to move logger file
     *
     * @param p_filename - name of output log file
     */
    Logger &operator=(const Logger &p_logger);

    /**
     * @brief Destructor to close log file
     */
    ~Logger();

    /**
     * @brief Set `m_saveLogToFile` to new boolean value
     *
     * @param p_flag - boolean value which assigned to `m_saveLogToFile`
     */
    void enableLogFile(const bool &p_flag);

    /**
     * @brief Set `m_priority` of all Logger instances to this LogPriority enum value
     *
     * @param p_priority - LogPriority value which assigned to `m_priority`
     */
    static void setPriority(LogPriority p_priority);

    /**
     * @brief Close log file
     */
    void close();

    /**
     * @brief Print log file at specific log priority (TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)
     *
     * @param p_message - multiple arguments that need to add to log message
     * @param p_location - current file and line as default parameter
     */
    void trace(const std::string &p_message, const sourceInfo &p_location = sourceInfo::current());
    void debug(const std::string &p_message, const sourceInfo &p_location = sourceInfo::current());
    void info(const std::string &p_message, const sourceInfo &p_location = sourceInfo::current());
    void warning(const std::string &p_message, const sourceInfo &p_location = sourceInfo::current());
    void error(const std::string &p_message, const sourceInfo &p_location = sourceInfo::current());
    void fatal(const std::string &p_message, const sourceInfo &p_location = sourceInfo::current());

private:
    std::ofstream m_logFile;
    std::string m_filePath;
    bool m_saveLogToFile;
    static LogPriority m_priority;

    /**
     * @brief Return the current time stamp based on `TIMESTAMP_FORMAT`
     */
    std::string getCurrentTime();
    /**
     * @brief Return the log string based on log priority (level), added arguments, and current source location
     * 
     * @param p_level - log priority value as UPPERCASE STRING
     * @param p_message - multiple arguments that need to add to log message
     * @param p_location - current file and line as default parameter
     */
    std::string logStr(const std::string &p_level, const std::string &p_message, const sourceInfo &p_location);
    /**
     * @brief Check flag and mutex to print out the log message as specific level, as like an interface
     * 
     * @param p_level - log priority value as UPPERCASE STRING
     * @param p_message - multiple arguments that need to add to log message
     * @param p_location - current file and line as default parameter
     */
    void log(const std::string &p_level, const std::string &p_message, const sourceInfo &p_location);
};

/**
 * @brief Stringify containers type with has begin() & end() method, act as overloading function
 * 
 * @param p_container - container variable
 */
template <class Container, class Iter = decltype(cbegin(std::declval<Container>()))> // Shouldn't name class Iterator to avoid redefinition
std::string toString(const Container &p_container);

/**
 * @brief Stringify pair container, act as overloading function
 * 
 * @param p_pair - pair variable
 */
template <class Arg1, class Arg2>
std::string toString(const std::pair<Arg1, Arg2> &p_pair);

/**
 * @brief Stringify tuple container, act as overloading function
 * 
 * @param p_tuple - tuple variable with multiple arguments
 */
template <class... Args>
std::string toString(std::tuple<Args...> &p_tuple);

/**
 * @brief Stringify C++ type with stringstream could print, act as overloading function
 * 
 * @param p_arg - container variable
 */
template <class Arg, class = decltype(std::declval<std::stringstream>() << std::declval<Arg>())>
std::string toString(Arg &p_arg)
{
    std::stringstream sstr;
    sstr << p_arg;
    return sstr.str();
}


template <class... Args>
std::string toString(std::tuple<Args...> &p_tuple)
{
    const auto stringComma = [](const auto &arg)
    {
        return toString(arg) + ", ";
    };

    const auto stringCommaWithMultipleArgs = [&](const auto &...args)
    { 
        return (stringComma(args) + ...);
    };

    std::string str = std::apply(stringCommaWithMultipleArgs, p_tuple);
    return '(' + str.substr(0, str.size() - 2) + ')';
}

template <class Arg1, class Arg2>
std::string toString(const std::pair<Arg1, Arg2> &p_pair)
{
    return '(' + toString(p_pair.first) + ", " + toString(p_pair.second) + ')';
}

template <class Iter>
std::string toString(Iter p_iterBegin, Iter p_iterEnd)
{
    std::string str{'['};
    while (p_iterBegin != p_iterEnd)
    {
        str += toString(*p_iterBegin);
        if (++p_iterBegin != p_iterEnd)
        {
            str += ", ";
        }
    }
    str += ']';
    return str;
}

template <class Container, class Iter>
std::string toString(const Container &p_container)
{
    return toString(cbegin(p_container), cend(p_container));
}

/**
 * @brief Stringify data & container types with have `toString()` method
 * 
 * @param p_args - multiple arguments (variadic arguments)
 */
template <typename... Args>
std::string stringify(Args &&...p_args)
{
    std::ostringstream oss;
    oss << std::boolalpha;
    (oss << ... << toString(p_args));
    return oss.str();
}