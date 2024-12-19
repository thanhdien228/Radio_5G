#include "client.h"

const std::regex recordPattern(R"(^\s*(/\S+)\s*(\S+)\s*(.+)$)");

void initLogger()
{
    g_clientLogger.enableLogFile(true);
    g_clientLogger.setPriority(LogPriority::ERROR);
}

std::string checkForceAndErase(const std::string &p_input, bool &p_force)
{
    std::string tmpInput(p_input);
    std::string forceFlag = "-f";
    size_t found = p_input.find(forceFlag);
    if (found != std::string::npos)
    {
        tmpInput.erase(found, 2);
        p_force = true;
    }
    tmpInput = std::regex_replace(tmpInput, std::regex(" +$"), "");
    return tmpInput;
}

void handleGetCommand(const std::string &p_key)
{
    if (p_key == "all")
    {
        std::vector<std::string> result = InMemDatabase::getInstance().getAll();
        for (std::string item : result)
        {
            std::cout << item << std::endl;
        }
    }
    else
    {
        try
        {
            std::string result = InMemDatabase::getInstance().get(p_key);
            std::cout << result << std::endl;
        }
        catch (DBException e)
        {
            g_clientLogger.error(e.what());
        }
    }
}

void handleWriteCommand(const std::string &p_input)
{
    bool isForce = false;
    std::string processedInput = checkForceAndErase(p_input, isForce);
    std::string key, type, value;
    std::smatch match;
    std::regex_search(processedInput, match, recordPattern);
    if (match.size() != 4)
    {
        std::cerr << "Invalid format of record, must be db write [-f] <key> <type> <value>."
                  << std::endl;
        return;
    }
    else
    {
        key = match[1];
        type = match[2];
        value = match[3];
        try
        {
            InMemDatabase::getInstance().modify(key, type, value, isForce);
            if (isForce)
            {
                g_clientLogger.info(stringify("Successfully write new value of ", key, " on disk"));
            }
            else
            {
                g_clientLogger.info(stringify("Successfully write new value of ", key, " on RAM"));
            }
        }
        catch (DBException e)
        {
            g_clientLogger.error(e.what());
        }
    }
}

Client::Client()
    : serverIp(SERVER_IP), serverPort(SERVER_PORT), socketFd(-1), running(false), connected(false)
{
    initLogger();
}

void Client::connectToServer()
{
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        g_clientLogger.error("Create socket failed");
        return;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0)
    {
        g_clientLogger.error("Failed to create struct sockaddr_in serverAddr");
        close(socketFd);
        socketFd = -1;
        return;
    }

    if (connect(socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == 0)
    {
        g_clientLogger.info(stringify("Connected to server at ", serverIp, ":", serverPort));
        connected = true;
        setSocketNonblocking(socketFd); // Optional: Make socket non-blocking
    }
    else
    {
        g_clientLogger.error("Failed to connect to server");
        close(socketFd);
        socketFd = -1;
    }
}

void Client::reconnectToServer()
{
    closeConnection();
    g_clientLogger.info("Attempting to reconnect...");
    connectToServer();
    if (connected && !communicationThread.joinable())
    {
        communicationThread = std::thread(&Client::communicationLoop, this);
    }
}

void Client::setSocketNonblocking(int p_fd)
{
    int flags = fcntl(p_fd, F_GETFL, 0);
    if (flags == -1)
    {
        g_clientLogger.error("Failed to get file descriptor flags");
        return;
    }

    if (fcntl(p_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        g_clientLogger.error("Failed to set file descriptor flags with non-blocking");
    }
}

void Client::communicationLoop()
{
    char buffer[BUFFER_SIZE];

    while (connected)
    {
        // Receive message from server
        int bytesReceived = recv(socketFd, buffer, BUFFER_SIZE - 1, 0);

        if (bytesReceived > 0)
        {
            if (buffer[bytesReceived - 1] == '\n') {
                buffer[bytesReceived - 1] = '\0';
            }
            buffer[bytesReceived] = '\0';
            std::cout << buffer << std::endl;
            g_clientLogger.info(stringify("Message received: ", buffer));
        }
        else if (bytesReceived == 0)
        {
            g_clientLogger.info("Lose connection with server");
            connected = false;
            break;
        }
        else if (errno != EWOULDBLOCK && errno != EAGAIN) // Non-blocking mode check
        {
            perror("recv");
            connected = false;
            break;
        }

        // Avoid busy-waiting in non-blocking mode
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Client::sendMessage(const std::string &p_message)
{
    // Attempt to reconnect if not connected
    if (!connected)
    {
        g_clientLogger.info("Client is not connected to server");
        reconnectToServer();
        if (!connected)
        {
            g_clientLogger.error("Can not connect to server");
            return;
        }
    }

    // Send message
    if (send(socketFd, p_message.c_str(), p_message.size(), 0) == -1)
    {
        perror("send");
    }
}

void Client::closeConnection()
{
    connected = false;
    if (socketFd != -1)
    {
        close(socketFd);
        socketFd = -1;
        g_clientLogger.info("Disconnected from server");
    }
    if (communicationThread.joinable())
    {
        communicationThread.join();
    }
}

void Client::loadDatabase()
{
    std::string path = "./db";
    try
    {
        InMemDatabase::getInstance().init(path);
        g_clientLogger.info("Load database successfully from default path: " + path);
    }
    catch (DBException e)
    {
        g_clientLogger.error(stringify("Error when loading database: ", e.what(),
                                       "from path: ", path));
        g_clientLogger.error("Client failed to start");
        exit(1);
    }
}

void Client::displayInfo()
{
    std::cout << "Client information" << std::endl;
    std::cout << "Client Socket: " << socketFd << std::endl;
}

void Client::displayHelp()
{
    std::cout << "Available commands: " << std::endl;
    std::cout << "clear - clear the screen" << std::endl;
    std::cout << "help - show this help" << std::endl;
    std::cout << "info - display client information" << std::endl;
    std::cout << "db get <key> - get data by key" << std::endl;
    std::cout << "db write [-f] <key> <data-type> <value> - modify data by key" << std::endl;
    std::cout << "db get all - get all data" << std::endl;
    std::cout << "exit - exit from client" << std::endl;
}

void Client::handleDatabaseCommands(const std::string &p_command)
{
    std::stringstream commandStream(p_command);
    std::string query;

    commandStream >> query;

    if (query == "get")
    {
        std::string key;
        commandStream >> key;
        handleGetCommand(key);
    }
    else if (query == "write")
    {
        handleWriteCommand(p_command.substr(6));
    }
    else
    {
        std::cout << "Invalid database query" << std::endl;
    }
}

void Client::start()
{
    loadDatabase();
    connectToServer();

    if (connected)
    {
        communicationThread = std::thread(&Client::communicationLoop, this);
    }

    running = true;

    while (running)
    {
        std::string input;
        std::cout << "(client) ";
        std::getline(std::cin, input);

        if (input == "exit")
        {
            std::cout << "Exiting client..." << std::endl;
            running = false;
            break;
        }

        // Check for "server " prefix
        if (input.rfind("server ", 0) == 0)
        {
            sendMessage(input.substr(7)); // Send only the message part
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else if (input == "" || input == "\n")
        {
            continue;
        }
        else if (input == "help")
        {
            displayHelp();
        }
        else if (input.rfind("clear", 0) == 0)
        {
            system(input.c_str());
        }
        else if (input == "info")
        {
            displayInfo();
        }
        else if (input.rfind("db ", 0) == 0)
        {
            handleDatabaseCommands(input.substr(3));
        }
        else
        {
            std::cout << "Invalid command. Use 'help' to display available commands list"
                      << std::endl;
        }
    }

    closeConnection();
}
