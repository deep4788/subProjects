#include "log.hpp"

#include <cstdio>
#include <ctime>

Log::Log() : _logFileName("../lib/sampleLog.log")
{}

Log::~Log()
{
    _os << std::endl;
    output(_os.str());
}

std::string nowTimeAndDate()
{
    char buffer[28] = {0};
    static const char wday_name[][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(buffer, "[%.3s %.2d/%.2d/%d] [%.2d:%.2d:%.2d]", wday_name[timeinfo->tm_wday], timeinfo->tm_mon,
            timeinfo->tm_mday, 1900+timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return buffer;
}

std::ostringstream& Log::getOutputStream(LOG_LEVEL logLevel)
{
    _os << "- " << nowTimeAndDate();
    _os << " " << toString(logLevel) << ": ";
    _os << "\t";
    return _os;
}

std::string Log::toString(LOG_LEVEL logLevel)
{
	static const char* const buffer[] = {"DEBUG", "ERROR", "INFO", "WARNING"};
    const int logLevelIndex = static_cast<int>(logLevel);
    if(logLevelIndex < 0 || logLevelIndex > 3)
    {
        return "UNKNOWN";
    }
    return buffer[logLevelIndex];
}

FILE*& Log::stream(std::string streamTypeToUse)
{
    static FILE* pStream = NULL;
    if(streamTypeToUse == "stderr")
    {
        pStream = stderr;
    }
    else //If "file" is passed, use the file stream
    {
        const char* logFile = Log::getLogFileName().c_str();
        if(logFile == NULL)
        {
            return stderr;
        }

        FILE* pFile = fopen(logFile, "r");
        if(pFile == NULL)
        {
            //If we are here, that means file does not exist on disk, so we will create it here
            pFile = fopen(logFile, "w");

            //We are closing the pFile object, since we want to open it for appending
            fclose(pFile);
        }

        //Open the file in append mode
        pFile = fopen(logFile, "a");

        if(pFile != NULL)
        {
            pStream = pFile;
        }
        else
        {
            pStream = stderr;
        }
    }

    return pStream;
}

void Log::output(const std::string& msg)
{
    FILE* pStream = stream("file");
    if(pStream == NULL)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}

std::string Log::getLogFileName() const
{
    return _logFileName;
}
