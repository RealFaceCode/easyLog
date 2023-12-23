#include "EasyLog.hpp"

namespace eLog
{
    namespace AsciiColor
    {
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

        const std::string_view ResetColor(AsciiColors.at(ColorEnum::RESET));

        bool CheckIfColorIsSupported()
        {
            if (const char* term = std::getenv("TERM"); term == nullptr)
                return false;
            return true;
        }
    } // namespace AsciiColor

    namespace StringHelper
    {
        std::size_t StringHash::operator()(std::string_view sv) const
        {
            std::hash<std::string_view> hasher;
            return hasher(sv);
        }

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

        std::size_t findStandaloneNextMatchPosition(std::string_view str, std::string_view match, std::size_t pos)
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

        void Replace(std::string& str, std::string_view match, std::string_view replace, bool replaceAllMatching)
        {
            std::size_t pos = findStandaloneNextMatchPosition(str, match, 0);
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

        ColorizedString::ColorizedString(std::string_view str) 
        : mStr(str) {}

        void ColorizedString::setContext(std::string_view str) 
        {
            mStr = str;
        }

        void ColorizedString::setColor(std::string_view strToColorize, AsciiColor::ColorEnum color, bool replaceAllMatching)
        {
            if (strToColorize.empty() && !AsciiColor::AsciiColors.contains(color))
                return;

            std::stringbuf prevColor;
            prevColor.sputn(AsciiColor::ResetColor.data(), AsciiColor::ResetColor.length());
            std::size_t lastColorPos = 0;

            if(!mReplaceStrings.empty())
            {
                prevColor = std::move(mReplaceStrings.back().mPrevColor);
                lastColorPos = mReplaceStrings.back().mPosEndColor;
            }

            std::stringbuf replace;
            auto c = AsciiColor::AsciiColors.at(color);
            replace.sputn(c.data(), c.length());
            replace.sputn(strToColorize.data(), strToColorize.length());
            replace.sputn(prevColor.view().data(), prevColor.in_avail());

            if(lastColorPos < mStr.find(strToColorize))
                replace.sputn(AsciiColor::AsciiColors.begin()->second.data(), AsciiColor::AsciiColors.begin()->second.length());

            prevColor.str("");
            prevColor.sputn(c.data(), c.length());

            StringHelper::ReplaceString replaceString;
            replaceString.mBaseString.str(strToColorize.data());
            replaceString.mReplaceString = std::move(replace);
            replaceString.mPrevColor = std::move(prevColor);
            replaceString.mPosEndColor = mStr.find(strToColorize) + strToColorize.length();
            replaceString.mReplaceAllMatching = replaceAllMatching;

            mReplaceStrings.push_back(std::move(replaceString));
        }

        void ColorizedString::ColorizedString::colorize() 
        {
            for(const auto& replace : mReplaceStrings)
            {
                StringHelper::Replace(mStr, replace.mBaseString.view(), replace.mReplaceString.view(), replace.mReplaceAllMatching);
            }
        }

        const std::string& ColorizedString::getColorizedString() const 
        {
            return mStr;
        }

        std::string_view ColorizedString::view() const
        {
            return mStr;
        }

        void ColorizedString::print() const {
            std::cout << getColorizedString() << std::endl;
        }

        std::ostream& operator<<(std::ostream& os, const ColorizedString& colorizedString)
        {
            os << colorizedString.getColorizedString();
            return os;
        }

        std::string getCurrentTD(std::string_view format) // "%b %d %Y" "%H:%M:%S"
        {
            auto currentTime = std::chrono::system_clock::now();
            auto localTime = std::chrono::system_clock::to_time_t(currentTime);
            std::ostringstream oss;
            
            std::tm result;
            #ifdef _WIN32
                localtime_s(&result, &localTime);
            #else
                localtime_r(&localTime, &result);
            #endif
            
            oss << std::put_time(&result, format.data());
            return oss.str();
        }
    } // namespace StringHelper

    namespace State
    {
        namespace Impl
        {
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
            bool Data::ThreadedLog = false;
            std::size_t Data::BufferSize = 100;
            bool Data::UseTime = true;
            bool Data::UseDate = true;
            bool Data::UseFile = true;
            bool Data::UseFunction = true;
            bool Data::UseLine = true;
            bool Data::COLORLESS = false;

            bool IsBuffering()
            {
                return (Data::BufferLog || Data::BufferLogLabel || Data::BufferFileLog || Data::BufferFileLogLabel);
            }

            bool UseFormat()
            {
                return (Data::UseTime || Data::UseDate || Data::UseFile || Data::UseFunction || Data::UseLine);
            }
        } // namespace Impl
    } // namespace State

    namespace Colorize
    {
        Colorize colorize(std::string_view str, AsciiColor::ColorEnum color, bool replaceAllMatching)
        {
            return Colorize { 
                .str = std::string(str),
                .color = color,
                .replaceAllMatching = replaceAllMatching
            };
        }

        void createColorizedString(StringHelper::ColorizedString& str, const std::vector<Colorize>& colorizedStrings)
        {
            if(!State::Impl::Data::COLORLESS)
            {
                for(const auto& colorizedString : colorizedStrings)
                {
                    str.setColor(colorizedString.str, colorizedString.color, colorizedString.replaceAllMatching);
                }
                str.colorize();
            }
        }
    } // namespace Colorize

    namespace LogLabel
    {

        namespace Impl
        {
            std::string_view Data::mLabelColor(AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::BOLD_WHITE));
        } // namespace Impl

        void getLabelStringLog(std::stringbuf& buf, Impl::Label label)
        {
            std::stringbuf labelString;
            buf.sputc('[');
            buf.sputn(label.data(), label.length());
            buf.sputc(']');
        }
    } // namespace LogLabel

    namespace LogLevel
    {
        namespace Impl
        {
            
            std::unordered_map<std::string, AsciiColor::ColorEnum, StringHelper::StringHash, std::equal_to<>> Data::LogLevels = {
                {"TRACE", AsciiColor::ColorEnum::BOLD_CYAN},
                {"DEBUG", AsciiColor::ColorEnum::BOLD_BLUE},
                {"INFO", AsciiColor::ColorEnum::BOLD_GREEN},
                {"WARNING", AsciiColor::ColorEnum::BOLD_YELLOW},
                {"ERROR", AsciiColor::ColorEnum::BOLD_RED},
                {"FATAL", AsciiColor::ColorEnum::BOLD_MAGENTA}
            }; // namespace Impl

            void FillColor(std::stringbuf& logLevelString, eLog::AsciiColor::ColorEnum color)
            {
                auto c = AsciiColor::AsciiColors.at(color);
                logLevelString.sputn(c.data(), c.length());
            }
        } // namespace Impl

        void getLogLevelString(std::stringbuf& logLevelString, LogLevel logLevel, bool colorize)
        {
            auto it = Impl::Data::LogLevels.find(logLevel);
            if(colorize && !State::Impl::Data::COLORLESS)
            {
                if(it != Impl::Data::LogLevels.end())
                {
                    Impl::FillColor(logLevelString, it->second);
                    logLevelString.sputn(logLevel.data(), logLevel.length());
                }
                else
                {
                    Impl::FillColor(logLevelString, AsciiColor::ColorEnum::BOLD_WHITE);
                    logLevelString.sputn("UNKNOWN", 7);
                }
                
                logLevelString.sputn(AsciiColor::ResetColor.data(), AsciiColor::ResetColor.length());
            }
            else
            {
                if(it != Impl::Data::LogLevels.end())
                    logLevelString.sputn(logLevel.data(), logLevel.length());
                else
                    logLevelString.sputn("UNKNOWN", 8);
            }
        }
    } // namespace LogLevel

    namespace LogInfo
    {
        LogInfo getLogInfo(const SourceLoc& src)
        {
            return LogInfo {
                .mColor = AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::BOLD_WHITE),
                .mFile = src.file_name(),
                .mFunction = src.function_name(),
                .mLine = std::to_string(src.line()),
                .mDate = StringHelper::getCurrentTD("%b %d %Y"),
                .mTime = StringHelper::getCurrentTD("%H:%M:%S"),
            };
        }

        void fillBaseFormat(std::stringbuf& buf, std::string_view file, std::string_view function, std::string_view line, std::string_view date, std::string_view time)
        {
            
            buf.sputn("[", 1);
            if(State::Impl::Data::UseDate)
            {
                buf.sputn(date.data(), date.length());
                buf.sputn(" | ", 3);
            }
            if(State::Impl::Data::UseTime)
            {
                buf.sputn(time.data(), time.length());
                buf.sputn(" | ", 3);
            }
            if(State::Impl::Data::UseFile)
            {
                buf.sputn(file.data(), file.length());
                buf.sputn(" | ", 3);
            }
            if(State::Impl::Data::UseFunction)
            {
                buf.sputn(function.data(), function.length());
                buf.sputn(" | ", 3);
            }
            if(State::Impl::Data::UseLine)
                buf.sputn(line.data(), line.length());
            buf.sputc(']');
            
        }

        void getFmtLogInfo(std::stringbuf& fmtLogInfo, const LogInfo& logInfo, bool colorize)
        {
            if(State::Impl::UseFormat())
            {
                if(colorize && !State::Impl::Data::COLORLESS)
                    fmtLogInfo.sputn(logInfo.mColor.data(), logInfo.mColor.length());
                fillBaseFormat(fmtLogInfo, logInfo.mFile.filename().string(), logInfo.mFunction, logInfo.mLine, logInfo.mDate, logInfo.mTime);
                if(colorize && !State::Impl::Data::COLORLESS)
                    fmtLogInfo.sputn(AsciiColor::ResetColor.data(), AsciiColor::ResetColor.length());
            }
        }
    } // namespace LogInfo

    namespace LogImpl
    {
        namespace Impl
        {
            std::mutex Data::mtx;
        } // namespace Impl

        void fillLogBuffer(std::stringbuf& buf, LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src, bool file)
        {
            std::stringbuf logLevelString;
            std::stringbuf fmtLogInfo;

            if(file)
            {
                LogLevel::getLogLevelString(logLevelString, logLevel, false);
                LogInfo::getFmtLogInfo(fmtLogInfo, LogInfo::getLogInfo(src), false);
            }
            else
            {
                LogLevel::getLogLevelString(logLevelString, logLevel);
                LogInfo::getFmtLogInfo(fmtLogInfo, LogInfo::getLogInfo(src));
            }

            auto vLogLevelString = logLevelString.view();
            auto vFmtLogInfo = fmtLogInfo.view();

            buf.sputn(vLogLevelString.data(), vLogLevelString.length());
            buf.sputn("\t: ", 3);

            if(label != "default")
            {
                std::stringbuf labelString;
                LogLabel::getLabelStringLog(labelString, label);
                auto vLabelString = labelString.view();
                buf.sputn(vLabelString.data(), vLabelString.length());
                if(State::Impl::UseFormat())
                    buf.sputc(' ');
            }

            buf.sputn(vFmtLogInfo.data(), vFmtLogInfo.length());
            buf.sputn(" : ", 3);
            buf.sputn(msg.data(), msg.length());
            buf.sputc('\n');
        }

        void log(LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::stringbuf out;
            fillLogBuffer(out, logLevel, msg, label, src);

            std::cout << out.str();

            if(State::Impl::Data::DirectFlush)
                std::cout.flush();
        }
    } // namespace LogImpl

    namespace FileLogInfo
    {
        FileLogInfo getFileLogInfo(const SourceLoc& src)
        {
            return FileLogInfo {
                .mFile = src.file_name(),
                .mFunction = src.function_name(),
                .mLine = std::to_string(src.line()),
                .mDate = StringHelper::getCurrentTD("%b %d %Y"),
                .mTime = StringHelper::getCurrentTD("%H:%M:%S"),
            };
        }
    } // namespace FileLogInfo

    namespace FileLogImpl
    {   
        namespace Impl
        {


            FileLogger:: FileLogger(std::ios_base::openmode openMode, const std::filesystem::path& path)
            : mOpenMode(openMode), mPath(path)
            {}

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

        void log(LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, SourceLoc src)
        {
            std::stringbuf out;
            LogImpl::fillLogBuffer(out, logLevel, msg, label, src, true);
            auto vOut = out.view();

            if(State::Impl::Data::UseDefaultFileLog)
            {
                std::scoped_lock lock(Impl::Data::DefaultFileLogger.mtx);

                if(!Impl::Data::DefaultFileLogger.mStream.is_open())
                    Impl::Data::DefaultFileLogger.mStream.open(Impl::Data::DefaultFileLogger.mPath, Impl::Data::DefaultFileLogger.mOpenMode);
                
                Impl::Data::DefaultFileLogger.mStream << vOut;
                    
                if(State::Impl::Data::DirectFlush)
                    Impl::Data::DefaultFileLogger.mStream.flush();
            }
            else
            {
                Impl::FileLogger& fileLogger = GetFileLogger(State::Impl::Data::FileLoggerName);
                std::scoped_lock lock(fileLogger.mtx);

                if(!fileLogger.mStream.is_open())
                    fileLogger.mStream.open(fileLogger.mPath, fileLogger.mOpenMode);

                fileLogger.mStream << vOut;

                if(State::Impl::Data::DirectFlush)
                    fileLogger.mStream.flush();
            }
        } 
    } // namespace FileLogImpl

    namespace LogBufferImpl
    {
        namespace Impl
        {
            std::mutex Data::mtx;
            std::vector<std::string> Data::mLogBuffer;
            std::vector<std::string> Data::mFileLogBuffer;
            std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> Data::mLogBufferLabel;
            std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> Data::mFileLogBufferLabel;
        } // namespace Impl

        void log(LogLevel::LogLevel level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::stringbuf out;
            LogImpl::fillLogBuffer(out, level, msg, label, src);

            if(State::Impl::Data::BufferLog)
            {
                if(Impl::Data::mLogBuffer.size() >= Impl::Data::mLogBuffer.capacity())
                    Impl::Data::mLogBuffer.reserve(Impl::Data::mLogBuffer.capacity() + State::Impl::Data::BufferSize);
                Impl::Data::mLogBuffer.emplace_back(out.view());
            }

            if(State::Impl::Data::BufferLogLabel)
            {
                auto& vec = Impl::Data::mLogBufferLabel[std::string(label)];
                if(vec.size() >= vec.capacity())
                    vec.reserve(vec.capacity() + State::Impl::Data::BufferSize);
                vec.emplace_back(out.view());
            }
        }

        void fileLog(LogLevel::LogLevel level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::stringbuf out;
            LogImpl::fillLogBuffer(out, level, msg, label, src);

            if(State::Impl::Data::BufferLog)
            {
                if(Impl::Data::mFileLogBuffer.size() >= Impl::Data::mFileLogBuffer.capacity())
                    Impl::Data::mFileLogBuffer.reserve(Impl::Data::mFileLogBuffer.capacity() + State::Impl::Data::BufferSize);
                Impl::Data::mLogBuffer.emplace_back(out.view());
            }
            
            if(State::Impl::Data::BufferLogLabel)
            {
                auto& vec = Impl::Data::mFileLogBufferLabel[std::string(label)];
                if(vec.size() >= vec.capacity())
                    vec.reserve(vec.capacity() + State::Impl::Data::BufferSize);
                vec.emplace_back(out.view());
            }
        }
    } // namespace BufferLogImpL

    namespace CLogImpl
    {
        void log(const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
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
    } // namespace CLogImpl

    namespace ThreadLog
    {
        namespace Impl
        {
            LogTask makeTask(LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
            {
                std::stringbuf sLevel;
                sLevel.sputn(logLevel.data(), logLevel.size());
                std::stringbuf sLabel;
                sLabel.sputn(label.data(), label.size());
                std::stringbuf sMsg;
                sMsg.sputn(msg.data(), msg.size());

                return LogTask {
                    .mLogLevel = std::make_unique<std::stringbuf>(std::move(sLevel)),
                    .mLabel = std::make_unique<std::stringbuf>(std::move(sLabel)),
                    .mMsg = std::make_unique<std::stringbuf>(std::move(sMsg)),
                    .mSrc = std::make_unique<SourceLoc>(src),
                    .mColorStack = nullptr,
                };
            }

            LogTask makeTask(LogLevel::LogLevel logLevel, std::string_view msg, const std::vector<Colorize::Colorize>& colorStack, LogLabel::Impl::Label label, const SourceLoc& src)
            {
                auto task = makeTask(logLevel, msg, label, src);
                task.mColorStack = std::make_unique<std::vector<Colorize::Colorize>>(colorStack);
                return task;
            }
            
            std::mutex Data::mtx;
            std::queue<LogTask> Data::mLogTasks;
            std::condition_variable Data::mCv;
            std::atomic<bool> Data::mIsRunning = false;
            std::atomic<bool> Data::mIsFinished = false;
            std::jthread Data::mThread;

            void loggerThreadFunc(std::stop_token stoken)
            {
                while(!stoken.stop_requested())
                {
                    std::unique_lock lock(Data::mtx);
                    Data::mCv.wait(lock, []{ return !Data::mLogTasks.empty() || !Data::mIsRunning; });

                    if(Data::mLogTasks.empty())
                        continue;

                    auto task = std::move(Data::mLogTasks.front());
                    Data::mLogTasks.pop();
                    lock.unlock();

                    if(!task.mLogLevel || !task.mLabel || !task.mMsg || !task.mSrc)
                        continue;


                    auto vLogLevel = task.mLogLevel.get()->view();
                    auto vLabel = task.mLabel.get()->view();
                    auto vMsg = task.mMsg.get()->view();

                    StringHelper::ColorizedString colorizedMsg;
                    if(task.mColorStack)
                    {
                        auto colorStack = task.mColorStack.get();
                        colorizedMsg.setContext(vMsg);
                        Colorize::createColorizedString(colorizedMsg, *colorStack);
                        vMsg = colorizedMsg.view();
                    }

                    CLogImpl::log(vLogLevel, vMsg, vLabel, *task.mSrc.get());
                }
                Impl::Data::mIsFinished = true;
                
            }
        } // namespace Impl

        void PushLogTask(Impl::LogTask&& task)
        {
            std::scoped_lock lock(Impl::Data::mtx);
            Impl::Data::mLogTasks.emplace(std::move(task));
            Impl::Data::mCv.notify_one();
        }

        void StartLoggerThread()
        {
            if(!Impl::Data::mIsRunning)
            {
                Impl::Data::mIsRunning = true;
                Impl::Data::mThread = std::jthread(Impl::loggerThreadFunc);
            }
        }

        void StopLoggerThread()
        {
            if(Impl::Data::mIsRunning)
            {
                Impl::Data::mIsRunning = false;
                Impl::Data::mThread.request_stop();
                Impl::Data::mCv.notify_all();
                Impl::Data::mThread.join();
            }
        }

        void WaitLoggerThread()
        {
            if(Impl::Data::mIsRunning)
                Impl::Data::mThread.join();
        }

        bool IsLoggerThreadRunning()
        {
            return Impl::Data::mIsRunning;
        }

        bool IsLoggerThreadFinished()
        {
            return Impl::Data::mIsFinished;
        }
    } // namespace ThreadLog

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
                case THREADED_LOG:
                    State::Impl::Data::ThreadedLog = isEnabled;
                    if(State::Impl::Data::ThreadedLog)
                        ThreadLog::StartLoggerThread();
                    else
                        ThreadLog::StopLoggerThread();
                    break;
                case USE_DATE:
                    State::Impl::Data::UseDate = isEnabled;
                    break;
                case USE_TIME:
                    State::Impl::Data::UseTime = isEnabled;
                    break;
                case USE_FILE:
                    State::Impl::Data::UseFile = isEnabled;
                    break;
                case USE_FUNCTION:
                    State::Impl::Data::UseFunction = isEnabled;
                    break;
                case USE_LINE:
                    State::Impl::Data::UseLine = isEnabled;
                    break;
                case COLORLESS:
                    State::Impl::Data::COLORLESS = isEnabled;
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

        bool AddCustomFileLogger(std::string_view FileLoggerName, const std::filesystem::path& path, std::ios_base::openmode openMode)
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

        std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> GetLogBufferLabel()
        {
            return LogBufferImpl::Impl::Data::mLogBufferLabel;
        }

        std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> GetFileLogBufferLabel()
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

        void ClearLogBuffer()
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            LogBufferImpl::Impl::Data::mLogBuffer.clear();
        }

        void ClearFileLogBuffer()
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            LogBufferImpl::Impl::Data::mFileLogBuffer.clear();
        }

        void ClearLogBufferLabel()
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            LogBufferImpl::Impl::Data::mLogBufferLabel.clear();
        }

        void ClearFileLogBufferLabel()
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            LogBufferImpl::Impl::Data::mFileLogBufferLabel.clear();
        }

        void ClearLogBufferByLabel(LogLabel::Impl::Label label)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            LogBufferImpl::Impl::Data::mLogBufferLabel.erase(std::string(label));
        }

        void ClearFileLogBufferByLabel(LogLabel::Impl::Label label)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            LogBufferImpl::Impl::Data::mFileLogBufferLabel.erase(std::string(label));
        }

        void ClearBuffers()
        {
            ClearLogBuffer();
            ClearFileLogBuffer();
            ClearFileLogBufferLabel();
            ClearLogBufferLabel();    
        }

        bool AddLogLevel(std::string_view logLevel, AsciiColor::ColorEnum color)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            auto [it, success] = LogLevel::Impl::Data::LogLevels.try_emplace(std::string(logLevel), color);
            return success;
        }

        void CloseStream(std::string_view stream)
        {
            if(stream != "")
            {
                std::scoped_lock lock(State::Impl::Data::mtx);
                if(stream == "default")
                {
                    if(FileLogImpl::Impl::Data::DefaultFileLogger.mStream.is_open())
                        FileLogImpl::Impl::Data::DefaultFileLogger.mStream.close();
                }
                else
                {
                    auto it = FileLogImpl::Impl::Data::FileLoggers.find(std::string(stream));
                    if(it != FileLogImpl::Impl::Data::FileLoggers.end() && it->second.mStream.is_open())
                        it->second.mStream.close();
                }
            }
            else
            {
                std::scoped_lock lock(State::Impl::Data::mtx);
                if(FileLogImpl::Impl::Data::DefaultFileLogger.mStream.is_open())
                    FileLogImpl::Impl::Data::DefaultFileLogger.mStream.close();
                for(auto& [name, fileLogger] : FileLogImpl::Impl::Data::FileLoggers)
                    if(fileLogger.mStream.is_open())
                        fileLogger.mStream.close();
            }
        }

        void setDefaultBufferSize(std::size_t size)
        {
            State::Impl::Data::BufferSize = size;
            LogBufferImpl::Impl::Data::mLogBuffer.reserve(size);
            LogBufferImpl::Impl::Data::mFileLogBuffer.reserve(size);

            for(auto& [label, buffer] : LogBufferImpl::Impl::Data::mLogBufferLabel)
                buffer.reserve(size);
            for(auto& [label, buffer] : LogBufferImpl::Impl::Data::mFileLogBufferLabel)
                buffer.reserve(size);
        }
    } // namespace State

    namespace fmt
    {
        namespace Impl
        {
            bool IsNumber(std::string_view str)
            {
                return !str.empty() && std::ranges::all_of(str, ::isdigit);
            }

            std::string stringToHex(const std::string& input)
            {
                if(!IsNumber(input))
                    return "";
                if(input.find(".") != std::string::npos)
                    return "";

                long long int i = std::stoll(input);
                std::stringstream ss;
                ss << std::uppercase << std::hex << i;
                return "0x" + ss.str();
            }
        }
    } // namespace fmt

    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        if(State::Impl::Data::ThreadedLog)
            ThreadLog::PushLogTask(ThreadLog::Impl::makeTask(level, msg, label, src));
        else
            CLogImpl::log(level, msg, label, src);
    }

    void logTrace(std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("TRACE", msg, label, src);
    }

    void logDebug(std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("DEBUG", msg, label, src);
    }

    void logInfo(std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("INFO", msg, label, src);
    }

    void logWarning(std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("WARNING", msg, label, src);
    }

    void logError(std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("ERROR", msg, label, src);
    }

    void logFatal(std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("FATAL", msg, label, src);
    }

    void logIfCustom(bool condition, const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        if(condition)
            logCustom(level, msg, label, src);
    }

    void logIfTrace(bool condition, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "TRACE", msg, label, src);
    }

    void logIfDebug(bool condition, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "DEBUG", msg, label, src);
    }

    void logIfInfo(bool condition, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "INFO", msg, label, src);
    }

    void logIfWarning(bool condition, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "WARNING", msg, label, src);
    }

    void logIfError(bool condition, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "ERROR", msg, label, src);
    }

    void logIfFatal(bool condition, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "FATAL", msg, label, src);
    }

    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, const std::vector<Colorize::Colorize>& colorStack, LogLabel::Impl::Label label, const SourceLoc src)
    {
        if(State::Impl::Data::ThreadedLog)
            ThreadLog::PushLogTask(std::move(ThreadLog::Impl::makeTask(level, msg, colorStack, label, src)));
        else
            CLogImpl::log(level, msg, label, src);
    }

    void logTrace(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("TRACE", msg, colorizeStrings, label, src);
    }

    void logDebug(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("DEBUG", msg, colorizeStrings, label, src);
    }

    void logInfo(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("INFO", msg, colorizeStrings, label, src);
    }

    void logWarning(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("WARNING", msg, colorizeStrings, label, src);
    }

    void logError(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logCustom("ERROR", msg, colorizeStrings, label, src);
    }

    void logFatal(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {   
        logCustom("FATAL", msg, colorizeStrings, label, src);
    }

    void logIfCustom(bool condition, const LogLevel::LogLevel& level, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        if(condition)
            logCustom(level, msg, colorizeStrings, label, src);
    }

    void logIfTrace(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "TRACE", msg, colorizeStrings, label, src);
    }

    void logIfDebug(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "DEBUG", msg, colorizeStrings, label, src);
    }

    void logIfInfo(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {
        logIfCustom(condition, "INFO", msg, colorizeStrings, label, src);
    }

    void logIfWarning(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src)
    {   
        logIfCustom(condition, "WARNING", msg, colorizeStrings, label, src);
    }

    void logIfError(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src) 
    {
        logIfCustom(condition, "ERROR", msg, colorizeStrings, label, src);
    }

    void logIfFatal(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label, const SourceLoc src) 
    {
        logIfCustom(condition, "FATAL", msg, colorizeStrings, label, src);
    }
} // namespace eLog
