#include "logger.h" // Should be updated in the future
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cmath>

LogPriority Logger::m_priority = LogPriority::INFO;

Logger::Logger(const std::string &p_filename) : m_filePath(p_filename), m_saveLogToFile(true)
{
    if (m_filePath.substr(m_filePath.find_last_of(".")) != LOG_EXTENSION)
    {
        throw LogException(LogExceptionType::INVALID_LOG_FILE_EXTENSION);
    }
    m_logFile.open(m_filePath, std::ios::app);
    if (!m_logFile.is_open())
    {
        throw LogException(LogExceptionType::FAIL_TO_OPEN_FILE);
    }
}

Logger &Logger::operator=(const Logger &p_logger)
{
    this->m_filePath = p_logger.m_filePath;
    this->m_logFile.open(this->m_filePath, std::ios::app);
    return *this;
}

void Logger::close()
{
    if (m_logFile.is_open())
    {
        m_logFile.close();
    }
}

Logger::~Logger()
{
    close();
}

std::string Logger::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    auto nowInMs = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % (int)std::pow(BASE, NUM_OF_DIGITS_AFTER_DECIMAL);

    std::stringstream ss;
    ss << std::put_time(localtime(&nowTimeT), TIMESTAMP_FORMAT)
       << "." << std::setw(NUM_OF_DIGITS_AFTER_DECIMAL) << std::setfill('0') << nowInMs.count();
    return ss.str();
}

std::string Logger::logStr(const std::string &p_level, const std::string &p_message, const sourceInfo &p_location)
{
    std::stringstream ss;
    ss << "[" << getCurrentTime() << "] [" << p_level
       << "] {\"" << p_location.file_name() << ":" << p_location.line() << "\", \""
       << p_message << "\"}" << '\n';
    return ss.str();
}

void Logger::log(const std::string &p_level, const std::string &p_message, const sourceInfo &p_location)
{
    if (m_saveLogToFile == true)
    {
        m_logFile << logStr(p_level, p_message, p_location);
        m_logFile.flush();
    }
    else
    {
        std::cout << logStr(p_level, p_message, p_location);
    }
}

void Logger::enableLogFile(const bool &p_flag)
{
    m_saveLogToFile = p_flag;
}

void Logger::setPriority(LogPriority p_priority)
{
    Logger::m_priority = p_priority;
}

void Logger::trace(const std::string &p_message, const sourceInfo &p_location)
{
    log("TRACE", p_message, p_location);
}

void Logger::debug(const std::string &p_message, const sourceInfo &p_location)
{
    if (Logger::m_priority < LogPriority::DEBUG)
        return;
    log("DEBUG", p_message, p_location);
}

void Logger::info(const std::string &p_message, const sourceInfo &p_location)
{
    if (Logger::m_priority < LogPriority::INFO)
        return;
    log("INFO", p_message, p_location);
}

void Logger::warning(const std::string &p_message, const sourceInfo &p_location)
{
    if (Logger::m_priority < LogPriority::WARNING)
        return;
    log("WARNING", p_message, p_location);
}

void Logger::error(const std::string &p_message, const sourceInfo &p_location)
{
    if (Logger::m_priority < LogPriority::ERROR)
        return;
    log("ERROR", p_message, p_location);
}

void Logger::fatal(const std::string &p_message, const sourceInfo &p_location)
{
    if (Logger::m_priority < LogPriority::FATAL)
        return;
    log("FATAL", p_message, p_location);
}