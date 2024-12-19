#pragma once
#include "dbException.h"
#include <string>
#include <set>
#include <variant>

/**
 * @brief A set of valid types declared according to the database requirements, used for checking
 the validity when writing to database.
 * */
const std::set<std::string> VALID_TYPES = {"char", "u32", "s32", "f32"};

/**
 * @brief Helper function to check if the passed in `p_type` is valid or not, based on the
 * `VALID_TYPES` set
 *
 * @param p_type `std::string` represents the type that need checking
 *
 * @returns `true` or `false`
 */
bool validateType(const std::string p_type);

/**
 * @brief Helper function to check if the passed `p_type` and `p_value` are compatible with each
 * other. If they are compatible, the `p_value` will be formatted according to its type. For
 * example, if the input is `0001234` for type `u32`, it will be formatted to `1234`
 *
 * @param p_type the type that need checking
 * @param p_value the value that need checking
 * 
 * @returns `true` or `false`
 */
bool validateAndFormatData(const std::string &p_type, std::string &p_value);

class DataValue
{
private:
    std::string m_type;
    std::string m_value;

public:
    /**
     * @brief Default constructor for class `DataValue`
     */
    DataValue() = default;

    /**
     * @brief Default destructor for class `DataValue`
     */
    ~DataValue() = default;

    /**
     * @brief Get the type and value of this record for output to the console
     *
     * @return The formatted value that saved in the database, along with its type, for displaying
     * purpose
     */
    std::string getDataValue();

    /**
     * @brief Set the type and the value passed in to the current `DataValue`
     *
     * @param p_type the type that need to be updated
     * @param p_valye the value that need to be updated
     *
     * @returns None
     */
    void setDataValue(const std::string &p_type, const std::string &p_value);

    /**
     * @brief Extract the type of this `DataValue` object
     *
     * @returns The type of this `DataValue` object
     */
    std::string getTypeToSave();

    /**
     * @brief Extract the value of this `DataValue` object
     *
     * @returns The formatted value of this `DataValue` object, will be used for saving the record
     * back to file (the value is wrapped with double quotation marks "")
     */
    std::string getValueToSave();

    /**
     * @brief Return the real value of this `DataValue` object, according to its type
     *
     * @returns An `std::variant` will be used for extracting the value by upper layer of the
     * database
     */
    std::variant<int, unsigned long, float, char const *> getValue();
};
