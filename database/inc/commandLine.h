#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "inMemDatabase.h"
#include <csignal>
#include <readline/readline.h>
#include <readline/history.h>
#include <optional>
#include <memory>

/**
 * @brief Operator "<<" to cout value of optional data type
 *
 * @param os Output stream.
 * @param opt optional<T> to write out.
 * @return Reference to the output stream.
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, std::optional<T> const &opt)
{
    return opt ? os << opt.value() : os;
}

class CommandLineInterface
{
private:
    std::vector<std::string> listCommand;

public:
    CommandLineInterface() = default;
    ~CommandLineInterface() = default;
    /**
     * @brief Call and return status when call a service to initialize Database, catch error
     *
     * @param path - path of database
     * @return 0 if initialize unsuccessfull or 1 if initialize successfully
     */
    bool initDB(std::string const &p_path);

    /**
     * @brief Handle process when user types input, supporting type again when user type a wrong path
     *
     * @param p_path - path of database
     * @return 1 if user type correct path and initialize successfully or 0 if user want to exit program
     */
    bool handleInitDB();

    /**
     * @brief Handle input path, support user type again if they type a invalid path,
     *
     *  call service to get key and catch error
     * @return the value of key of throw an error if user type incorrectly
     */
    std::optional<std::string> getKey(const std::string &p_key);

    /**
     * @brief Call service to get all keys and catch error
     *
     * @return value of the input key
     */
    std::vector<std::string> getAllKey();

    /**
     * @brief Call service to write keys and catch error
     *
     * @param p_key key of data user want to write
     * @param p_type data type of data user want to write
     * @param p_value value of data user want to write
     * @param p_force boolean variable to decide this is a force command or not
     */
    void writeKey(const std::string &p_key, const std::string &p_type, std::string &p_value, const bool &p_force);

    /**
     * @brief Handle get one key or get all keys
     *
     * @param p_key - key user want to get value or "all" to get all values
     */
    void handleGetCommand(const std::string &p_key);

    /**
     * @brief Split input command to seperate value at pass them to the modify service,
     *
     * assign flag value to true if force = 1, check format of input command
     * @param p_key - key user want to write value and force flag
     * @return  1 if write successfully or 0 if command is invalid format
     */
    bool handleWriteCommand(const std::string &p_key, const bool &p_force);

    /**
     * @brief Check force flag from command, assign that value to p_force and erase
     *
     * "-f" to split token easily
     * @param p_input input command
     * @param p_force force variable
     */
    void checkForceAndErase(std::string &p_input, bool &p_force);

    /**
     * @brief Handle input and call the suitable function
     */
    void handleCommandLine();
};
