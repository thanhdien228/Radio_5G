#pragma once
#include "commandLine.h"
#include "logger.h"

/// @brief The CLI to control and execute database commands at server side
static inline CommandLineInterface g_serverDatabase;

/// @brief The Logger to control and execute log message at server side
static inline Logger g_serverLogger("server.log");
