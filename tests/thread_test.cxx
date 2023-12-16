#include <iostream>
#include <string>
#include <cstdio>
#include <thread>
#include "../Easylog.hpp"

std::string removePattern(const std::string& input)
{
    std::string result = input;
    std::size_t start = result.find("[") + 1;
    std::size_t end = result.find("]");

    result.erase(start, end - start);

    return result;
}

void workerThread()
{
    for(int i = 0; i < 15; i++)
    {
        eLog::logDebug("worker thread");
    }
}

int main()
{
    eLog::State::SetState(eLog::State::StateEnum::DIRECT_FLUSH, true);
    eLog::State::SetState(eLog::State::StateEnum::FILE_LOG, true);

    std::thread worker(::workerThread);
    for(int i = 0; i < 15; i++)
    {
        eLog::logDebug("main thread");
    }

    worker.join();

    eLog::FileLogImpl::Impl::Data::DefaultFileLogger.mStream.close();

    std::ifstream file("log.txt");
    int passed = 0;

    if (file.is_open()) 
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::string result = ::removePattern(line);
            if(result != std::string("DEBUG\t: [] : worker thread") && result != std::string("DEBUG\t: [] : main thread"))
            {
                passed = -1;
                break;
            }
        }
    } 
    else
        passed = -1;
    

    file.close();

    std::remove("log.txt");

    return passed;
}