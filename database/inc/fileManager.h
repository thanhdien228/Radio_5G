#pragma once
#include "dataValue.h"
#include "dbException.h"
#include <gtest/gtest_prod.h>
#include <vector>
#include <regex>
#include <fstream>
#include <iostream>
#include <unordered_map>

/// @brief Number of matching group required when reading data from file using `regex`
constexpr size_t NUM_OF_MATCHES = 4;
class FileManager
{
private:
    FRIEND_TEST(FileManagerTest, fileManagerTest);
    std::string m_filePath;
    std::vector<std::string> m_keysOrder;
    std::unordered_map<std::string, DataValue> m_items;

public:
    /**
     * @brief Constructor of the class `FileManager`. The path file is compulsory.
     *
     * @param p_filePath The path to a file that needs to be read
     */
    FileManager(const std::string &p_filePath);

    /**
     * @brief Load the file in the given file path to memory
     *
     * @returns None
     *
     * @throws - `DBException(ExceptionType::CAN_NOT_OPEN_FILE_IN)` if the file can not be opened
     * for reading
     * @throws - `DBException(ExceptionType::INVALID_TYPE)` if the type of a record is invalid
     * @throws - `DBException(ExceptionType::INVALID_TYPE_VALUE)` if the type and value is not
     * compatible
     * @throws - `DBException(ExceptionType::BAD_FORMATTING_LINE)` if the current line can not be
     * parsed using `regex`
     */
    void loadFileToMemory();

    /**
     * @brief Save the file in the given file path to memory
     *
     * @returns None
     *
     * @throws `DBException(ExceptionType::CAN_NOT_OPEN_FILE_OUT)` if the file can not be opened
     * for writing
     */
    void saveMemoryToFile();

    /**
     * @brief Save the line with the given key in its position in the file
     *
     * @param p_key key of the record that need to be saved to file
     *
     * @returns None
     */
    void saveLine(const std::string &p_key);

    /**
     * @brief Get the string that represents the formatted record using for output to console
     *
     * @param p_key key of the record that need to be retrieved
     *
     * @returns A string that represents the whole record. It is in the format `key type value`.
     *
     * @throws `DBException(ExceptionType::KEY_NOT_FOUND)` if the given key is not found in the
     * database
     */
    std::string getByKey(const std::string &p_key);

    /**
     * @brief Get the real value of the record according to its type.
     *
     * @param p_key key of the record that need to be retrieved
     *
     * @returns An `std::variant` will be used for extracting the value by upper layer of the
     * database. It actually calls the `getValue()` function of the `DataValue` object, if the key
     * is found in the database.
     *
     * @throws `DBException(ExceptionType::KEY_NOT_FOUND)` if the given key is not found in the
     * database
     */
    std::variant<int, unsigned long, float, char const *> getValueByKey(const std::string &p_key);

    /**
     * @brief Modify the record with the given key, by the given type and value.
     *
     * @param p_key key of the record that need modifying
     * @param p_type new type of the record
     * @param p_value new value of the record
     *
     * @returns `true` if the modifcation is performed successfully. This function never return
     * false, but the `bool` return type is needed to break the loop for searching in multiple
     * files, because the database can contain many files.
     *
     * @throws `DBException(ExceptionType::KEY_NOT_FOUND)` if the given key is not found in the
     * database
     * @throws - `DBException(ExceptionType::INVALID_TYPE)` if the type of a record is invalid
     * @throws - `DBException(ExceptionType::INVALID_TYPE_VALUE)` if the type and value is not
     * compatible
     */
    bool modify(const std::string &p_key, const std::string &p_type, std::string &p_value);

    /**
     * @brief Modify the record with the given key, by the given type and value.
     *
     * @param p_key key of the record that need modifying
     * @param p_type new type of the record
     * @param p_value new value of the record
     *
     * @returns A `vector` of `string`, represents all the records loaded to the database. It is
     * important to notice that the `vector` can be an empty `vector` if there is no record loaded
     * to the database
     * */
    std::vector<std::string> getAll();
};
