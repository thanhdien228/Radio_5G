#pragma once
#include <dirent.h>
#include "fileManager.h"
#include "sys/stat.h"

/**
 * @brief Helper function that extract the value from the `std::variant` and store in the
 * `p_valueStore` variable.
 *
 * @tparam T the type of data that needs to be retrieved
 * @param p_var the `std::variant` that needs to be extracted from
 * @param p_valueStore: the reference to the variable that is intentedly used to store the exrtacted
 * value
 *
 * @returns None. If `std::bad_variant_access` is catched, it will log the error to the console.
 */
template <typename T>
void extractValue(std::variant<int, unsigned long, float, char const *> &p_var, T &p_valueStore);

extern template void extractValue<int>(std::variant<int, unsigned long, float, char const *> &p_var,
                                       int &p_valueStore);

extern template void extractValue<unsigned long>(std::variant<int, unsigned long, float,
                                                              char const *> &p_var,
                                                 unsigned long &p_valueStore);

extern template void extractValue<float>(std::variant<int, unsigned long, float,
                                                      char const *> &p_var,
                                         float &p_valueStore);

extern template void extractValue<char const *>(std::variant<int, unsigned long, float,
                                                             char const *> &p_var,
                                                char const *&p_valueStore);

class InMemDatabase
{
private:
    static InMemDatabase m_instance;
    std::vector<FileManager> m_fileManagers;

    /**
     * @brief Default constructor of the `InMemDatabase` class. It is set private to apply the
     * singleton pattern
     */
    InMemDatabase() = default;

    /**
     * @brief The copy constructor is deleted to make sure there are no other copies of the
     * database and preserve the singleton properties
     */
    InMemDatabase(const InMemDatabase &) = delete;

    /**
     * @brief The copy assignment is deleted to ensure there are no self-assignments happenned
     * intentively or accidentally in the program.
     */
    InMemDatabase &operator=(const InMemDatabase &) = delete;

    /**
     * @brief Default destructor of the `InMemDatabase` class.
     */
    ~InMemDatabase() = default;

public:
    /**
     * @brief The function that allows retrieving database singleton object. Should be used before
     * invoking any other methods.
     *
     * @note For example, if we want to invoke the `init` function, it should be:
     * `InMemDatabase::getInstance().init(path);`
     */
    static InMemDatabase &getInstance();

    /**
     * @brief Clear the current memory in the database and load the new data from the given path
     *
     * @param p_path the path to the directory that need to be loaded to the memory
     *
     * @returns None
     *
     * @throws - `DBException(ExceptionType::INVALID_DIR)` if the given directory is invalid
     * @throws - `DBException(ExceptionType::CAN_NOT_OPEN_DIR)` if the database can not open the
     * given directory
     *
     * @note It will also throw exceptions from the `FileManager.loadFileToMemory()`. However, all
     * the exceptions belongs to the `DBException` type; therefore, we have to catch and handle the
     * exceptions properly to prevent the program from termination.
     */
    void init(const std::string &p_path);

    /**
     * @brief Get the string represents the record of the given key. The string is formatted for
     * output to the console.
     *
     * @param p_key the key of the record that we need to search for
     *
     * @returns A string containing the whole record of the given key
     *
     * @throws - `DBException(ExceptionType::NO_KEY_PROVIDED)` if `p_key` is an empty string
     * @throws - `DBException(ExceptionType::KEY_NOT_FOUND)` if the given key is not found in the
     * database
     */
    std::string get(const std::string &p_key);

    /**
     * @brief Write the new value on the record with given `key`. If `isForce` is `true`, also
     * save that change on the corresponding lines.
     *
     * @param p_key the key of the record that we need to write on
     * @param p_type the new type for the record
     * @param p_value the new value for the record
     * @param p_isForce a boolean determine whether to save this change to the corresponding file
     * or not. By default, `p_isForce` is set to `false`
     *
     * @returns None
     *
     * @throws - `DBException(ExceptionType::KEY_NOT_FOUND)` if the given key is not found in the
     * database
     * @throws - `DBException(ExceptionType::INVALID_TYPE)` if the type of a record is invalid
     * @throws - `DBException(ExceptionType::INVALID_TYPE_VALUE)` if the type and value is not
     * compatible
     */
    void modify(const std::string &p_key, const std::string &p_type, std::string &p_value,
                const bool p_isForce = false);

    /**
     * @brief Save all changes made to memory to corresponding files.
     *
     * @returns none
     *
     * @throws `DBException(ExceptionType::CAN_NOT_OPEN_FILE_OUT)` if the file can not be opened
     * for writing
     */
    void save();

    /**
     * @brief Retrieve all the records loaded on the database.
     *
     * @returns A `std::vector` of `std::string` represent all the records loaded to the database
     */
    std::vector<std::string> getAll();

    /**
     * @brief Retrieve the real value of the record.
     *
     * @param p_key the key of the record that need to get
     *
     * @returns An `std::variant` will be used for extracting the value, using the `extractValue()`
     * helper function.
     *
     * @throws `DBException(ExceptionType::KEY_NOT_FOUND)` if the given key is not found in the
     * database
     *
     * @note In order to retrieve the real value and store it to a variable, we can use something
     * like this:
     * 
     *     `std::string exampleKey = "exampleKey";`
     *     
     *     `const char* toStore;`
     *     
     *     `extractValue(InMemDatabase::getInstance().getValue(exampleKey), toStore)`
     */
    std::variant<int, unsigned long, float, char const *> getValue(const std::string &p_key);
};
