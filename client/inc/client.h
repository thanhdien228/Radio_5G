#pragma once
#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include "logger.h"
#include "inMemDatabase.h"
#include <optional>

/// @brief Size of the buffer for send and receive message
constexpr int BUFFER_SIZE = 1024;

/// @brief Server IP to which client connect
constexpr const char *SERVER_IP = "0.0.0.0";

/// @brief Server port to which client connect
constexpr int SERVER_PORT = 8080;

/// @brief The log file path of client side
constexpr const char *LOG_FILE_PATH = "client.log";

/// @brief Logger to control logging message of client side
inline Logger g_clientLogger = Logger(LOG_FILE_PATH);

/// @brief Function to init logger
void initLogger();

/// @brief Client class in a TCP Socket Network
class Client
{
public:
    /// @brief Constructor of Client class
    Client();

    /// @brief Default destructor of Client class
    ~Client() = default;

    /// @brief Start function of the Client class, which will be called by clientMain
    void start();

private:
    /// @brief Server IP attribute inside the Client object
    std::string serverIp;

    /// @brief Server Port attribute inside the Client object
    int serverPort;

    /// @brief Client file descriptor (FD), equals -1 if there is no connection with server
    int socketFd;

    /// @brief Attribute to control the running state of the client
    std::atomic<bool> running;

    /// @brief Attribute to control the connection state of the client to server
    std::atomic<bool> connected;

    /// @brief Thread to continuously holding the connection with the server
    std::thread communicationThread;

    /**
     * @brief Initiate a connection with the server at the beginning of the program
    */
    void connectToServer();

    /**
     * @brief Attempting to reconnect with the server whenever needed
    */
    void reconnectToServer();

    /**
     * @brief Create a thread to continuously listen to the message from the server. This thread
     * will be joined when the connection is cut down by either the server or the client
    */
    void communicationLoop();
    
    /**
     * @brief Send a message to server. If no connection is made, client will try to connect with
     * server and create a new connection
     * 
     * @param p_message The message that needed to be sent
    */
    void sendMessage(const std::string &p_message);

    /**
     * @brief Close the connection from client side and safely join the communication thread
    */
    void closeConnection();

    /**
     * @brief Set the socket to non-blocking mode
     * 
     * @param p_fd The file descriptor that needed to be set to non-blocking mode
    */
    void setSocketNonblocking(int p_fd);

    /**
     * @brief Load the defined folder to the client side database. If failed to load, the client
     * will not be able to start
    */
    void loadDatabase();

    /**
     * @brief Show the list of available commands to interact with the client
    */
    void displayHelp();

    /**
     * @brief Handle database command at client side
     * 
     * @param p_command The part after "db " from the user input
    */
    void handleDatabaseCommands(const std::string &p_command);

    /**
     * @brief Show information of the client side, including the FD. If FD = -1, it means there
     * is no connection from client to server, otherwise the FD will be a non-negative integer.
    */
    void displayInfo();
};