#include "../inc/inMemDatabase.h" // Should be updated in the future

template <typename T>
void extractValue(std::variant<int, unsigned long, float, char const *> &var, T &valueStore)
{
    try
    {
        valueStore = std::get<T>(var);
    }
    catch (std::bad_variant_access ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}

template void extractValue<int>(std::variant<int, unsigned long, float, char const *> &var,
                                int &valueStore);

template void extractValue<unsigned long>(std::variant<int, unsigned long, float,
                                                       char const *> &var,
                                          unsigned long &valueStore);

template void extractValue<float>(std::variant<int, unsigned long, float, char const *> &var,
                                  float &valueStore);

template void extractValue<char const *>(std::variant<int, unsigned long, float,
                                                      char const *> &var,
                                         char const *&valueStore);

InMemDatabase &InMemDatabase::getInstance()
{
    static InMemDatabase m_instance;
    return m_instance;
}

void InMemDatabase::init(const std::string &path)
{
    struct stat statBuffer;
    if (stat(path.c_str(), &statBuffer) != 0)
    {
        throw DBException(ExceptionType::INVALID_DIR);
    }
    m_fileManagers.clear();
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != nullptr)
    {
        while ((ent = readdir(dir)) != nullptr)
        {
            if (ent->d_type == DT_REG)
            {
                std::string filePath = path + "/" + ent->d_name;
                m_fileManagers.push_back(FileManager(filePath));
                try
                {
                    m_fileManagers.back().loadFileToMemory();
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        closedir(dir);
    }
    else
    {
        throw DBException(ExceptionType::CAN_NOT_OPEN_DIR);
    }
}

std::string InMemDatabase::get(const std::string &key)
{
    if (key == "")
    {
        throw DBException(ExceptionType::NO_KEY_PROVIDED);
    }
    for (FileManager &fm : m_fileManagers)
    {
        std::string result = fm.getByKey(key);
        if (result != "") {
            return result;
        }
    }
    throw DBException(ExceptionType::KEY_NOT_FOUND);
}

void InMemDatabase::modify(const std::string &key, const std::string &type, std::string &value,
                           const bool isForce)
{
    size_t count = 0;
    size_t size = m_fileManagers.size();
    for (FileManager &fm : m_fileManagers)
    {
        try
        {
            count++;
            if (fm.modify(key, type, value))
            {
                if (isForce)
                {
                    fm.saveLine(key);
                }
                return;
            }
        }
        catch (DBException e)
        {
            if (e.m_exceptionType == ExceptionType::INVALID_TYPE ||
                e.m_exceptionType == ExceptionType::INVALID_TYPE_VALUE)
            {
                throw;
            }
            else if (count == size && e.m_exceptionType == ExceptionType::KEY_NOT_FOUND)
            {
                throw DBException(ExceptionType::KEY_NOT_FOUND);
            }
        }
        catch (...)
        {
            throw;
        }
    }
}

void InMemDatabase::save()
{
    try
    {
        for (FileManager fm : m_fileManagers)
        {
            fm.saveMemoryToFile();
        }
    }
    catch (DBException e)
    {
        throw;
    }
}

std::vector<std::string> InMemDatabase::getAll()
{
    std::vector<std::string> all;
    for (FileManager fm : m_fileManagers)
    {
        std::vector<std::string> temp = fm.getAll();
        all.insert(all.end(), temp.begin(), temp.end());
    }
    return all;
}

std::variant<int, unsigned long, float, char const *> InMemDatabase::getValue(const std::string &key)
{
    for (FileManager &fm : m_fileManagers)
    {
        try
        {
            return fm.getValueByKey(key);
        }
        catch (DBException e)
        {
            if (e.m_exceptionType != ExceptionType::KEY_NOT_FOUND)
            {
                throw;
            }
        }
    }
    throw DBException(ExceptionType::KEY_NOT_FOUND);
}
