#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

class Logger {
private:
    static std::ofstream logFile;
    static bool initialized;

    enum Level { DEBUG, INFO, WARN, ERROR };

    static const char* levelStr(Level level) {
        switch (level) {
            case DEBUG: return "DEBUG";
            case INFO: return "INFO";
            case WARN: return "WARN";
            case ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

    static void writeLog(Level level, const std::string& message) {
        std::time_t now = std::time(nullptr);
        std::tm* timeinfo = std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << " [" << levelStr(level) << "] " << message;
        std::string logMsg = oss.str();

        std::cerr << logMsg << std::endl;

        if (initialized && logFile.is_open()) {
            logFile << logMsg << std::endl;
            logFile.flush();
        }
    }

public:
    static void init(const std::string& filename = "parqueo.log") {
        logFile.open(filename, std::ios::app);
        initialized = true;
    }

    static void debug(const std::string& message) { writeLog(DEBUG, message); }
    static void info(const std::string& message) { writeLog(INFO, message); }
    static void warn(const std::string& message) { writeLog(WARN, message); }
    static void error(const std::string& message) { writeLog(ERROR, message); }

    static void close() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

std::ofstream Logger::logFile;
bool Logger::initialized = false;

#endif
