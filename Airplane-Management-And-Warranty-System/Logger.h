#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <iostream>
#include "TimeUtils.h"

class Logger
{
	std::ofstream logFile;
	std::mutex logMutex;
public:
    Logger(const std::string& filename = "serverLog.txt");

    void Log(const std::string& message);

    ~Logger();
};

