#pragma once
#include <exception>
#include <string>

/**
 * List of possible exception types may occur during the performance of the database
 */
enum class ExceptionType
{
    /// @brief The initialization of the database can not be completed properly
    FAIL_TO_INIT_DB,
    /// @brief The given directory is invalid
    INVALID_DIR,
    /// @brief The database can not open the given directory for some reasons
    CAN_NOT_OPEN_DIR,
    /// @brief Can not open a file in input mode
    CAN_NOT_OPEN_FILE_IN,
    /// @brief Can not open a file in output mode
    CAN_NOT_OPEN_FILE_OUT,
    /// @brief Bad formatting line during the initialization of the database
    BAD_FORMATTING_LINE,
    /// @brief Key is not found in the database
    KEY_NOT_FOUND,
    /// @brief The given type is not contained in the `VALID_TYPES` set
    INVALID_TYPE,
    /// @brief The given type and value is not compatible with each other
    INVALID_TYPE_VALUE,
    /// @brief No key was given when using the get by key function of the database
    NO_KEY_PROVIDED,
};

class DBException : public std::exception
{
public:
    ExceptionType m_exceptionType;

    /**
     * @brief Default constructor for class `DBException`
     */
    DBException() = default;

    /**
     * @brief Default destructor for class `DBException`
     */
    ~DBException() = default;

    /**
     * @brief Constructor with a given type from the enum class `ExceptionType`
     *
     * @param p_type The type belongs to the enum class `ExceptionType`
     */
    DBException(const ExceptionType p_type);

    /**
     * @brief Override function of the exception base class that provides the content of exception
     * message
     */
    const char *what() const noexcept override;
};
