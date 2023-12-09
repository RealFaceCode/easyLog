#ifndef EASY_LOG_HPP
#define EASY_LOG_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <stdint.h>
#include <cstring>
#include <source_location>
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace eLog {
    namespace AsciiColor
    {
        static constexpr const char* RESET = "\033[0m";
        static constexpr const char* BLACK = "\033[30m";
        static constexpr const char* RED = "\033[31m";
        static constexpr const char* GREEN = "\033[32m";
        static constexpr const char* YELLOW = "\033[33m";
        static constexpr const char* BLUE = "\033[34m";
        static constexpr const char* MAGENTA = "\033[35m";
        static constexpr const char* CYAN = "\033[36m";
        static constexpr const char* WHITE = "\033[37m";
        static constexpr const char* BOLD_BLACK = "\033[1m\033[30m";
        static constexpr const char* BOLD_RED = "\033[1m\033[31m";
        static constexpr const char* BOLD_GREEN = "\033[1m\033[32m";
        static constexpr const char* BOLD_YELLOW = "\033[1m\033[33m";
        static constexpr const char* BOLD_BLUE = "\033[1m\033[34m";
        static constexpr const char* BOLD_MAGENTA = "\033[1m\033[35m";
        static constexpr const char* BOLD_CYAN = "\033[1m\033[36m";
        static constexpr const char* BOLD_WHITE = "\033[1m\033[37m";

        static bool CheckIfColorIsSupported()
        {
            if (const char* term = std::getenv("TERM"); term == nullptr)
                return false;
            return true;
        }

        bool IsColorString(const char* str)
        {
            if(str == nullptr)
                return false;
            else if(::strcmp(str, RESET) == 0)
                return true;
            else if(::strcmp(str, BLACK) == 0)
                return true;
            else if(::strcmp(str, RED) == 0)
                return true;
            else if(::strcmp(str, GREEN) == 0)
                return true;
            else if(::strcmp(str, YELLOW) == 0)
                return true;
            else if(::strcmp(str, BLUE) == 0)
                return true;
            else if(::strcmp(str, MAGENTA) == 0)
                return true;
            else if(::strcmp(str, CYAN) == 0)
                return true;
            else if(::strcmp(str, WHITE) == 0)
                return true;
            else if(::strcmp(str, BOLD_BLACK) == 0)
                return true;
            else if(::strcmp(str, BOLD_RED) == 0)
                return true;
            else if(::strcmp(str, BOLD_GREEN) == 0)
                return true;
            else if(::strcmp(str, BOLD_YELLOW) == 0)
                return true;
            else if(::strcmp(str, BOLD_BLUE) == 0)
                return true;
            else if(::strcmp(str, BOLD_MAGENTA) == 0)
                return true;
            else if(::strcmp(str, BOLD_CYAN) == 0)
                return true;
            else if(::strcmp(str, BOLD_WHITE) == 0)
                return true;
            else
                return false;
        }
    } // namespace AsciiColor

    namespace StringHelper
    {
        struct ReplaceString
        {
            std::string mBaseString;
            std::string mReplaceString;
            std::string mPrevColor;
            std::size_t mPosEndColor;
            bool mReplaceAllMatching = false;
        };

        bool isCharPunctuationMark(char c)
        {
            if(c == ' ' || c == ',' || c == '.' || c == '!' || c == '?' || c == ';' || c == ':' || c == '\n' || c == '\t')
                return true;
            else
                return false;
        }
    
        std::vector<size_t> findStandaloneMatchPositions(std::string_view str, std::string_view match)
        {
            std::vector<size_t> positions;
            size_t pos = str.find(match);
            while(pos != std::string::npos)
            {
                if(pos == 0)
                {
                    if(isCharPunctuationMark(str[pos + match.length()]))
                        positions.push_back(pos);
                }
                else if(pos == str.length() - match.length())
                {
                    if(isCharPunctuationMark(str[pos - 1]))
                        positions.push_back(pos);
                }
                else
                {
                    if(isCharPunctuationMark(str[pos - 1]) && isCharPunctuationMark(str[pos + match.length()]))
                        positions.push_back(pos);
                }
                pos = str.find(match, pos + match.length());
            }
            return positions;
        }

        std::size_t findStandaloneNextMatchPosition(std::string_view str, std::string_view match, std::size_t pos = 0)
        {
            std::size_t nextPos = str.find(match, pos);
            if(nextPos == std::string::npos)
                return nextPos;

            if(nextPos == 0)
            {
                if(isCharPunctuationMark(str[nextPos + match.length()]))
                    return nextPos;
            }
            else if(nextPos == str.length() - match.length())
            {
                if(isCharPunctuationMark(str[nextPos - 1]))
                    return nextPos;
            }
            else
            {
                if(isCharPunctuationMark(str[nextPos - 1]) && isCharPunctuationMark(str[nextPos + match.length()]))
                    return nextPos;
            }

            return findStandaloneNextMatchPosition(str, match, nextPos + match.length());
        }

        void Replace(std::string& str, std::string_view match, std::string_view replace, bool replaceAllMatching = false)
        {
            std::size_t pos = findStandaloneNextMatchPosition(str, match);
            if(!replaceAllMatching)
            {
                if(pos != std::string::npos)
                    str.replace(pos, match.length(), replace);
            }
            else
            {
                while(pos != std::string::npos && pos < str.length())
                {
                    str.replace(pos, match.length(), replace);
                    pos = findStandaloneNextMatchPosition(str, match, pos + replace.length());
                }
            }
        }

        class ColorizedString
        {
        public:
            ColorizedString() = default;
            explicit ColorizedString(std::string_view str) 
            : mStr(str) {}

            void setContext(std::string_view str) 
            {
                mStr = str;
            }

            void setColor(std::string_view strToColorize, const char* color, bool replaceAllMatching = false)
            {
                if (strToColorize.empty() && !AsciiColor::IsColorString(color))
                    return;

                std::string prevColor (AsciiColor::RESET);
                std::size_t lastColorPos = 0;

                if(!mReplaceStrings.empty())
                {
                    prevColor = mReplaceStrings.back().mPrevColor;
                    lastColorPos = mReplaceStrings.back().mPosEndColor;
                }

                std::string replace;
                if(lastColorPos < mStr.find(strToColorize))
                    replace = std::string(color).append(strToColorize).append(AsciiColor::RESET);
                else
                    replace = std::string(color).append(strToColorize).append(prevColor);

                prevColor = color;

                StringHelper::ReplaceString replaceString
                {
                    .mBaseString = std::string(strToColorize),
                    .mReplaceString = replace,
                    .mPrevColor = prevColor,
                    .mPosEndColor = mStr.find(strToColorize) + strToColorize.length(),
                    .mReplaceAllMatching = replaceAllMatching
                };

                mReplaceStrings.push_back(replaceString);
            }

            void colorize() 
            {
                for(const auto& replace : mReplaceStrings)
                {
                    StringHelper::Replace(mStr, replace.mBaseString, replace.mReplaceString, replace.mReplaceAllMatching);
                }
            }

            const std::string& getColorizedString() const 
            {
                return mStr;
            }

            void print() const {
                std::cout << getColorizedString() << std::endl;
            }

            friend std::ostream& operator<<(std::ostream& os, const ColorizedString& colorizedString)
            {
                os << colorizedString.getColorizedString();
                return os;
            }

            private:
                std::string mStr;
                std::vector<StringHelper::ReplaceString> mReplaceStrings;
        };
    } // namespace StringHelper

    namespace State
    {
        static bool IsFileLogEnabled = false;
        static bool IsConsoleLogEnabled = true;
        static bool IsColorEnabled = AsciiColor::CheckIfColorIsSupported();
        static bool UseDefaultFileLog = true;
        static std::string FileLoggerName = "";

        enum class StateEnum
        {
            TERMINAL_LOG,
            FILE_LOG,
            DEFAULT_FILE_LOG
        };
    }

    namespace LogLevel
    {
        struct LogLevel
        {
            std::string mLogLevel;
            std::string mColor;
        };

        constexpr LogLevel DEBUG = { "DEBUG", AsciiColor::BOLD_BLUE };
        constexpr LogLevel INFO = { "INFO", AsciiColor::BOLD_GREEN };
        constexpr LogLevel WARNING = { "WARNING", AsciiColor::BOLD_YELLOW };
        constexpr LogLevel ERROR = { "ERROR", AsciiColor::BOLD_RED };
        constexpr LogLevel FATAL = { "FATAL", AsciiColor::BOLD_MAGENTA };

        std::string getLogLevelString(const LogLevel& logLevel)
        {
            return std::string(logLevel.mColor).append(logLevel.mLogLevel).append(AsciiColor::RESET);
        }
    } // namespace LogLevel

    namespace LogInfo
    {
        struct LogInfo
        {
            std::string mColor;
            std::filesystem::path mFile;
            std::string mFunction;
            std::string mLine;
            std::string mDate;
            std::string mTime;
        };

        constexpr const char* LogInfoFmt = "{}[{} | {} | {} | {} | {}]{}";

        LogInfo getLogInfo(const std::source_location& src)
        {
            return LogInfo {
                .mColor = AsciiColor::BOLD_WHITE,
                .mFile = src.file_name(),
                .mFunction = src.function_name(),
                .mLine = std::to_string(src.line()),
                .mDate = __DATE__,
                .mTime = __TIME__,
            };
        }

        std::string getFmtLogInfo(const LogInfo& logInfo)
        {
            std::string fmtLogInfo = LogInfoFmt;

            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, logInfo.mColor);
            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, logInfo.mDate);
            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, logInfo.mTime);
            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, logInfo.mFile.filename().string());
            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, logInfo.mFunction);
            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, logInfo.mLine);
            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, AsciiColor::RESET);

            return fmtLogInfo;
        }
    } // namespace LogInfo

    namespace LogImpl
    {
        void log(const LogLevel::LogLevel& logLevel, std::string_view msg, const std::source_location& src = std::source_location::current())
        {
            std::string logLevelString = LogLevel::getLogLevelString(logLevel);
            std::string fmtLogInfo = LogInfo::getFmtLogInfo(LogInfo::getLogInfo(src));
            std::cout << logLevelString << "\t: " << fmtLogInfo << " : " << msg << std::endl;
        }

        void log(const LogLevel::LogLevel& logLevel, const StringHelper::ColorizedString& colorizedString, const std::source_location& src = std::source_location::current())
        {
            std::string logLevelString = LogLevel::getLogLevelString(logLevel);
            std::string fmtLogInfo = LogInfo::getFmtLogInfo(LogInfo::getLogInfo(src));
            std::cout << logLevelString << "\t: " << fmtLogInfo << " : " << colorizedString << std::endl;
        }
    } // namespace LogImpl

    namespace FileLogLevel
    {
        constexpr const char* DEBUG = "DEBUG";
        constexpr const char* INFO = "INFO";
        constexpr const char* WARNING = "WARNING";
        constexpr const char* ERROR = "ERROR";
        constexpr const char* FATAL = "FATAL";
    }

    namespace FileLogInfo
    {
        struct FileLogInfo
        {
            std::filesystem::path mFile;
            std::string mFunction;
            std::string mLine;
            std::string mDate;
            std::string mTime;
        };

        constexpr const char* FileLogInfoFmt = "[{} | {} | {} | {} | {}]";

        FileLogInfo getFileLogInfo(const std::source_location& src)
        {
            return FileLogInfo {
                .mFile = src.file_name(),
                .mFunction = src.function_name(),
                .mLine = std::to_string(src.line()),
                .mDate = __DATE__,
                .mTime = __TIME__,
            };
        }

        std::string getFmtFileLogInfo(const FileLogInfo& fileLogInfo)
        {
            std::string fmtFileLogInfo = FileLogInfoFmt;

            fmtFileLogInfo.replace(fmtFileLogInfo.find("{}"), 2, fileLogInfo.mDate);
            fmtFileLogInfo.replace(fmtFileLogInfo.find("{}"), 2, fileLogInfo.mTime);
            fmtFileLogInfo.replace(fmtFileLogInfo.find("{}"), 2, fileLogInfo.mFile.filename().string());
            fmtFileLogInfo.replace(fmtFileLogInfo.find("{}"), 2, fileLogInfo.mFunction);
            fmtFileLogInfo.replace(fmtFileLogInfo.find("{}"), 2, fileLogInfo.mLine);

            return fmtFileLogInfo;
        }
    } // namespace FileLogInfo

    namespace FileLogImpl
    {   
        namespace Impl
        {
            struct FileLogger
            {
                std::ios_base::openmode mOpenMode;
                std::filesystem::path mPath;
                std::ofstream mStream;
            };

            static FileLogger DefaultFileLogger(std::ios::app);
            static std::unordered_map<std::string, FileLogger> FileLoggers;
        }

        Impl::FileLogger& GetFileLogger(std::string_view FileLoggerName)
        {
            if(FileLoggerName.empty())
                return Impl::DefaultFileLogger;

            auto it = Impl::FileLoggers.find(std::string(FileLoggerName));
            if(it != Impl::FileLoggers.end())
                return it->second;
            else
                return Impl::DefaultFileLogger;
        }

        void log(const char* logLevel, std::string_view msg, const std::source_location& src = std::source_location::current())
        {
            std::string fmtFileLogInfo = FileLogInfo::getFmtFileLogInfo(FileLogInfo::getFileLogInfo(src));

            if(State::UseDefaultFileLog)
            {
                if(!Impl::DefaultFileLogger.mStream.is_open())
                    Impl::DefaultFileLogger.mStream.open(Impl::DefaultFileLogger.mPath, Impl::DefaultFileLogger.mOpenMode);

                Impl::DefaultFileLogger.mStream << logLevel << "\t: " << fmtFileLogInfo << " : " << msg << std::endl;
            }
            else
            {
                Impl::FileLogger& fileLogger = GetFileLogger(State::FileLoggerName);
                if(!fileLogger.mStream.is_open())
                    fileLogger.mStream.open(fileLogger.mPath, fileLogger.mOpenMode);

                fileLogger.mStream << logLevel << "\t: " << fmtFileLogInfo << " : " << msg << std::endl;
            }
        }
    }

    namespace Colorize
    {

        struct Colorize
        {
            std::string str;
            std::string color;
            bool replaceAllMatching = false;
        };

        Colorize colorize(std::string_view str, const char* color, bool replaceAllMatching = false)
        {
            return Colorize { 
                .str = std::string(str),
                .color = std::string(color),
                .replaceAllMatching = replaceAllMatching
            };
        }

        void createColorizedString(StringHelper::ColorizedString& str, const std::vector<Colorize>& colorizedStrings)
        {
            for(const auto& colorizedString : colorizedStrings)
            {
                str.setColor(colorizedString.str, colorizedString.color.c_str(), colorizedString.replaceAllMatching);
            }
            str.colorize();
        }
    } // namespace Colorize

    void SetState(State::StateEnum state, bool isEnabled)
    {
        switch(state)
        {
            using enum eLog::State::StateEnum;
            case TERMINAL_LOG:
                State::IsConsoleLogEnabled = isEnabled;
                break;
            case FILE_LOG:
                State::IsFileLogEnabled = isEnabled;
                break;
            case DEFAULT_FILE_LOG:
                State::UseDefaultFileLog = isEnabled;
                break;
        }
    }

    void SetDefaultFileLogPath(const std::filesystem::path& path)
    {
        FileLogImpl::Impl::DefaultFileLogger.mPath = path;
    }

    void UseFileLogger(std::string_view FileLoggerName)
    {
        State::FileLoggerName = FileLoggerName;
    }

    bool AddCustomFileLogger(std::string_view FileLoggerName, const std::filesystem::path& path, std::ios_base::openmode openMode = std::ios::app)
    {
        auto [it, success] = FileLogImpl::Impl::FileLoggers.try_emplace(std::string(FileLoggerName),
                            openMode, path, std::ofstream(path, openMode));
        return success;
    }

    void logDebug(std::string_view msg, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
            LogImpl::log(LogLevel::DEBUG, msg, src);
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::DEBUG, msg, src);
    }

    void logInfo(std::string_view msg, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
            LogImpl::log(LogLevel::INFO, msg, src);
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::INFO, msg, src);
    }

    void logWarning(std::string_view msg, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
            LogImpl::log(LogLevel::WARNING, msg, src);
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::WARNING, msg, src);
    }

    void logError(std::string_view msg, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
            LogImpl::log(LogLevel::ERROR, msg, src);
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::ERROR, msg, src);
    }

    void logFatal(std::string_view msg, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
            LogImpl::log(LogLevel::FATAL, msg, src);
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::FATAL, msg, src);
    }

    void logDebug(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
        {
            StringHelper::ColorizedString colorizedString(msg);
            Colorize::createColorizedString(colorizedString, colorizeStrings);
            LogImpl::log(LogLevel::DEBUG, colorizedString, src);
        }
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::DEBUG, msg, src);
    }

    void logInfo(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
        {
            StringHelper::ColorizedString colorizedString(msg);
            Colorize::createColorizedString(colorizedString, colorizeStrings);  
            LogImpl::log(LogLevel::INFO, colorizedString, src);
        }
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::INFO, msg, src);
    }

    void logWarning(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
        {
            StringHelper::ColorizedString colorizedString(msg);
            Colorize::createColorizedString(colorizedString, colorizeStrings);
            LogImpl::log(LogLevel::WARNING, colorizedString, src);
        }
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::WARNING, msg, src);
    }

    void logError(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, const std::source_location& src = std::source_location::current())
    {
        if(State::IsConsoleLogEnabled)
        {
            StringHelper::ColorizedString colorizedString(msg);
            Colorize::createColorizedString(colorizedString, colorizeStrings);
            LogImpl::log(LogLevel::ERROR, colorizedString, src);
        }
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::ERROR, msg, src);
    }

    void logFatal(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, const std::source_location& src = std::source_location::current())
    {   
        if(State::IsConsoleLogEnabled)
        {
            StringHelper::ColorizedString colorizedString(msg);
            Colorize::createColorizedString(colorizedString, colorizeStrings);
            LogImpl::log(LogLevel::FATAL, colorizedString, src);
        }
        if(State::IsFileLogEnabled)
            FileLogImpl::log(FileLogLevel::FATAL, msg, src);
    }
} // namespace eLog
#endif
