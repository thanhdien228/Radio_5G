#include "../inc/dbException.h" // Should be updated in the future

DBException::DBException(const ExceptionType p_type)
{
    m_exceptionType = p_type;
}

const char* DBException::what() const noexcept
{
    switch (m_exceptionType)
    {
    case ExceptionType::FAIL_TO_INIT_DB:
        return "Fail to init database.";
    case ExceptionType::INVALID_DIR:
        return "Invalid directory.";
    case ExceptionType::CAN_NOT_OPEN_DIR:
        return "Can not open the given directory.";
    case ExceptionType::CAN_NOT_OPEN_FILE_IN:
        return "Can not open file from the given path to input.";
    case ExceptionType::CAN_NOT_OPEN_FILE_OUT:
        return "Can not open file from the given path to output.";
    case ExceptionType::BAD_FORMATTING_LINE:
        return "Bad formatting line.";
    case ExceptionType::KEY_NOT_FOUND:
        return "Key not found in database.";
    case ExceptionType::INVALID_TYPE:
        return "Type not supported.";
    case ExceptionType::INVALID_TYPE_VALUE:
        return "Invalid value with the given type.";
    case ExceptionType::NO_KEY_PROVIDED:
        return "No key provided.";
    default:
        return "Unknown database error.";
    }
}
