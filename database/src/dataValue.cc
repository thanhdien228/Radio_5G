#include "../inc/dataValue.h" // Should be updated in the future
#include <iostream>

bool validateType(const std::string p_type)
{
    return VALID_TYPES.find(p_type) != VALID_TYPES.end();
}

void trimmingFloatString(std::string &p_value)
{
    p_value.erase(p_value.find_last_not_of('0') + 1, std::string::npos);
    p_value.erase(p_value.find_last_not_of('.') + 1, std::string::npos);
}

bool validateAndFormatData(const std::string &p_type, std::string &p_value)
{
    if ((p_type == "u32" || p_type == "s32") && p_value.find(" ") != std::string::npos)
    {
        return false;
    }
    if (p_type == "char")
    {
        return true;
    }
    else if (p_type == "s32")
    {
        try
        {
            p_value = std::to_string(std::stoi(p_value));
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    else if (p_type == "u32")
    {
        try
        {
            if (p_value[0] == '-')
            {
                return false;
            }
            p_value = std::to_string(std::stoul(p_value));
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    else if (p_type == "f32")
    {
        try
        {
            p_value = std::to_string(std::stof(p_value));
            trimmingFloatString(p_value);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    return false;
}

std::string DataValue::getDataValue()
{
    return m_type + " " + m_value;
}

void DataValue::setDataValue(const std::string &p_type, const std::string &p_value)
{
    m_type = p_type;
    m_value = p_value;
}

std::string DataValue::getTypeToSave()
{
    return m_type;
}

std::string DataValue::getValueToSave()
{
    return "\"" + m_value + "\"";
}

std::variant<int, unsigned long, float, char const *> DataValue::getValue()
{
    if (m_type == "char")
    {
        return m_value.c_str();
    }
    else if (m_type == "s32")
    {
        return std::stoi(m_value);
    }
    else if (m_type == "u32")
    {
        return std::stoul(m_value);
    }
    else if (m_type == "f32")
    {
        return std::stof(m_value);
    }
    else
    {
        throw DBException(ExceptionType::INVALID_TYPE);
    }
}
