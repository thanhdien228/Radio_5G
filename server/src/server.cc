#include "server.h"
#include <regex>
#include <cstring>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <complex>
#include <sstream>

bool saveInputFile(const std::vector<double> &p_inputWave, bool isFilter);
bool isBinaryString(std::string p_string);

void initLogger()
{
    g_serverLogger.enableLogFile(true);
    g_serverLogger.setPriority(LogPriority::ERROR);
}

void Server::initDB()
{
    g_serverLogger.info("Loading database...");
    m_canInitDB = g_serverDatabase.initDB(m_dbPath);
    if (!m_canInitDB)
    {
        g_serverLogger.error("Invalid database directory!");
        exit(1);
    }
    g_serverLogger.info(stringify("The default database directory is '", m_dbPath, "'"));
}

Server::Server() : m_serverRunning(true)
{
    m_dbPath = INITIAL_DATABASE_PATH;
    initLogger();
    initDB();
    m_carrier = std::make_unique<Carrier>();
    m_modulator = std::make_unique<Modulator>();
    m_antenna = std::make_unique<Antenna>();
}

Server::~Server()
{
    m_threads.clear();
}

int Server::setSocketNonblocking(const int &p_sockFD)
{
    // fcntl - manipulate file descriptor
    // If successful, the value returned will depend on the action that was specified.
    // If unsuccessful, returns a value of -1

    int flags = fcntl(p_sockFD, F_GETFL);
    if (flags == -1)
    {
        g_serverLogger.error("fcntl: " + std::string(strerror(errno)));
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(p_sockFD, F_SETFL, flags) == -1)
    {
        g_serverLogger.error("fcntl: " + std::string(strerror(errno)));
        return -1;
    }
    return 0;
}

void Server::init()
{
    // Create socket
    // int socket(int domain, int type, int protocol)
    // The protocol specifies a particular protocol to be used with the socket.
    // If PROTOCOL is zero, one is chosen automatically.
    // Returns a file descriptor for the new socket, or -1 for errors.
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket == -1)
    {
        g_serverLogger.error("Failed to create socket.");
    }

    if (setSocketNonblocking(m_serverSocket) == -1)
    {
        g_serverLogger.error("Failed to set non-blocking mode for server.");
    }

    // Bind socket to port
    m_serverAddress.sin_family = AF_INET;
    m_serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
    m_serverAddress.sin_port = htons(SERVER_PORT);

    // A pointer to the buffer in which the value for the requested option is specified.
    int optval = 1;

    // SO_REUSEADDR
    // Specifies that the rules used in validating addresses
    // supplied to bind() should allow reuse of local addresses, if this is supported by the protocol.
    // Without SO_REUSEADDR, the bind() call in the restarted program's new instance will fail
    // if there were connections open to the previous instance and even after getting killed (30s~120s)
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // On success, 0 is returned.  On error, -1 is returned.
    if (bind(m_serverSocket, (struct sockaddr *)&m_serverAddress, sizeof(m_serverAddress)) == -1)
    {
        g_serverLogger.error("Bind failed.");
        close(m_serverSocket);
    }

    // Listen for incoming connections
    // Returns 0 on success, -1 for errors.
    if (listen(m_serverSocket, MAX_CONNECTION_REQUEST) == -1)
    {
        g_serverLogger.error("Listen failed.");
        close(m_serverSocket);
    }

    // Create the epoll instance
    // int epoll_create1(int flags)
    // If flags is 0, then, the obsolete size argument is dropped
    // On success, these system calls return a nonnegative file descriptor.
    // On error, -1 is returned, and errno is set to indicate the error.
    m_epollFd = epoll_create1(0);
    if (m_epollFd == -1)
    {
        g_serverLogger.error("epoll_create: " + std::string(strerror(errno)));
        close(m_serverSocket);
    }

    // Add the server socket to the epoll instance
    ev.events = EPOLLIN;
    ev.data.fd = m_serverSocket;

    // epoll_ctl - control interface for an epoll file descriptor
    // When successful, returns zero.
    // When an error occurs, returns -1 and errno is set to indicate the error.
    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_serverSocket, &ev) == -1)
    {
        g_serverLogger.error("epoll_ctl: " + std::string(strerror(errno)));
        close(m_serverSocket);
        close(m_epollFd);
    }

    g_serverLogger.info(stringify("Server listening on port ", SERVER_PORT, "..."));
}

void Server::start()
{
    m_threads.emplace_back([&, this]()
                           { handleCommand(); });

    // Main loop to handle epoll events
    while (m_serverRunning)
    {
        //  wait for an I/O event on an epoll file descriptor
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        // The "timeout" parameter specifies the maximum wait time in milliseconds(-1 == infinite).
        // On success, epoll_wait() returns the number of file descriptor ready for the requested I/O operation,
        // or zero if no file descriptor became ready during the requested timeout milliseconds.
        // On failure, epoll_wait() returns -1 and errno is set to indicate the error.

        int num_events = epoll_wait(m_epollFd, events, MAX_EVENTS, -1);
        if (num_events == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            g_serverLogger.error("epoll_wait: " + std::string(strerror(errno)));
            break;
        }

        for (int i = 0; i < num_events; i++)
        {
            if (events[i].data.fd == m_serverSocket)
            {
                // New client connection
                // If successful, accept() returns a nonnegative socket descriptor.
                // If unsuccessful, accept() returns -1 and sets errno to indicate the error.
                m_clientSocket = accept(m_serverSocket, (struct sockaddr *)&m_clientAddress, &m_clientAddrLen);
                if (m_clientSocket == -1)
                {
                    g_serverLogger.error("accept socket: " + std::string(strerror(errno)));
                    continue;
                }

                // Set the client socket to non-blocking
                if (setSocketNonblocking(m_clientSocket) == -1)
                {
                    close(m_clientSocket);
                    continue;
                }

                // Add the new client socket to the epoll instance
                ev.events = EPOLLIN | EPOLLET; // Edge-triggered mode
                ev.data.fd = m_clientSocket;

                // epoll_ctl - control interface for an epoll file descriptor
                // When successful, returns zero.
                // When an error occurs, returns -1 and errno is set to indicate the error.
                if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_clientSocket, &ev) == -1)
                {
                    g_serverLogger.error("epoll_ctl: " + std::string(strerror(errno)));
                    close(m_clientSocket);
                    continue;
                }

                g_serverLogger.info(stringify("Accepted connection from ", inet_ntoa(m_clientAddress.sin_addr),
                                              ". Client ", m_clientSocket, " connected."));
            }
            else
            {
                // Handle data from an existing client
                handleClient(events[i].data.fd);
            }
        }
    }

    for (auto &th : m_threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    // Close the server socket and epoll instance
    close(m_serverSocket);
    close(m_epollFd);
}

void Server::handleCommand()
{
    const std::regex whiteSpace("\\s*");

    while (m_serverRunning)
    {
        std::cout << "(server) ";
        std::getline(std::cin, m_command);
        std::stringstream cmdStream(m_command);
        std::string firstCmd;
        cmdStream >> firstCmd;
        if (firstCmd == "exit")
        {
            m_serverRunning = false;
            g_serverLogger.info("Shutting down server...");
            std::cout << "Shutting down server..." << "\n";
            exit(0);
        }
        else if (firstCmd == "info")
        {
            std::cout << "Server listening on port " << SERVER_PORT << "." << "\n";
            std::cout << "The server address is " << inet_ntoa(m_serverAddress.sin_addr) << "\n";
            std::cout << "The server network is " << m_carrier.get()->getNetwork() << "\n";
            std::cout << "Frequency Carrier is " << m_carrier.get()->getFrequency() << "\n";
        }
        else if (firstCmd == "help")
        {
            std::cout << "Commands:" << "\n";
            std::cout << "info - show information of server" << "\n";
            std::cout << "help - show all commands" << "\n";
            std::cout << "clear - clear the screen" << "\n";
            std::cout << "db get <key> - get data by key" << "\n";
            std::cout << "db write [-f] <key> <data-type> <value> - modify data by key" << "\n";
            std::cout << "db get all - get all data" << "\n";
            std::cout << "exit - exit the server" << "\n";
        }
        else if (firstCmd == "clear")
        {
            system("clear");
        }
        else if (firstCmd == "db")
        {
            handleDBCommand();
        }
        else if (regex_match(m_command, whiteSpace))
        {
            continue;
        }
        else
        {
            std::cout << "Invalid command - use help to list all the commands." << "\n";
        }
    }
}

std::string Server::handleClientCommand(const char *p_buffer)
{
    std::string message;
    std::string bufferStr = p_buffer;
    std::stringstream strStream(bufferStr);
    std::string query;
    std::string keyNetwork;
    std::string frequency;
    std::string binaryData;

    strStream >> query;

    if (query == "db")
    {
        strStream >> query;
        if (query == "get")
        {
            std::string key;
            strStream >> key;
            message = handleClientGetDBCommand(key);
        }
        else
        {
            message = "Only allow get commands for server database.";
        }
    }
    else if (query == "carrier")
    {
        strStream >> query;
        if (query == "setup")
        {
            strStream >> keyNetwork;
            strStream >> frequency;
            if (keyNetwork == "" || frequency == "")
            {
                message = "Missing keyNetwork or frequency";
                return message;
            }
            ssize_t numFreq = std::stol(frequency);
            message = setNetworkForServer(keyNetwork, numFreq);
        }
        else if (query == "release")
        {
            m_carrier.get()->releaseCarrier();
            message = "Release carrier setting";
        }
        else
        {
            message = "Invalid command for carrier";
        }
    }
    else if (query == "DL")
    {
        if (!m_carrier.get()->getCarrierStatus())
        {
            message = "Please setup carrier: 'server carrier setup <network> <frequency>";
        }
        else
        {
            strStream >> binaryData;
            if (binaryData == "")
            {
                message = "Missing binaryData";
                return message;
            }
            else if (!isBinaryString(binaryData))
            {
                message = "Data received is not a binary string";
                return message;
            }
            std::cout << "Received binary data: " << binaryData << std::endl;
            std::string passNetwork = m_carrier.get()->getNetwork();
            if (passNetwork == "5G" && binaryData.length() % 4 != 0)
            {
                message = "Binary data length must be a multiple of 4 for 16-QAM.";
                g_serverLogger.error("Binary data length must be a multiple of 4 for 16-QAM.");
            }
            else
            {
                m_modulator.get()->setBinaryInput(binaryData);
                m_modulator.get()->setFrequency(m_carrier.get()->getFrequency());
                std::vector<double> signalModulated = m_modulator.get()->modulate(passNetwork);
                if (saveInputFile(signalModulated, true))
                {
                    g_serverLogger.info("Open file inputFiltered successfully");
                }
                else
                {
                    g_serverLogger.error("Fail to open file for wave inputFiltered data");
                }               
                m_antenna.get()->addNoise(signalModulated);
                if (saveInputFile(signalModulated, false))
                {
                    g_serverLogger.info("Open file inputNoise successfully");
                }
                else
                {
                    g_serverLogger.error("Fail to open file for wave inputNoise data");
                }
                std::optional<std::pair<std::string, std::string>> dlParam = std::make_pair(binaryData, std::to_string(m_carrier.get()->getFrequency()));
                m_antenna.get()->visualizeData(dlParam);
            }
        }
    }
    else if (query == "UL")
    {
        if (!m_carrier.get()->getCarrierStatus())
        {
            message = "Please setup network: 'server carrier setup <network> <frequency>";
            return message;
        }
        int bitSize = 13;
        if (m_carrier.get()->getNetwork() == "5G")
        {
            bitSize *= 4;
        }
        std::string binaryGenerated = m_antenna.get()->randomBinaryMessageGenerator(bitSize);
        m_modulator.get()->setBinaryInput(binaryGenerated);
        m_modulator.get()->setFrequency(m_carrier.get()->getFrequency());
        std::vector<double> signalGenerated = m_modulator.get()->modulate(m_carrier.get()->getNetwork());
        if (saveInputFile(signalGenerated, true))
        {
            g_serverLogger.info("Open file inputFilter is successfull");
        }
        else
        {
            g_serverLogger.error("Fail to open file for wave inputFilter data");
        }
        m_antenna.get()->addNoise(signalGenerated);
        if (saveInputFile(signalGenerated, false))
        {
            g_serverLogger.info("Open file inputNoise is successfull");
        }
        else
        {
            g_serverLogger.error("Fail to open file for wave inputNoise data");
        }
        m_antenna.get()->filterNoise(signalGenerated);
        std::string demodBinaryData = m_modulator.get()->demodulate(signalGenerated, m_carrier.get()->getNetwork());
        g_serverLogger.info(binaryGenerated);
        m_antenna.get()->visualizeData();
        message = demodBinaryData;
    }
    else
    {
        message = bufferStr;
    }
    return message;
}

void Server::handleClient(const int &p_clientSocket)
{
    char buffer[BUFFER_SIZE] = {0};

    // ssize_t recv(int socket, char *buffer, int length, int flags);
    // if flags are 0, no flags are specified
    // If successful, recv() returns the length of the message or datagram in bytes. The value 0 indicates the connection is closed.
    // If unsuccessful, recv() returns -1 and sets errno to indicate the error.

    ssize_t bytesRead = recv(p_clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';

        g_serverLogger.info(stringify("Received message: ", std::string(buffer)));
        std::string message = handleClientCommand(buffer);

        // Convert message to char array
        strcpy(buffer, message.c_str());

        // Echo the message back to the client
        // ssize_t send(int socket, const void *buffer, size_t length, int flags);
        // if flags are 0, no flags are specified
        send(p_clientSocket, buffer, message.size(), 0);
    }
    else if (bytesRead == 0)
    {
        g_serverLogger.info(stringify("Client ", p_clientSocket, " disconnected."));
    }
    else
    {
        g_serverLogger.error("Error receiving data.");
    }
}

void Server::handleDBCommand()
{
    std::stringstream strStream(m_command);
    std::string query;
    strStream >> query;
    strStream >> query;

    if (query == "get")
    {
        std::string key;
        strStream >> key;
        g_serverDatabase.handleGetCommand(key);
    }
    else if (query == "write")
    {
        bool force = false;
        g_serverDatabase.checkForceAndErase(m_command, force);
        int checkWriteComand = g_serverDatabase.handleWriteCommand(m_command, force);
        if (checkWriteComand == 0)
        {
            std::cout << "Fail to change data for database!" << "\n";
        }
    }
    else
    {
        std::cout << "Invalid command - use help to list all the commands.\n";
    }
}

std::string Server::handleClientGetDBCommand(const std::string &p_key)
{
    std::string message;
    if (p_key == "all")
    {
        std::vector<std::string> result = g_serverDatabase.getAllKey();
        for (std::string item : result)
        {
            message += item + "\n";
        }
    }
    else
    {
        std::optional<std::string> result = g_serverDatabase.getKey(p_key);
        if (result != std::nullopt)
        {
            message += result.value() + "\n";
        }
    }

    return message;
}

std::string Server::setNetworkForServer(const std::string &p_network, const ssize_t &p_freq)
{
    std::string message;
    if (m_carrier.get()->checkSupportedCarrier(p_network) &&
        m_carrier.get()->checkSupportedFrequency(p_freq))
    {
        g_serverLogger.info("The server has support " + p_network + "!");
        if (m_carrier.get()->setNetwork(p_network))
        {
            m_carrier.get()->setFrequency(p_freq);
            message = "Successfully set up " + p_network + " network";
        }
        else
        {
            message = "Please remove old network setting";
        }
    }
    else if (!m_carrier.get()->checkSupportedFrequency(p_freq))
    {
        message = "Frequency out of range, must be from 1-10";
    }
    else
    {
        g_serverLogger.error(stringify("The server do not support ", p_network, "!"));
        message = "The server do not support " + p_network + " network";
    }
    return message;
}

bool saveInputFile(const std::vector<double> &p_inputWave, bool isFilter)
{
    std::string inputFilePathKey;
    if (!isFilter)
    {
        inputFilePathKey = "/inputNoise";
    }
    else
    {
        inputFilePathKey = "/inputFiltered";
    }
    const char *inputFilePath = "";
    auto var = InMemDatabase::getInstance().getValue(inputFilePathKey);
    extractValue<char const *>(var, inputFilePath);
    std::string strInputFilePath(inputFilePath);
    std::ofstream file(strInputFilePath);
    if (!file.is_open())
    {
        return false;
    }
    for (auto value : p_inputWave)
    {
        file << value << std::endl;
    }
    file.close();
    return true;
}

bool isBinaryString(std::string p_string)
{
    for (char curChar : p_string)
    {
        if (curChar != '0' && curChar != '1')
        {
            return false;
        }
    }
    return true;
}