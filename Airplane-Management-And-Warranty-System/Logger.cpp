#include "Logger.h"

Logger::Logger(const std::string& filename)
{
    logFile.open(filename, std::ios::app);
}

void Logger::Log(const std::string& message)
{
    // lock write mutex
    std::lock_guard<std::mutex> lock(logMutex);

    // structure log and log if file is open
    std::string entry = "[" + TimeUtils::TimeToString(TimeUtils::NowMs()) + "] " + message;
    if (logFile.is_open()) {
        logFile << entry << std::endl;
        std::cout << entry << std::endl; // print to console
    }
    else { // bad path
        std::cout << "could not log (" << entry << ") as log file was not open.";
    }    
}

Logger::~Logger()
{
	if (logFile.is_open())
        logFile.close();
}
