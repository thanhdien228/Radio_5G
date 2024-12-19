#include "../inc/commandLine.h" // Should be updated in the future
#include <csignal>

sig_atomic_t g_sigflag = 0;

const std::regex pattern(R"(^(/\S+) (\S+) (.+)$)");

void sighandler(int s)
{
    g_sigflag = 1;
}

bool CommandLineInterface::initDB(std::string const &p_path)
{
    try
    {
        InMemDatabase::getInstance().init(p_path);
        std::cout << "Load successfully" << std::endl;
        return 1;
    }
    catch (DBException e)
    {
        std::cout << e.what() << std::endl;
        return 0;
    }
}
bool CommandLineInterface::handleInitDB()
{
    std::string path;
    bool initDbSuccess = 0;
    while (true)
    {
        std::cout << "Your database path ('exit' if you want to stop): ";
        std::cin >> path;
        if (path == "exit")
            return 0;
        initDbSuccess = initDB(path);
        if (initDbSuccess == 1)
            break;
    }
    return 1;
}
std::optional<std::string> CommandLineInterface::getKey(const std::string &p_key)
{
    try
    {
        std::string result;
        result = InMemDatabase::getInstance().get(p_key);
        return result;
    }
    catch (DBException error)
    {
        std::cout << "Error: " << error.what() << std::endl;
        return std::nullopt;
    }
};

std::vector<std::string> CommandLineInterface::getAllKey()
{
    std::vector<std::string> result;
    try
    {
        result = InMemDatabase::getInstance().getAll();
        return result;
    }
    catch (DBException error)
    {
        std::cout << "Error: " << error.what() << std::endl;
        return result;
    }
};

void CommandLineInterface::writeKey(const std::string &p_key, const std::string &p_type, std::string &p_value,
                                    const bool &p_force)
{
    try
    {
        InMemDatabase::getInstance().modify(p_key, p_type, p_value, p_force);
        if (!p_force)
        {
            std::cout << "Successfully write new " << "\"" << p_key << " " << p_type << " "
                      << p_value << "\"" << " to DB on RAM." << std::endl;
        }
        else
        {
            std::cout << "Successfully write new " << "\"" << p_key << " " << p_type << " "
                      << p_value << "\"" << " to DB and saved to file." << std::endl;
        }
    }
    catch (DBException e)
    {
        std::cout << e.what() << std::endl;
    }
}

void CommandLineInterface::handleGetCommand(const std::string &p_key)
{
    if (p_key == "all")
    {
        std::vector<std::string> result = getAllKey();
        for (std::string item : result)
        {
            std::cout << item << std::endl;
        }
    }
    else
    {
        std::optional<std::string> result = getKey(p_key);
        if (result != std::nullopt)
        {
            std::cout << result << std::endl;
        }
    }
}

void CommandLineInterface::checkForceAndErase(std::string &p_input, bool &p_force)
{
    std::string str1 = "-f";
    size_t found = p_input.find(str1);
    if (found != std::string::npos)
    {
        p_input.erase(found - 1, 3);
        p_force = true;
    }
}

bool CommandLineInterface::handleWriteCommand(const std::string &newInput, const bool &p_force)
{
    std::stringstream newstrStream(newInput);
    std::string db, record, key, type, value;
    newstrStream >> db >> db;
    std::getline(newstrStream, record);
    record.erase(0, 1);
    std::smatch match;
    std::regex_search(record, match, pattern);
    if (match.size() != 4)
    {
        std::cerr << "Invalid format of record, must be db write [-f] <key> <type> <value>."
                  << std::endl;
        return 0;
    }
    else
    {
        key = match[1];
        type = match[2];
        value = match[3];
        writeKey(key, type, value, p_force);
    }
    return 1;
}

void CommandLineInterface::handleCommandLine()
{
    if (!handleInitDB())
        return;
    std::signal(SIGINT, sighandler);
    std::unique_ptr<char[], decltype(std::free) *> command(nullptr, std::free);
    while (true)
    {
        if (g_sigflag != 0)
        {
            std::cerr << "Signal!\n";
            g_sigflag = 0;
        }
        command.reset(readline("> "));
        if (command == nullptr)
        {
            std::cout << "Exiting..." << std::endl;
            break;
        }
        if (*command.get())
        {
            add_history(command.get());
        }

        std::string input = command.get();

        if (input == "" || input == "\n")
        {
            continue;
        }
        else if (input == "exit")
            return;
        else if (input == "clear")
        {
            system("clear");
            continue;
        }
        else if (input == "help")
        {
            std::cout << "Commands:" << std::endl;
            std::cout << "clear - clear the screen" << std::endl;
            std::cout << "help - show this help" << std::endl;
            std::cout << "db get <key> - get data by key" << std::endl;
            std::cout << "db write [-f] <key> <data-type> <value> - modify data by key" << std::endl;
            std::cout << "db get all - get all data" << std::endl;
            std::cout << "exit - exit the program" << std::endl;
            continue;
        }
        std::stringstream strStream(input);
        std::string query;
        strStream >> query;
        if (query != "db")
        {
            std::cout << "Invalid command, must have db at the begin\n";
            continue;
        }

        strStream >> query;

        if (query == "get")
        {
            std::string key;
            strStream >> key;
            handleGetCommand(key);
        }
        else if (query == "write")
        {
            bool force = false;
            checkForceAndErase(input, force);
            int checkWriteComand = handleWriteCommand(input, force);
            if (checkWriteComand == 0)
            {
                continue;
            }
        }
        else
            std::cout << "Invalid command - use help to list all the commands.\n";
    }
};