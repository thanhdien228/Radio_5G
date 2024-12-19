#pragma once
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "serverCommon.h"
#include "carrier.h"
#include "modulator.h"
#include "antenna.h"

/// @brief The port number used to establish connection with client
constexpr unsigned int SERVER_PORT = 8080;

/// @brief The size of the read and the write buffer of server
constexpr unsigned int BUFFER_SIZE = 1024;

/// @brief The domain address used to establish connection with client
constexpr const char *SERVER_IP_ADDR = "0.0.0.0";

/// @brief The database file path of server
constexpr int MAX_CONNECTION_REQUEST = 3;

/// @brief The maximum number of events to be monitored with epoll
constexpr int MAX_EVENTS = 10;

/// @brief The log file path of server
constexpr const char *LOG_FILE_PATH = "server.log";

/// @brief The database file path of server
constexpr const char *INITIAL_DATABASE_PATH = "./db";

/// @brief Initialize logger of server side
void initLogger();

class Server
{
public:
    Server();
    ~Server();
    /**
     * @brief Initialize server socket
     */
    void init();

    /**
     * @brief Accept incoming connections and handle clients
     */
    void start();

private:
    std::string m_command;
    std::string m_dbPath;
    std::atomic<bool> m_serverRunning;

    int m_serverSocket;
    int m_clientSocket;
    std::vector<std::thread> m_threads;
    struct sockaddr_in m_serverAddress;
    struct sockaddr_in m_clientAddress;
    socklen_t m_clientAddrLen = sizeof(m_clientAddress);

    int m_epollFd;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];

    bool m_canInitDB;

    std::unique_ptr<Carrier> m_carrier;
    std::unique_ptr<Modulator> m_modulator;
    std::unique_ptr<Antenna> m_antenna;

    /**
     * @brief Initialize database of server side
     */
    void initDB();

    /**
     * @brief Set a socket to non-blocking mode
     *
     * @param p_sockFD - a socket
     * @return a number indicating whether a socket is set to non-blocking mode successfully or not
     * (Returns 0 on success, -1 for errors)
     */
    int setSocketNonblocking(const int &p_sockFD);

    /**
     * @brief Handle a client
     *
     * @param p_clientSocket - a client socket
     */
    void handleClient(const int &p_clientSocket);

    /**
     * @brief Handle common server commands
     */
    void handleCommand();

    /**
     * @brief Handle database server commands
     */
    void handleDBCommand();

    /**
     * @brief Handle client get commands for server database
     *
     * @param p_key - key user want to get value or "all" to get all values
     * @return database results
     */
    std::string handleClientGetDBCommand(const std::string &p_key);

    /**
     * @brief Handle commands from client sent to server.
     *
     * @param p_buffer - message from client
     * @return message containing database results sent to client.
     */
    std::string handleClientCommand(const char *p_buffer);

    /**
     * @brief Set up carrier for server
     *
     * @param p_network - network that is set up to transmit and receive data
     * @param p_freq - frequency of carrier
     * @return message send to client
     */
    std::string setNetworkForServer(const std::string &p_network, const ssize_t &p_freq);
};