#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <stdint.h>
#include <cstring>
#ifdef __clang__
#include <experimental/source_location>
#else
#include <source_location>
#endif
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include <ctime>

#ifdef __clang__
    using SourceLoc = std::experimental::source_location;
#else
    using SourceLoc = std::source_location;
#endif


namespace eLog {
    namespace AsciiColor
    {
        enum class ColorEnum
        {
            RESET,
            BLACK,
            RED,
            GREEN,
            YELLOW,
            BLUE,
            MAGENTA,
            CYAN,
            WHITE,
            BOLD_BLACK,
            BOLD_RED,
            BOLD_GREEN,
            BOLD_YELLOW,
            BOLD_BLUE,
            BOLD_MAGENTA,
            BOLD_CYAN,
            BOLD_WHITE
        };

        const std::unordered_map<ColorEnum, std::string> AsciiColors = {
            {ColorEnum::RESET, "\033[0m"},
            {ColorEnum::BLACK, "\033[30m"},
            {ColorEnum::RED, "\033[31m"},
            {ColorEnum::GREEN, "\033[32m"},
            {ColorEnum::YELLOW, "\033[33m"},
            {ColorEnum::BLUE, "\033[34m"},
            {ColorEnum::MAGENTA, "\033[35m"},
            {ColorEnum::CYAN, "\033[36m"},
            {ColorEnum::WHITE, "\033[37m"},
            {ColorEnum::BOLD_BLACK, "\033[1m\033[30m"},
            {ColorEnum::BOLD_RED, "\033[1m\033[31m"},
            {ColorEnum::BOLD_GREEN, "\033[1m\033[32m"},
            {ColorEnum::BOLD_YELLOW, "\033[1m\033[33m"},
            {ColorEnum::BOLD_BLUE, "\033[1m\033[34m"},
            {ColorEnum::BOLD_MAGENTA, "\033[1m\033[35m"},
            {ColorEnum::BOLD_CYAN, "\033[1m\033[36m"},
            {ColorEnum::BOLD_WHITE, "\033[1m\033[37m"}
        };

        static bool CheckIfColorIsSupported()
        {
            if (const char* term = std::getenv("TERM"); term == nullptr)
                return false;
            return true;
        }
    } // namespace AsciiColor

    namespace StringHelper
    {
        struct StringHash
        {
            using is_transparent = void;
            std::size_t operator()(std::string_view sv) const
            {
                std::hash<std::string_view> hasher;
                return hasher(sv);
            }
        };

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

            void setColor(std::string_view strToColorize, AsciiColor::ColorEnum color, bool replaceAllMatching = false)
            {
                if (strToColorize.empty() && !AsciiColor::AsciiColors.contains(color))
                    return;

                std::string prevColor = AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::RESET);
                std::size_t lastColorPos = 0;

                if(!mReplaceStrings.empty())
                {
                    prevColor = mReplaceStrings.back().mPrevColor;
                    lastColorPos = mReplaceStrings.back().mPosEndColor;
                }

                std::string replace;
                if(lastColorPos < mStr.find(strToColorize))
                    replace = std::string(AsciiColor::AsciiColors.at(color)).append(strToColorize).append(AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::RESET));
                else
                    replace = std::string(AsciiColor::AsciiColors.at(color)).append(strToColorize).append(prevColor);

                prevColor = AsciiColor::AsciiColors.at(color);

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

        std::string getCurrentDateAsString()
        {
            std::time_t currentTime = std::time(nullptr);
            std::tm* localTime = std::localtime(&currentTime);
            std::ostringstream oss;
            oss << std::put_time(localTime, "%b %d %Y");
            return oss.str();
        }

        std::string getCurrentTimeAsString()
        {
            std::time_t currentTime = std::time(nullptr);
            std::tm* localTime = std::localtime(&currentTime);
            std::ostringstream oss;
            oss << std::put_time(localTime, "%H:%M:%S");
            return oss.str();
            
        }
    } // namespace StringHelper

    namespace State
    {
        namespace Impl
        {
            struct Data
            {
                static std::mutex mtx;
                static std::string FileLoggerName;
                static bool IsFileLogEnabled;
                static bool IsConsoleLogEnabled;
                static bool IsColorEnabled;
                static bool UseDefaultFileLog;
                static bool DirectFlush;
                static bool BufferLogEnabled;
                static bool BufferLog;
                static bool BufferLogLabel;
                static bool BufferFileLog;
                static bool BufferFileLogLabel;
            };

            std::mutex Data::mtx;
            std::string Data::FileLoggerName = "";
            bool Data::IsFileLogEnabled = false;
            bool Data::IsConsoleLogEnabled = true;
            bool Data::IsColorEnabled = AsciiColor::CheckIfColorIsSupported();
            bool Data::UseDefaultFileLog = true;
            bool Data::DirectFlush = false;
            bool Data::BufferLogEnabled = false;
            bool Data::BufferLog = false;
            bool Data::BufferLogLabel = false;
            bool Data::BufferFileLog = false;
            bool Data::BufferFileLogLabel = false;

            bool IsBuffering() {
                return (Data::BufferLog || Data::BufferLogLabel || Data::BufferFileLog || Data::BufferFileLogLabel);
            }
        } // namespace Impl

        enum class StateEnum
        {
            TERMINAL_LOG,
            FILE_LOG,
            DEFAULT_FILE_LOG,
            DIRECT_FLUSH,
            BUFFER_LOG,
            BUFFER_LOG_LABEL,
            BUFFER_FILE_LOG,
            BUFFER_FILE_LOG_LABEL
        };
    } // namespace State

    namespace LogLabel
    {
        namespace Impl
        {
            using Label = std::string_view;

            struct Data
            {
                static std::string mLabelColor;
            };

            std::string Data::mLabelColor = AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::BOLD_WHITE);
        } // namespace Impl

        std::string getLabelStringLog(const Impl::Label& label)
        {
            return std::string(Impl::Data::mLabelColor).append("[").append(label).append("]").append(AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::RESET));
        }

        std::string getLabelStringFileLog(const Impl::Label& label)
        {
            return std::string("[").append(label).append("]");
        }
    } // namespace LogLabel

    namespace LogLevel
    {
        using LogLevel = std::string;

        std::unordered_map<std::string, AsciiColor::ColorEnum> LogLevels = {
            {"TRACE", AsciiColor::ColorEnum::BOLD_CYAN},
            {"DEBUG", AsciiColor::ColorEnum::BOLD_BLUE},
            {"INFO", AsciiColor::ColorEnum::BOLD_GREEN},
            {"WARNING", AsciiColor::ColorEnum::BOLD_YELLOW},
            {"ERROR", AsciiColor::ColorEnum::BOLD_RED},
            {"FATAL", AsciiColor::ColorEnum::BOLD_MAGENTA}
        };

        std::string getLogLevelString(const LogLevel& logLevel, bool colorize = true)
        {
            auto it = LogLevels.find(logLevel);
            if(colorize)
            {
                if(it != LogLevels.end())
                    return std::string(AsciiColor::AsciiColors.at(it->second)).append(logLevel).append(AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::RESET));
                else
                    return std::string(AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::BOLD_WHITE)).append("UNKNOWN").append(AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::RESET));
            }
            else
            {
                if(it != LogLevels.end())
                    return logLevel;
                else
                    return "UNKNOWN";
            }
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

#ifdef __clang__
        LogInfo getLogInfo(const std::experimental::source_location& src)
#else
        LogInfo getLogInfo(const std::source_location& src)
#endif
        {
            return LogInfo {
                .mColor = AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::BOLD_WHITE),
                .mFile = src.file_name(),
                .mFunction = src.function_name(),
                .mLine = std::to_string(src.line()),
                .mDate = StringHelper::getCurrentDateAsString(),
                .mTime = StringHelper::getCurrentTimeAsString(),
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
            fmtLogInfo.replace(fmtLogInfo.find("{}"), 2, AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::RESET));

            return fmtLogInfo;
        }
    } // namespace LogInfo

    namespace LogImpl
    {
        namespace Impl
        {
            struct Data
            {
                static std::mutex mtx;
            };

            std::mutex Data::mtx;
        } // namespace Impl

        void log(const LogLevel::LogLevel& logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::string logLevelString = LogLevel::getLogLevelString(logLevel);
            std::string fmtLogInfo = LogInfo::getFmtLogInfo(LogInfo::getLogInfo(src));
            std::string labelString;
            if(label != "default")
            {
                labelString = LogLabel::getLabelStringLog(label);
                std::cout << logLevelString << "\t: " << labelString << " " << fmtLogInfo << " : " << msg << std::endl;
            }
            else
                std::cout << logLevelString << "\t: " << fmtLogInfo << " : " << msg << std::endl;

            if(State::Impl::Data::DirectFlush)
                std::cout.flush();
        }

        void log(const LogLevel::LogLevel& logLevel, const StringHelper::ColorizedString& colorizedString, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::string logLevelString = LogLevel::getLogLevelString(logLevel);
            std::string fmtLogInfo = LogInfo::getFmtLogInfo(LogInfo::getLogInfo(src));
            std::string labelString = "";
            if(label != "default")
            {
                labelString = LogLabel::getLabelStringLog(label);
                std::cout << logLevelString << "\t: " << labelString << " " << fmtLogInfo << " : " << colorizedString << std::endl;
            }
            else
                std::cout << logLevelString << "\t: " << fmtLogInfo << " : " << colorizedString << std::endl;

            if(State::Impl::Data::DirectFlush)
                std::cout.flush();
        }
    } // namespace LogImpl

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

#ifdef __clang__
        FileLogInfo getFileLogInfo(const std::experimental::source_location& src)
#else
        FileLogInfo getFileLogInfo(std::source_location src)
#endif
        {
            return FileLogInfo {
                .mFile = src.file_name(),
                .mFunction = src.function_name(),
                .mLine = std::to_string(src.line()),
                .mDate = StringHelper::getCurrentDateAsString(),
                .mTime = StringHelper::getCurrentTimeAsString(),
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
                std::mutex mtx;

                FileLogger() = default;

                FileLogger(std::ios_base::openmode openMode, const std::filesystem::path& path)
                : mOpenMode(openMode), mPath(path)
                {}
            };

            struct Data
            {
                static FileLogger DefaultFileLogger;
                static std::unordered_map<std::string, FileLogger, StringHelper::StringHash, std::equal_to<>> FileLoggers;
            };

            FileLogger Data::DefaultFileLogger = FileLogger{std::ios_base::app, "log.txt"};
            std::unordered_map<std::string, FileLogger, StringHelper::StringHash, std::equal_to<>> Data::FileLoggers;
        } // namespace Impl

        Impl::FileLogger& GetFileLogger(std::string_view FileLoggerName)
        {
            if(FileLoggerName.empty())
                return Impl::Data::DefaultFileLogger;

            auto it = Impl::Data::FileLoggers.find(std::string(FileLoggerName));
            if(it != Impl::Data::FileLoggers.end())
                return it->second;
            else
                return Impl::Data::DefaultFileLogger;
        }

        void log(const LogLevel::LogLevel& logLevel, std::string_view msg, LogLabel::Impl::Label label, SourceLoc src)
        {
            std::string fmtFileLogInfo = FileLogInfo::getFmtFileLogInfo(FileLogInfo::getFileLogInfo(src));
            std::string labelString;
            if(label != "default")
                labelString = LogLabel::getLabelStringFileLog(label);

            std::string level = LogLevel::getLogLevelString(logLevel, false);
            
            if(State::Impl::Data::UseDefaultFileLog)
            {
                std::scoped_lock lock(Impl::Data::DefaultFileLogger.mtx);

                if(!Impl::Data::DefaultFileLogger.mStream.is_open())
                    Impl::Data::DefaultFileLogger.mStream.open(Impl::Data::DefaultFileLogger.mPath, Impl::Data::DefaultFileLogger.mOpenMode);
                
                if(labelString.empty())
                    Impl::Data::DefaultFileLogger.mStream << level << "\t: " << fmtFileLogInfo << " : " << msg << std::endl;
                else
                    Impl::Data::DefaultFileLogger.mStream << level << "\t: " << labelString << " " << fmtFileLogInfo << " : " << msg << std::endl;
                    
                if(State::Impl::Data::DirectFlush)
                    Impl::Data::DefaultFileLogger.mStream.flush();
            }
            else
            {
                Impl::FileLogger& fileLogger = GetFileLogger(State::Impl::Data::FileLoggerName);
                std::scoped_lock lock(fileLogger.mtx);

                if(!fileLogger.mStream.is_open())
                    fileLogger.mStream.open(fileLogger.mPath, fileLogger.mOpenMode);

                if(labelString.empty())
                    fileLogger.mStream << level << "\t: " << fmtFileLogInfo << " : " << msg << std::endl;
                else
                    fileLogger.mStream << level << "\t: " << labelString << " " << fmtFileLogInfo << " : " << msg << std::endl;

                if(State::Impl::Data::DirectFlush)
                    fileLogger.mStream.flush();
            }
        } 
    } // namespace FileLogImpl

    namespace LogBufferImpl
    {
        namespace Impl
        {
            struct Data
            {
                static std::mutex mtx;
                static std::vector<std::string> mLogBuffer;
                static std::vector<std::string> mFileLogBuffer;
                static std::unordered_map<std::string, std::vector<std::string>> mLogBufferLabel;
                static std::unordered_map<std::string, std::vector<std::string>> mFileLogBufferLabel;
            };

            std::mutex Data::mtx;
            std::vector<std::string> Data::mLogBuffer;
            std::vector<std::string> Data::mFileLogBuffer;
            std::unordered_map<std::string, std::vector<std::string>> Data::mLogBufferLabel;
            std::unordered_map<std::string, std::vector<std::string>> Data::mFileLogBufferLabel;
        } // namespace Impl

        void log(LogLevel::LogLevel level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::string logLevelString = LogLevel::getLogLevelString(level);
            std::string fmtLogInfo = LogInfo::getFmtLogInfo(LogInfo::getLogInfo(src));
            std::string labelString;
            if(label != "default")
                labelString = LogLabel::getLabelStringLog(label);

            if(labelString.empty())
            {
                if(State::Impl::Data::BufferLog)
                    Impl::Data::mLogBuffer.emplace_back(logLevelString).append("\t: ").append(fmtLogInfo).append(" : ").append(msg).append("\n");
                if(State::Impl::Data::BufferLogLabel)
                    Impl::Data::mLogBufferLabel[std::string(label)].emplace_back(logLevelString).append("\t: ").append(fmtLogInfo).append(" : ").append(msg).append("\n");
            }
            else
            {
                if(State::Impl::Data::BufferLog)
                    Impl::Data::mLogBuffer.emplace_back(logLevelString).append("\t: ").append(labelString).append(" ").append(fmtLogInfo).append(" : ").append(msg).append("\n");
                if(State::Impl::Data::BufferLogLabel)
                    Impl::Data::mLogBufferLabel[std::string(label)].emplace_back(logLevelString).append("\t: ").append(labelString).append(" ").append(fmtLogInfo).append(" : ").append(msg).append("\n");
            }    
        }

        void fileLog(const LogLevel::LogLevel& logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::string fmtFileLogInfo = FileLogInfo::getFmtFileLogInfo(FileLogInfo::getFileLogInfo(src));
            std::string labelString;
            if(label != "default")
                labelString = LogLabel::getLabelStringFileLog(label);

            std::string level = LogLevel::getLogLevelString(logLevel, false);

            if(labelString.empty())
            {
                if(State::Impl::Data::BufferFileLog)
                    Impl::Data::mFileLogBuffer.emplace_back(level).append("\t: ").append(fmtFileLogInfo).append(" : ").append(msg).append("\n");
                if(State::Impl::Data::BufferFileLogLabel)
                    Impl::Data::mFileLogBufferLabel[std::string(label)].emplace_back(level).append("\t: ").append(fmtFileLogInfo).append(" : ").append(msg).append("\n");
            }
            else
            {
                if(State::Impl::Data::BufferFileLog)
                    Impl::Data::mFileLogBuffer.emplace_back(level).append("\t: ").append(labelString).append(" ").append(fmtFileLogInfo).append(" : ").append(msg).append("\n");
                if(State::Impl::Data::BufferFileLogLabel)
                    Impl::Data::mFileLogBufferLabel[std::string(label)].emplace_back(level).append("\t: ").append(labelString).append(" ").append(fmtFileLogInfo).append(" : ").append(msg).append("\n");
            }
        }
    } // namespace BufferLogImpL

    namespace Colorize
    {
        struct Colorize
        {
            std::string str;
            AsciiColor::ColorEnum color;
            bool replaceAllMatching = false;
        };

        Colorize colorize(std::string_view str, AsciiColor::ColorEnum color, bool replaceAllMatching = false)
        {
            return Colorize { 
                .str = std::string(str),
                .color = color,
                .replaceAllMatching = replaceAllMatching
            };
        }

        void createColorizedString(StringHelper::ColorizedString& str, const std::vector<Colorize>& colorizedStrings)
        {
            for(const auto& colorizedString : colorizedStrings)
            {
                str.setColor(colorizedString.str, colorizedString.color, colorizedString.replaceAllMatching);
            }
            str.colorize();
        }
    } // namespace Colorize

    namespace State
    {
        void SetState(State::StateEnum state, bool isEnabled)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);

            switch(state)
            {
                using enum eLog::State::StateEnum;
                case TERMINAL_LOG:
                    State::Impl::Data::IsConsoleLogEnabled = isEnabled;
                    break;
                case FILE_LOG:
                    State::Impl::Data::IsFileLogEnabled = isEnabled;
                    break;
                case DEFAULT_FILE_LOG:
                    State::Impl::Data::UseDefaultFileLog = isEnabled;
                    break;
                case DIRECT_FLUSH:
                    State::Impl::Data::DirectFlush = isEnabled;
                    break;
                case BUFFER_LOG:
                    State::Impl::Data::BufferLog = isEnabled;
                    State::Impl::Data::BufferLogEnabled = State::Impl::IsBuffering();
                    break;
                case BUFFER_LOG_LABEL:
                    State::Impl::Data::BufferLogLabel = isEnabled;
                    State::Impl::Data::BufferLogEnabled = State::Impl::IsBuffering();
                    break;
                case BUFFER_FILE_LOG:
                    State::Impl::Data::BufferFileLog = isEnabled;
                    State::Impl::Data::BufferLogEnabled = State::Impl::IsBuffering();
                    break;
                case BUFFER_FILE_LOG_LABEL:
                    State::Impl::Data::BufferFileLogLabel = isEnabled;
                    State::Impl::Data::BufferLogEnabled = State::Impl::IsBuffering();
                    break;
            }
        }

        void SetDefaultFileLogPath(const std::filesystem::path& path)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            FileLogImpl::Impl::Data::DefaultFileLogger.mPath = path;
        }

        void UseFileLogger(std::string_view FileLoggerName)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            State::Impl::Data::FileLoggerName = FileLoggerName;
        }

        bool AddCustomFileLogger(std::string_view FileLoggerName, const std::filesystem::path& path, std::ios_base::openmode openMode = std::ios::app)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            auto [it, success] = FileLogImpl::Impl::Data::FileLoggers.try_emplace(std::string(FileLoggerName), openMode, path);
            return success;
        }

        std::vector<std::string> GetLogBuffer()
        {
            return LogBufferImpl::Impl::Data::mLogBuffer;
        }

        std::vector<std::string> GetFileLogBuffer()
        {
            return LogBufferImpl::Impl::Data::mFileLogBuffer;
        }

        std::unordered_map<std::string, std::vector<std::string>> GetLogBufferLabel()
        {
            return LogBufferImpl::Impl::Data::mLogBufferLabel;
        }

        std::unordered_map<std::string, std::vector<std::string>> GetFileLogBufferLabel()
        {
            return LogBufferImpl::Impl::Data::mFileLogBufferLabel;
        }

            std::vector<std::string> GetLogBufferByLabel(LogLabel::Impl::Label label)
        {
            if(LogBufferImpl::Impl::Data::mLogBufferLabel.contains(std::string(label)))
                return LogBufferImpl::Impl::Data::mLogBufferLabel[std::string(label)];
            return std::vector<std::string>();
        }

        std::vector<std::string> GetFileLogBufferByLabel(LogLabel::Impl::Label label)
        {
            if(LogBufferImpl::Impl::Data::mFileLogBufferLabel.contains(std::string(label)))
                return LogBufferImpl::Impl::Data::mFileLogBufferLabel[std::string(label)];
            return std::vector<std::string>();
        }

        bool AddLogLevel(std::string_view logLevel, AsciiColor::ColorEnum color)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            auto [it, success] = LogLevel::LogLevels.try_emplace(std::string(logLevel), color);
            return success;
        }
    } // namespace State

    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        if(State::Impl::Data::IsConsoleLogEnabled)
            LogImpl::log(level, msg, label, src);
        if(State::Impl::Data::IsFileLogEnabled)
            FileLogImpl::log(level, msg, label, src);
        if(State::Impl::Data::BufferLogEnabled)
        {
            LogBufferImpl::log(level, msg, label, src);
            LogBufferImpl::fileLog(level, msg, label, src);
        }
    }

    void logTrace(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("TRACE", msg, label, src);
    }

    void logDebug(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("DEBUG", msg, label, src);
    }

    void logInfo(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("INFO", msg, label, src);
    }

    void logWarning(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("WARNING", msg, label, src);
    }

    void logError(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("ERROR", msg, label, src);
    }

    void logFatal(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("FATAL", msg, label, src);
    }

    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        StringHelper::ColorizedString colorizedString(msg);
        Colorize::createColorizedString(colorizedString, colorizeStrings);

        logCustom(level, colorizedString.getColorizedString(), label, src);
    }

    void logTrace(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("TRACE", msg, label, src);
    }

    void logDebug(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("DEBUG", msg, label, src);
    }

    void logInfo(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("INFO", msg, label, src);
    }

    void logWarning(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("WARNING", msg, label, src);
    }

    void logError(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("ERROR", msg, label, src);
    }

    void logFatal(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {   
        logCustom("FATAL", msg, label, src);
    }
} // namespace eLog
