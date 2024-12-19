#include "../inc/fileManager.h" // Should be updated in the future

FileManager::FileManager(const std::string &p_filePath) : m_filePath(p_filePath)
{
}

void FileManager::loadFileToMemory()
{
    std::ifstream file(m_filePath);
    if (!file.is_open())
    {
        throw DBException(ExceptionType::CAN_NOT_OPEN_FILE_IN);
    }
    std::regex pattern(R"(^(/\S+) (\S+) \"(.+)\"$)");
    m_items.clear();
    m_keysOrder.clear();
    std::string line;
    while (getline(file, line))
    {
        std::smatch match;
        if (regex_search(line, match, pattern) && match.size() == NUM_OF_MATCHES)
        {
            std::string key = match[1];
            std::string type = match[2];
            std::string value = match[3];
            if (!validateType(type))
            {
                throw DBException(ExceptionType::INVALID_TYPE);
            }
            if (!validateAndFormatData(type, value))
            {
                throw DBException(ExceptionType::INVALID_TYPE_VALUE);
            }
            DataValue dataValue;
            dataValue.setDataValue(type, value);
            m_items[key] = dataValue;
            m_keysOrder.push_back(key);
        }
        else if (line == "" || line == "\n")
        {
            m_keysOrder.push_back("");
            continue;
        }
        else
        {
            throw DBException(ExceptionType::BAD_FORMATTING_LINE);
        }
    }
    file.close();
}

void FileManager::saveMemoryToFile()
{
    std::ofstream file(m_filePath);
    if (!file.is_open())
    {
        throw DBException(ExceptionType::CAN_NOT_OPEN_FILE_OUT);
    }
    for (std::string key : m_keysOrder)
    {
        if (key == "")
        {
            file << std::endl;
            continue;
        }
        DataValue dataValue = m_items[key];
        std::string record = key + " " + dataValue.getTypeToSave() + " " + dataValue.getValueToSave();
        file << record << std::endl;
    }
    file.close();
}

void FileManager::saveLine(const std::string &key)
{
    size_t lineToSaveIndex;
    for (size_t i = 0; i < m_keysOrder.size(); ++i)
    {
        if (m_keysOrder[i] == key)
        {
            lineToSaveIndex = i;
            break;
        }
    }
    DataValue dataValue = m_items[key];
    std::string lineToSave = key + " " + dataValue.getTypeToSave() + " " + dataValue.getValueToSave();
    std::ifstream inputFile(m_filePath);
    if (!inputFile.is_open())
    {
        std::cerr << "Error opening file for reading!" << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line))
    {
        lines.push_back(line);
    }
    inputFile.close();

    if (lineToSaveIndex < 0 || lineToSaveIndex >= lines.size())
    {
        std::cerr << "Line number out of range!" << std::endl;
        return;
    }

    lines[lineToSaveIndex] = lineToSave;

    // Write all lines back to the file
    std::ofstream outputFile(m_filePath);
    if (!outputFile.is_open())
    {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    for (const auto &l : lines)
    {
        outputFile << l << '\n';
    }
    outputFile.close();
}

std::string FileManager::getByKey(const std::string &key)
{
    if (m_items.find(key) == m_items.end())
    {
        return "";
    }
    DataValue dataValue = m_items[key];
    return key + " " + dataValue.getDataValue();
}

std::variant<int, unsigned long, float, char const *>
FileManager::getValueByKey(const std::string &p_key)
{
    if (m_items.find(p_key) == m_items.end())
    {
        throw DBException(ExceptionType::KEY_NOT_FOUND);
    }
    try
    {
        return m_items[p_key].getValue();
    }
    catch (DBException e)
    {
        throw;
    }
}

bool FileManager::modify(const std::string &key, const std::string &type, std::string &value)
{
    if (m_items.find(key) == m_items.end())
    {
        throw DBException(ExceptionType::KEY_NOT_FOUND);
    }
    if (!validateType(type))
    {
        throw DBException(ExceptionType::INVALID_TYPE);
    }
    if (!validateAndFormatData(type, value))
    {
        throw DBException(ExceptionType::INVALID_TYPE_VALUE);
    }
    DataValue dataValue;
    dataValue.setDataValue(type, value);
    m_items[key] = dataValue;
    return true;
}

std::vector<std::string> FileManager::getAll()
{
    std::vector<std::string> result;
    for (std::string key : m_keysOrder)
    {
        if (key == "")
        {
            continue;
        }
        DataValue dataValue = m_items[key];
        result.push_back(key + " " + dataValue.getDataValue());
    }
    return result;
}
