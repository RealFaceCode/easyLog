/**
 * @file Easylog.hpp
 * @brief Header file for the easyLog library.
 * 
 * This file contains the declarations of various classes, enums, and functions
 * that make up the easyLog library. The library provides functionality for
 * logging messages to the console and/or a file, with support for colorizing
 * the output and buffering the log messages.
 * 
 * Features:
 * - Logging messages to the console and/or a file
 * - Colorizing the output using ASCII color codes
 * - Buffering log messages for delayed output
 * - Support for different log states, such as terminal log, file log, etc.
 * - Configurable options for enabling/disabling logging, colorization, buffering, etc.
 * - Utility functions for string manipulation and date/time formatting
 * - Support for log labels to categorize log messages
 * 
 * The library is implemented using C++ standard library features such as
 * iostream, string, vector, unordered_map, mutex, etc. It also uses
 * experimental/source_location or source_location (depending on the compiler)
 * for capturing the source location of log messages.
 * 
 * Note: This library assumes that the environment supports ASCII color codes
 * for colorizing the console output. The CheckIfColorIsSupported() function
 * can be used to check if the environment supports colorization.
 */

#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <stdint.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include <ctime>
#include <tuple>
#include <assert.h>
#include <sstream>
#include <algorithm>
#include <memory>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <thread>

#ifdef __clang__
#include <experimental/source_location>
    using SourceLoc = std::experimental::source_location;
#else
#include <source_location>
    using SourceLoc = std::source_location;
#endif

/**
 * @namespace eLog
 * @brief Namespace containing classes and functions for logging and string manipulation.
 *
 * The eLog namespace provides functionality for logging messages to the console or a file,
 * as well as manipulating strings and applying color formatting.
 * It includes sub-namespaces for different aspects of logging and string manipulation.
 * The namespace also defines enums and structs used for configuration and data storage.
 */
namespace eLog
{

    /**
     * @namespace LogUsings
     * @brief Namespace containing using declarationss.
     *
     * The LogUsings namespace contains using declarations.
     * It is used to simplify the syntax for declaring stdlib objects.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     * 
     * @example
     * LogUsings::Ref<std::stringbuf> ptr = LogUsings::MakeRef<std::stringbuf>();
     * LogUsings::Scope<std::stringbuf> ptr = LogUsings::MakeScope<std::stringbuf>();
     */
    namespace LogUsings
    {
        template<typename T>
        using Ref = std::shared_ptr<T>;
        template<typename T>
        using Scope = std::unique_ptr<T>;

        /**
         * @brief Creates a Ref to an object of type T.
         * 
         * This function creates a Ref to an object of type T.
         * 
         * @tparam T The type of the object.
         * @tparam Args The types of the arguments to construct the object.
         * @param args The arguments to construct the object.
         * @return A Ref to the object.
        */
        template<typename T, typename... Args>
        Ref<T> MakeRef(Args&& ... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        /**
         * @brief Creates a Scope to an object of type T.
         * 
         * This function creates a Scope to an object of type T.
         * 
         * @tparam T The type of the object.
         * @tparam Args The types of the arguments to construct the object.
         * @param args The arguments to construct the object.
         * @return A Scope to the object.
        */
        template<typename T, typename... Args>
        Scope<T> MakeScope(Args&& ... args)
        {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }
    } // namespace LogUsings

    /**
     * @namespace AsciiColor
     * @brief Namespace containing utilities for working with ASCII colors.
     */
    namespace AsciiColor
    {
        /**
         * @enum ColorEnum
         * @brief Enumeration representing different ASCII colors.
         */
        enum class ColorEnum
        {
            RESET,          /**< Reset color */
            BLACK,          /**< Black color */
            RED,            /**< Red color */
            GREEN,          /**< Green color */
            YELLOW,         /**< Yellow color */
            BLUE,           /**< Blue color */
            MAGENTA,        /**< Magenta color */
            CYAN,           /**< Cyan color */
            WHITE,          /**< White color */
            BOLD_BLACK,     /**< Bold black color */
            BOLD_RED,       /**< Bold red color */
            BOLD_GREEN,     /**< Bold green color */
            BOLD_YELLOW,    /**< Bold yellow color */
            BOLD_BLUE,      /**< Bold blue color */
            BOLD_MAGENTA,   /**< Bold magenta color */
            BOLD_CYAN,      /**< Bold cyan color */
            BOLD_WHITE      /**< Bold white color */
        };

        /**
         * @brief Checks if the current terminal supports ASCII colors.
         * 
         * This function checks if the current terminal supports ASCII colors.
         * 
         * @return true if ASCII colors are supported, false otherwise.
         */
        bool CheckIfColorIsSupported();
    } // namespace AsciiColor

    /**
     * @namespace StringHelper
     * @brief A namespace that provides various string manipulation utilities.
     *
     * The StringHelper namespace contains functions and classes for performing operations on strings,
     * such as finding standalone matches, replacing substrings, colorizing strings, and getting the current date and time as strings.
     * It also includes helper functions for checking if a character is a punctuation mark.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     */
    namespace StringHelper
    {
        /**
         * @brief Custom hash function object for std::unordered_map with std::string_view as key.
         * 
         * This hash function object is used to hash std::string_view objects when they are used as keys in std::unordered_map.
         * It provides a transparent comparison function, allowing the use of std::string_view objects as keys without the need for explicit conversion.
         * The hash function uses std::hash<std::string_view> internally to compute the hash value.
         * 
         * @note This hash function object is defined within the Easylog.hpp file.
         */
        struct StringHash
        {
            using is_transparent = void;
            std::size_t operator()(std::string_view sv) const;
        };

        /**
         * @brief This class represents a log configuration for the Easylog library.
         * 
         * It stores the base string, replace string, previous color, position of the end color,
         * and a flag indicating whether to replace all matching strings.
         * 
         * @note This struct is defined within the Easylog.hpp file.
         */
        struct ReplaceString
        {
            std::stringbuf mBaseString;
            std::stringbuf mReplaceString;
            std::stringbuf mPrevColor;
            std::size_t mPosEndColor;
            bool mReplaceAllMatching = false;

            /**
             * @brief Default constructor.
             * 
             * This constructor is used to initialize the ReplaceString struct with default values
            */
            ReplaceString() = default;

            /**
             * @brief This Constructor is deleted.
             * 
             * This constructor is deleted to prevent the compiler from generating it.
            */
            ReplaceString(const ReplaceString&) = delete;

            /**
             * @brief This assignment operator is deleted.
             * 
             * This assignment operator is deleted to prevent the compiler from generating it.
            */
            ReplaceString& operator=(const ReplaceString&) = delete;

            /**
             * @brief Move constructor.
             * 
             * This constructor is used to initialize the ReplaceString struct with an rvalue reference.
            */
            ReplaceString(ReplaceString&&) noexcept = default;

            /**
             * @brief Move assignment operator.
             * 
             * This assignment operator is used to assign an rvalue reference to the ReplaceString struct.
            */
            ReplaceString& operator=(ReplaceString&&) noexcept = default;
        };

        /**
         * @brief Checks if a character is a punctuation mark.
         * 
         * This function checks if a character is a punctuation mark, such as a space, comma, period, etc.
         * 
         * @param c The character to check.
         * @return true if the character is a punctuation mark, false otherwise.
         */
        bool isCharPunctuationMark(char c);
    
        /**
         * @brief Finds all positions of standalone matches in a string.
         * 
         * This function finds all positions of standalone matches in a string.
         * A standalone match is a match that is not part of a larger word.
         * For example, if the string is "Hello World!" and the match is "Hello",
         * then the position of the match is 0, since the match is not part of a larger word.
         * 
         * @param str The string to search.
         * @param match The string to match.
         * @return A vector containing the positions of all standalone matches.
         */
        std::vector<size_t> findStandaloneMatchPositions(std::string_view str, std::string_view match);

        /**
         * @brief Finds the next position of a standalone match in a string.
         * 
         * This function finds the next position of a standalone match in a string.
         * A standalone match is a match that is not part of a larger word.
         * For example, if the string is "Hello World!" and the match is "Hello",
         * then the position of the match is 0, since the match is not part of a larger word.
         * 
         * @param str The string to search.
         * @param match The string to match.
         * @param pos The position to start searching from.
         * @return The position of the next standalone match, or std::string::npos if no match was found.
         */
        std::size_t findStandaloneNextMatchPosition(std::string_view str, std::string_view match, std::size_t pos);

        /**
         * @brief Replaces all standalone matches in a string with another string.
         * 
         * This function replaces all standalone matches in a string with another string.
         * A standalone match is a match that is not part of a larger word.
         * For example, if the string is "Hello World!" and the match is "Hello",
         * then the position of the match is 0, since the match is not part of a larger word.
         * 
         * @param str The string to search.
         * @param match The string to match.
         * @param replace The string to replace the match with.
         * @param replaceAllMatching A flag indicating whether to replace all matching strings.
         */
        void Replace(std::string& str, std::string_view match, std::string_view replace, bool replaceAllMatching = false);

        /**
         * @brief The ColorizedString class represents a string that can be colorized with ASCII colors.
         * 
         * This class provides functionality to set the context of the string, set the color of specific substrings,
         * and perform colorization of the string. The colorized string can be retrieved and printed.
         */
        class ColorizedString
        {
        public:

            /**
             * @brief Default constructor.
            */
            ColorizedString() = default;

            /**
             * @brief Constructor that sets the context of the string.
             * 
             * @param str The string to set as the context.
            */
            explicit ColorizedString(std::string_view str);

            /**
             * @brief Sets the context of the string.
             * 
             * @param str The string to set as the context.
            */
            void setContext(std::string_view str);

            /**
             * @brief Sets the color of a substring in the string.
             * 
             * This function sets the color of a substring in the string.
             * The substring is identified by the string to colorize.
             * The color is identified by the ASCII color code.
             * 
             * @param strToColorize The string to colorize.
             * @param color The ASCII color code.
             * @param replaceAllMatching A flag indicating whether to replace all matching strings.
            */
            void setColor(std::string_view strToColorize, AsciiColor::ColorEnum color, bool replaceAllMatching = false);

            /**
             * @brief Colorizes the string.
             * 
             * This function colorizes the string by replacing all substrings with their corresponding colorized substrings.
            */
            void colorize();

            /**
             * @brief Gets the colorized string.
             * 
             * This function returns the colorized string.
             * 
             * @return The colorized string.
            */
            const std::string& getColorizedString() const;

            std::string_view view() const;

            /**
             * @brief Prints the colorized string.
             * 
             * This function prints the colorized string to the console.
            */
            void print() const;

            /**
             * @brief Overloaded operator for printing the colorized string.
             * 
             * This function overloads the << operator for printing the colorized string to the console.
             * 
             * @param os The output stream.
             * @param colorizedString The colorized string.
             * @return The output stream.
            */
            friend std::ostream& operator<<(std::ostream& os, const ColorizedString& colorizedString);

            private:
                std::string mStr;
                std::vector<StringHelper::ReplaceString> mReplaceStrings;
        };

        /**
         * @brief Gets the current date or time as a string.
         * 
         * This function gets the current date or time as a string.
         * 
         * @param format The format of the date string.
         * @return The current date as a string.
        */
        std::string getCurrentTD(std::string_view format); // "%b %d %Y" "%H:%M:%S"
    } // namespace StringHelper

    /**
     * @namespace State
     * @brief Namespace containing enums and structs for storing the state of the Easylog library.
     *
     * The State namespace contains enums and structs for storing the state of the Easylog library.
     * It also includes functions for checking if the library is in a certain state.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     */
    namespace State
    {

        /**
         * @namespace Impl
         * @brief Namespace containing implementation details for the State namespace.
         * 
         * The Impl namespace contains implementation details for the State namespace.
         * It includes structs for storing the state of the Easylog library.
         * It also includes functions for checking if the library is in a certain state.
         * 
         * @note This namespace is defined within the Easylog.hpp file.
        */
        namespace Impl
        {

            /**
             * @struct Data
             * @brief Struct for storing the state of the Easylog library.
             * 
             * This struct stores the state of the Easylog library.
             * It includes static variables for storing the state of the library.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
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
                static bool ThreadedLog;
                static std::size_t BufferSize;
                static bool UseTime;
                static bool UseDate;
                static bool UseFile;
                static bool UseFunction;
                static bool UseLine;
                static bool COLORLESS;
            };

            /**
             * @brief Checks if the library is in the terminal log state.
             * 
             * This function checks if the library is in the terminal log state.
             * 
             * @return true if the library is in the terminal log state, false otherwise.
             * 
             * @note The library is in the terminal log state if any of the following are true:
            */
            bool IsBuffering();

            /**
             * @brief Checks if the format is enabled.
             * 
             * This function checks if the format is enabled.
             * 
             * @return true if the format is enabled, false otherwise.
             * 
             * @note The format is enabled if any of the following are true:
            */
            bool UseFormat();
        } // namespace Impl

        /**
         * @enum StateEnum
         * @brief Enumeration representing different states of the Easylog library.
         * 
         * This enumeration represents different states of the Easylog library.
         * It is used to set the state of the library.
         * 
         * @note This enum is defined within the Easylog.hpp file.
        */
        enum class StateEnum
        {
            TERMINAL_LOG,
            FILE_LOG,
            DEFAULT_FILE_LOG,
            DIRECT_FLUSH,
            BUFFER_LOG,
            BUFFER_LOG_LABEL,
            BUFFER_FILE_LOG,
            BUFFER_FILE_LOG_LABEL,
            THREADED_LOG,
            USE_TIME,
            USE_DATE,
            USE_FILE,
            USE_FUNCTION,
            USE_LINE,
            COLORLESS,
        };
    } // namespace State

    /**
     * @namespace Colorize
     * @brief Namespace containing functions for colorizing strings.
     * 
     * The Colorize namespace contains functions for colorizing strings.
     * It includes functions for colorizing strings.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
    */
    namespace Colorize
    {
        /**
         * @struct Colorize
         * @brief Struct for storing the information for colorizing strings.
         * 
         * This struct stores the information for colorizing strings.
         * It includes the string to colorize, the color to use, and a flag indicating whether to replace all matching strings.
         * 
         * @note This struct is defined within the Easylog.hpp file.
        */
        struct Colorize
        {
            std::string str;
            AsciiColor::ColorEnum color;
            bool replaceAllMatching = false;
        };

        /**
         * @brief Creates the colorize information to colorize a string.
         * 
         * This function creates the colorize information to colorize a string.
         * 
         * @param str The string to colorize.
         * @param color The color to use.
         * @param replaceAllMatching A flag indicating whether to replace all matching strings.
         * @return The colorize information.
        */
        Colorize colorize(std::string_view str, AsciiColor::ColorEnum color, bool replaceAllMatching = false);

        /**
         * @brief Creates the colorized string based on the colorize information.
         * 
         * This function creates the colorized string based on the colorize information.
         * 
         * @param str The colorized string.
         * @param colorizedStrings The colorize information.
        */
        void createColorizedString(StringHelper::ColorizedString& str, const std::vector<Colorize>& colorizedStrings);
    } // namespace Colorize

    /**
     * @namespace LogLabel
     * @brief Namespace containing functions for working with log labels.
     * 
     * The LogLabel namespace contains functions for working with log labels.
     * It includes functions for getting the string representation of a log label.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
    */
    namespace LogLabel
    {

        /**
         * @namespace Impl
         * @brief Namespace containing implementation details for the LogLabel namespace.
         * 
         * The Impl namespace contains implementation details for the LogLabel namespace.
         * It includes structs for storing the state of the Easylog library.
         * It also includes functions for checking if the library is in a certain state.
         * 
         * @note This namespace is defined within the Easylog.hpp file.
        */
        namespace Impl
        {
            using Label = std::string_view;

            /**
             * @struct Data
             * @brief Struct for storing the state of the Easylog library.
             * 
             * This struct stores the state of the Easylog library.
             * It includes static variables for storing the state of the library.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
            struct Data
            {
                static std::string_view mLabelColor;
            };
        } // namespace Impl

        /**
         * @brief Gets the string representation of a log label for terminal logging.
         * 
         * This function gets the string representation of a log label for terminal logging.
         * 
         * @param label The log label.
         * @return The string representation of the log label.
        */
        void getLabelStringLog(std::stringbuf& buf, Impl::Label label);
    } // namespace LogLabel

    /**
     * @namespace LogLevel
     * @brief Namespace containing functions for working with log levels.
     * 
     * The LogLevel namespace contains functions for working with log levels.
     * It includes functions for getting the string representation of a log level.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
    */
    namespace LogLevel
    {
        using LogLevel = std::string_view;

        /**
         * @namespace Impl
         * @brief Namespace containing implementation details for the LogLevel namespace.
         * 
         * The Impl namespace contains implementation details for the LogLevel namespace.
         * 
         * @note This namespace is defined within the Easylog.hpp file.
        */
        namespace Impl
        {
            /**
             * @struct Data
             * @brief Struct for storing the log levels of the Easylog library.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
            struct Data
            {
                static std::unordered_map<std::string, AsciiColor::ColorEnum, StringHelper::StringHash, std::equal_to<>> LogLevels;
            };

            /**
             * @brief Fills the string buffer with the color of the log level.
             * 
             * This function fills the string buffer with the color of the log level.
             * 
             * @param logLevelString The string buffer to fill.
             * @param color The color of the log level.
            */
            void FillColor(std::stringbuf& logLevelString, eLog::AsciiColor::ColorEnum color);
        } // namespace Impl

        /**
         * @brief Gets the string representation of a log level.
         * 
         * This function gets the string representation of a log level.
         * 
         * @param logLevel The log level.
         * @param colorize A flag indicating whether to colorize the log level.
         * @return The string representation of the log level.
        */
        void getLogLevelString(std::stringbuf& logLevelString, LogLevel logLevel, bool colorize = true);
    } // namespace LogLevel

    /**
     * @namespace LogInfo
     * @brief Namespace containing structs and functions for getting log information.
     * 
     * The LogInfo namespace contains structs and functions for getting log information.
     * It includes structs for storing log information, such as the file name, function name, line number, etc.
     * It also includes functions for getting the log information and formatting it.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
    */
    namespace LogInfo
    {

        /**
         * @struct LogInfo
         * @brief Struct for storing log information.
         * 
         * This struct stores log information, such as the file name, function name, line number, etc.
         * 
         * @note This struct is defined within the Easylog.hpp file.
        */
        struct LogInfo
        {
            std::string mColor;
            std::filesystem::path mFile;
            std::string mFunction;
            std::string mLine;
            std::string mDate;
            std::string mTime;
        };

        /**
         * @brief Gets the log information for a log message.
         * 
         * This function gets the log information for a log message.
         * 
         * @param src The source location of the log message.
         * @return The log information.
        */
        LogInfo getLogInfo(const SourceLoc& src);

        /**
         * @brief Fills the base format string with the log information.
         * 
         * This function fills the base format string with the log information.
         * 
         * @param buf The base format string.
         * @param logInfo The log information.
        */
        void fillBaseFormat(std::stringbuf& buf, std::string_view file, std::string_view function, std::string_view line, std::string_view date, std::string_view time);

        /**
         * @brief Formats the log information.
         * 
         * This function formats the log information by replacing the placeholders in the format string with the corresponding log information.
         * 
         * @param logInfo The log information.
         * @return The formatted log information.
        */
        void getFmtLogInfo(std::stringbuf& fmtLogInfo, const LogInfo& logInfo, bool colorize = true);
    } // namespace LogInfo

    /**
     * @namespace LogImpl
     * @brief Namespace containing functions for logging messages.
     * 
     * The LogImpl namespace contains functions for logging messages.
     * It includes functions for logging messages to the console.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     * 
    */
    namespace LogImpl
    {

        namespace Impl
        {
            struct Data
            {
                static std::mutex mtx;
            };
        } // namespace Impl


        void fillLogBuffer(std::stringbuf& buf, LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src, bool file = false);

        /**
         * @brief Logs a message to the console.
         * 
         * This function logs a message to the console.
         * 
         * @param logLevel The log level.
         * @param msg The message to log.
         * @param label The log label.
         * @param src The source location of the log message.
        */
        void log(LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src);
    } // namespace LogImpl

    /**
     * @namespace FileLogInfo
     * @brief Namespace containing structs and functions for getting file log information.
     * 
     * The FileLogInfo namespace contains structs and functions for getting file log information.
     * It includes structs for storing file log information, such as the file name, function name, line number, etc.
     * It also includes functions for getting the file log information and formatting it.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
    */
    namespace FileLogInfo
    {
        /**
         * @struct FileLogInfo
         * @brief Struct for storing file log information.
         * 
         * This struct stores file log information, such as the file name, function name, line number, etc.
         * 
         * @note This struct is defined within the Easylog.hpp file.
        */
        struct FileLogInfo
        {
            std::filesystem::path mFile;
            std::string mFunction;
            std::string mLine;
            std::string mDate;
            std::string mTime;
        };

        /**
         * @brief Gets the file log information for a log message.
         * 
         * This function gets the file log information for a log message.
         * 
         * @param src The source location of the log message.
         * @return The file log information.
        */
        FileLogInfo getFileLogInfo(const SourceLoc& src);
    } // namespace FileLogInfo

    /**
     * @namespace FileLogImpl
     * @brief Namespace containing functions for logging messages to a file.
     * 
     * The FileLogImpl namespace contains functions for logging messages to a file.
     * It includes functions for logging messages to a file.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     * 
    */
    namespace FileLogImpl
    {   

        /**
         * @namespace Impl
         * @brief Namespace containing implementation details for the FileLogImpl namespace.
         * 
         * The Impl namespace contains implementation details for the FileLogImpl namespace.
         * It includes structs for storing the state of the Easylog library.
         * It also includes functions for checking if the library is in a certain state.
         * 
         * @note This namespace is defined within the Easylog.hpp file.
        */
        namespace Impl
        {

            /**
             * @struct FileLogger
             * @brief Struct for storing the information of a file logger.
             * 
             * This struct stores the information of a file logger.
             * It includes the open mode, path, stream, and mutex of the file logger.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
            struct FileLogger
            {
                std::ios_base::openmode mOpenMode;
                std::filesystem::path mPath;
                std::ofstream mStream;
                std::mutex mtx;

                /**
                 * @brief Default constructor.
                */
                FileLogger() = default;

                /**
                 * @brief Constructor that sets the open mode and path of the file logger.
                 * 
                 * @param openMode The open mode of the file logger.
                 * @param path The path of the file logger.
                */
                FileLogger(std::ios_base::openmode openMode, const std::filesystem::path& path);
            };

            /**
             * @struct Data
             * @brief Struct for storing the state of the Easylog library.
             * 
             * This struct stores the state of the Easylog library.
             * It includes static variables for storing the state of the library.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
            struct Data
            {
                static FileLogger DefaultFileLogger;
                static std::unordered_map<std::string, FileLogger, StringHelper::StringHash, std::equal_to<>> FileLoggers;
            };
        } // namespace Impl

        /**
         * @brief Gets the file logger with the specified name.
         * 
         * This function gets the file logger with the specified name.
         * 
         * @param FileLoggerName The name of the file logger.
         * @return The file logger with the specified name.
        */
        Impl::FileLogger& GetFileLogger(std::string_view FileLoggerName);

        /**
         * @brief Logs a message to a file.
         * 
         * This function logs a message to a file.
         * 
         * @param logLevel The log level.
         * @param msg The message to log.
         * @param label The log label.
         * @param src The source location of the log message.
        */
        void log(LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, SourceLoc src);
    } // namespace FileLogImpl

    /**
     * @namespace BufferLogImpl
     * @brief Namespace containing functions for buffering log messages.
     * 
     * The BufferLogImpl namespace contains functions for buffering log messages.
     * It includes functions for buffering log messages.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     * 
    */
    namespace LogBufferImpl
    {

        /**
         * @namespace Impl
         * @brief Namespace containing implementation details for the BufferLogImpl namespace.
         * 
         * The Impl namespace contains implementation details for the BufferLogImpl namespace.
         * It includes structs for storing the state of the Easylog library.
         * It also includes functions for checking if the library is in a certain state.
         * 
         * @note This namespace is defined within the Easylog.hpp file.
        */
        namespace Impl
        {

            /**
             * @struct Data
             * @brief Struct for storing the the information for buffering log messages.
             * 
             * This struct stores the the information for buffering log messages.
             * It includes static variables for storing the the information for buffering log messages.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
            struct Data
            {
                static std::mutex mtx;
                static std::vector<std::string> mLogBuffer;
                static std::vector<std::string> mFileLogBuffer;
                static std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> mLogBufferLabel;
                static std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> mFileLogBufferLabel;
            };
        } // namespace Impl

        /**
         * @brief Logs to a buffer object.
         * 
         * This function logs to a buffer object.
         * 
         * @param logLevel The log level.
         * @param msg The message to log.
         * @param label The log label.
         * @param src The source location of the log message.
        */
        void log(LogLevel::LogLevel level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src);

        /**
         * @brief Logs to a file buffer object.
         * 
         * This function logs to a file buffer object.
         * 
         * @param logLevel The log level.
         * @param msg The message to log.
         * @param label The log label.
         * @param src The source location of the log message.
        */
        void fileLog(LogLevel::LogLevel level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src);
    } // namespace BufferLogImpL

    /**
     * @namespace CLogImpl
     * @brief Namespace containing functions for logging messages.
     * 
     * The CLogImpl namespace contains functions for logging messages.
     * It includes functions for logging messages to the console.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     * 
    */
    namespace CLogImpl
    {
        void log(const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());
    } // namespace CLogImpl

    /**
     * @namespace Log
     * @brief Namespace containing functions for logging messages in a seperate logging thread.
     * 
     * The Log namespace contains functions for logging messages in a seperate logging thread.
     * It includes functions for logging messages in a seperate logging thread.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
    */
    namespace ThreadLog
    {
        /**
         * @namespace Impl
         * @brief Namespace containing implementation details for the ThreadLog namespace.
         * 
         * The Impl namespace contains implementation details for the ThreadLog namespace.
         * It includes structs for storing the state of the Easylog library.
         * It also includes functions for checking if the library is in a certain state.
         * 
         * @note This namespace is defined within the Easylog.hpp file.
        */
        namespace Impl
        {
            /**
             * @struct LogTask
             * @brief Struct for storing the information for a log task.
             * 
             * This struct stores the information for a log task.
             * It includes the log level, log label, log message, and source location of the log task.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
            struct LogTask
            {
                LogUsings::Scope<std::stringbuf> mLogLevel;
                LogUsings::Scope<std::stringbuf> mLabel;
                LogUsings::Scope<std::stringbuf> mMsg;
                LogUsings::Scope<SourceLoc> mSrc;
                LogUsings::Scope<std::vector<Colorize::Colorize>> mColorStack;
            };

            /**
             * @brief Makes a log task.
             * 
             * This function makes a log task.
             * 
             * @param logLevel The log level.
             * @param msg The message to log.
             * @param label The log label.
             * @param src The source location of the log message.
             * @return The log task.
            */
            LogTask makeTask(LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src);

            /**
             * @brief Makes a log task.
             * 
             * This function makes a log task.
             * 
             * @param logLevel The log level.
             * @param msg The message to log.
             * @param colorStack The color stack.
             * @param label The log label.
             * @param src The source location of the log message.
             * @return The log task.
            */
            LogTask makeTask(LogLevel::LogLevel logLevel, std::string_view msg, const std::vector<Colorize::Colorize>& colorStack, LogLabel::Impl::Label label, const SourceLoc& src);

            /**
             * @struct Data
             * @brief Struct for storing the state of the Easylog library.
             * 
             * This struct stores the state of the Easylog library.
             * It includes static variables for storing the state of the library.
             * 
             * @note This struct is defined within the Easylog.hpp file.
            */
            struct Data
            {
                static std::mutex mtx;
                static std::queue<LogTask> mLogTasks;
                static std::condition_variable mCv;
                static std::atomic<bool> mIsRunning;
                static std::atomic<bool> mIsFinished;
                static std::jthread mThread;
            };

            /**
             * @brief The logger thread function.
             * 
             * This function is the logger thread function.
             * 
             * @param stoken The stop token.
            */
            void loggerThreadFunc(std::stop_token stoken);
        } // namespace Impl

        /**
         * @brief Pushes a log task to the log task queue.
         * 
         * This function pushes a log task to the log task queue.
         * 
         * @param task The log task.
        */
        void PushLogTask(Impl::LogTask&& task);

        /**
         * @brief Starts the logger thread.
         * 
         * This function starts the logger thread.
        */
        void StartLoggerThread();

        /**
         * @brief Stops the logger thread.
         * 
         * This function stops the logger thread.
        */
        void StopLoggerThread();

        /**
         * @brief Waits for the logger thread to finish.
         * 
         * This function waits for the logger thread to finish.
        */
        void WaitLoggerThread();

        /**
         * @brief Checks if the logger thread is running.
         * 
         * This function checks if the logger thread is running.
         * 
         * @return A flag indicating whether the logger thread is running.
        */
        bool IsLoggerThreadRunning();

        /**
         * @brief Checks if the logger thread is finished.
         * 
         * This function checks if the logger thread is finished.
         * 
         * @return A flag indicating whether the logger thread is finished.
        */
        bool IsLoggerThreadFinished();
    } // namespace ThreadLog

    /**
     * @namespace State
     * @brief Namespace containing enums and structs for storing the state of the Easylog library.
     *
     * The State namespace contains enums and structs for storing the state of the Easylog library.
     * It also includes functions for checking if the library is in a certain state.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     */
    namespace State
    {

        /**
         * @brief Sets the state of the Easylog library.
         * 
         * This function sets the state of the Easylog library.
         * 
         * @param state The state to set.
         * @param isEnabled A flag indicating whether the state is enabled.
        */
        void SetState(State::StateEnum state, bool isEnabled);

        /**
         * @brief Sets the default file log path.
         * 
         * This function sets the default file log path.
         * 
         * @param path The default file log path.
        */
        void SetDefaultFileLogPath(const std::filesystem::path& path);

        /**
         * @brief Sets the filelogger wich is used for file logging.
         * 
         * This function sets the filelogger wich is used for file logging.
         * 
         * @param FileLoggerName The name of the filelogger.
        */
        void UseFileLogger(std::string_view FileLoggerName);

        /**
         * @brief Adds a new custom file logger.
         * 
         * This function adds a new custom file logger.
         * 
         * @param FileLoggerName The name of the file logger.
         * @param path The path of the file logger.
         * @param openMode The open mode of the file logger.
         * @return A flag indicating whether the file logger was added successfully.
         */
        bool AddCustomFileLogger(std::string_view FileLoggerName, const std::filesystem::path& path, std::ios_base::openmode openMode = std::ios::app);

        /**
         * @brief Gets the log buffer object.
         * 
         * This function gets the log buffer object.
         * 
         * @return The log buffer object.
         */
        std::vector<std::string> GetLogBuffer();

        /**
         * @brief Gets the file log buffer object.
         * 
         * This function gets the file log buffer object.
         * 
         * @return The file log buffer object.
        */
        std::vector<std::string> GetFileLogBuffer();

        /**
         * @brief Gets the log buffer label object.
         * 
         * This function gets the log buffer label object.
         * 
         * @param label The log label.
         * @return The log buffer label object.
        */
        std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> GetLogBufferLabel();

        /**
         * @brief Gets the file log buffer label object.
         * 
         * This function gets the file log buffer label object.
         * 
         * @param label The log label.
         * @return The file log buffer label object.
        */
        std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> GetFileLogBufferLabel();

        /**
         * @brief Gets the log buffer object for a specific log label.
         * 
         * This function gets the log buffer object for a specific log label.
         * 
         * @param label The log label.
         * @return The log buffer object for the specific log label.
        */
        std::vector<std::string> GetLogBufferByLabel(LogLabel::Impl::Label label);

        /**
         * @brief Gets the file log buffer object for a specific log label.
         * 
         * This function gets the file log buffer object for a specific log label.
         * 
         * @param label The log label.
         * @return The file log buffer object for the specific log label.
        */
        std::vector<std::string> GetFileLogBufferByLabel(LogLabel::Impl::Label label);

        /**
         * @brief Clears the LogBuffer.
         * 
         * This function clears the LogBuffer.
        */
        void ClearLogBuffer();

        /**
         * @brief Clears the FileLogBuffer.
         * 
         * This function clears the FileLogBuffer.
        */
        void ClearFileLogBuffer();

        /**
         * @brief Clears the LogBufferLabel.
         * 
         * This function clears the LogBufferLabel.
        */
        void ClearLogBufferLabel();

        /**
         * @brief Clears the FileLogBufferLabel.
         * 
         * This function clears the FileLogBufferLabel.
        */
        void ClearFileLogBufferLabel();

        /**
         * @brief Clears the LogBuffer for a specific log label.
         * 
         * This function clears the LogBuffer for a specific log label.
         * 
         * @param label The log label.
        */
        void ClearLogBufferByLabel(LogLabel::Impl::Label label);

        /**
         * @brief Clears the FileLogBuffer for a specific log label.
         * 
         * This function clears the FileLogBuffer for a specific log label.
         * 
         * @param label The log label.
        */
        void ClearFileLogBufferByLabel(LogLabel::Impl::Label label);

        /**
         * @brief Clears all buffers.
         * 
         * This function clears all buffers.
        */
        void ClearBuffers();

        /**
         * @brief Adds a new log level.
         * 
         * This function adds a new log level.
         * 
         * @param logLevel The log level.
         * @param color The color to use for the log level.
         * @return A flag indicating whether the log level was added successfully.
        */
        bool AddLogLevel(std::string_view logLevel, AsciiColor::ColorEnum color);

        /**
         * @brief Close the stream of a file logger.
         * 
         * This function closes the stream of a file logger. Or all file loggers if no stream is specified.
         * 
         * @param stream The stream to close.
        */
        void CloseStream(std::string_view stream = "");

        /**
         * @brief Sets the default buffer size.
         * 
         * This function sets the default buffer size.
         * 
         * @param size The default buffer size.
         * 
         * @note The default buffer size is 1000.
        */
        void setDefaultBufferSize(std::size_t size);
    } // namespace State

    /**
     * @namespace fmt
     * @brief Namespace containing functions for formatting strings.
     * 
     * The fmt namespace contains functions for formatting strings.
     * It includes functions for formatting strings.
     * 
     * @note This namespace is defined within the Easylog.hpp file.
     */
    namespace fmt
    {

        /**
         * @namespace Impl
         * @brief Namespace containing implementation details for the fmt namespace.
         * 
         * The Impl namespace contains implementation details for the fmt namespace.
         * 
         * @note This namespace is defined within the Easylog.hpp file.
        */
        namespace Impl
        {

            /**
             * @brief Checks if a string is a number.
             * 
             * This function checks if a string is a number.
             * 
             * @param str The string to check.
             * @return A flag indicating whether the string is a number.
            */
            bool IsNumber(std::string_view str);

            /**
             * @brief Converts a string to a hex string.
             * 
             * This function converts a string to a hex string.
             * 
             * @param input The string to convert.
             * @return The hex string.
            */
            std::string stringToHex(const std::string& input);

            /**
             * @brief Gets the arguments as a std::pair<std::tuple, std::size_t>.
             * 
             * This function gets the arguments as a std::pair<std::tuple, std::size_t>.
             * 
             * @param args The arguments.
             * @return The arguments as a std::pair<std::tuple, std::size_t>.
            */
            template<typename... Args>
            std::pair<std::tuple<Args...>, std::size_t> GetArgsAsTuple(Args&& ... args)
            {
                return std::make_pair(std::tuple<Args...>(std::forward<Args>(args)...), sizeof...(Args));
            }

            /**
             * @brief Converts the arguments to a vector of strings.
             * 
             * This function converts the arguments to a vector of strings.
             * 
             * @param vals The vector of strings.
             * @param t The arguments.
            */
            template<std::size_t I = 0, typename... Tp>
            inline typename std::enable_if_t<I == sizeof...(Tp), void>
            ToString([[maybe_unused]] const std::vector<std::string>&, const std::tuple<Tp...>&)
            {
                // This method is intentionally left empty.
            }

            /**
             * @brief Converts the arguments to a vector of strings.
             * 
             * This function converts the arguments to a vector of strings.
             * 
             * @param vals The vector of strings.
             * @param t The arguments.
            */
            template<std::size_t I = 0, typename... Tp>
            inline typename std::enable_if_t<I < sizeof...(Tp), void>
            ToString(std::vector<std::string>& vals, const std::tuple<Tp...>& t)
            {
                auto value = std::get<I>(t);
                std::stringstream ss;
                ss << value;
                vals.emplace_back(ss.str());
                ToString<I + 1, Tp...>(vals, t);
            }

            /**
             * @brief Converts the arguments to a vector of strings.
             * 
             * This function converts the arguments to a vector of strings.
             * 
             * @param args The arguments.
             * @return The vector of strings.
            */
            template<typename... Args>
            std::vector<std::string> ArgsToVector(Args&& ... args)
            {
                std::vector<std::string> vals;
                auto [t, s] = GetArgsAsTuple(std::forward<Args>(args)...);
                ToString(vals, t);
                return vals;
            }
        } // namespace Impl

        /**
         * @brief Formats a string.
         * 
         * This function formats a string.
         * 
         * @param fmt The format string.
         * @param args The arguments.
         * @return The formatted string.
        */
        template<typename... Args>
        std::string Format(std::string_view fmt, Args&& ... args)
        {
            std::vector<std::string> vals = Impl::ArgsToVector(std::forward<Args>(args)...);
            std::string result = fmt.data();
            for (size_t i = 0; i < vals.size(); ++i)
            {
                std::string key = "{" + std::to_string(i) + "}"; 

                if (auto pos = result.find(key); pos != std::string::npos) // value keys
                    result.replace(pos, key.size(), vals[i]);
                else if(auto pos = result.find("{}"); pos != std::string::npos) // empty keys
                    result.replace(pos, 2, vals[i]);
                else if(auto pos = result.find("{ }"); pos != std::string::npos) // empty keys
                    result.replace(pos, 3, vals[i]);
                else if(auto pos = result.find("{:"); pos != std::string::npos) // format keys
                {
                    auto end = result.find("}", pos);
                    auto fmt = result.substr(pos + 2, end - pos);

                    if (fmt.find("d") != std::string::npos) // int
                        result.replace(pos, end - pos + 1, vals[i]);
                    else if (auto p = fmt.find("f"); p != std::string::npos) // float
                    {
                        int precision = 6;
                        if(p + 1 <= fmt.size() && fmt[p + 1] != '}')
                            precision = std::stoi(fmt.substr(p + 1, end));

                        if (auto posPersBegin = vals[i].find('.'); posPersBegin != std::string::npos)
                            vals[i].resize(posPersBegin + precision + 1, '0');

                        result.replace(pos, end - pos + 1, vals[i]);
                    }
                    else if (fmt.find("s") != std::string::npos) // string
                        result.replace(pos, end - pos + 1, vals[i]);
                    else if (auto p = fmt.find("x"); p != std::string::npos) // hex
                    {
                        auto hexVal = Impl::stringToHex(vals[i]);

                        int precision = 0;
                        if(p + 1 <= fmt.size() && fmt[p + 1] != '}')
                            precision = std::stoi(fmt.substr(p + 1, end));

                        if(auto posPersBegin = hexVal.find('x'); posPersBegin != std::string::npos && precision >= 1)
                            hexVal.insert(posPersBegin + 1, precision - hexVal.size() + 2, '0');

                        result.replace(pos, end - pos + 1, hexVal);
                    }
                }
            }
            return result;
        }
    } // namespace fmt

    /**
     * @brief Logs a custom log message.
     * 
     * This function logs a custom log message.
     * 
     * @param logLevel The log level.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a trace log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logTrace(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a debug log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logDebug(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an info log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logInfo(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a warning log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logWarning(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an error log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */

    void logError(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a fatal log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logFatal(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a custom log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param logLevel The log level.
     * @param msg The message to log.
    */
    void logIfCustom(bool condition, const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a trace log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfTrace(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a debug log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfDebug(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an info log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfInfo(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a warning log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfWarning(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an error log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfError(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a fatal log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfFatal(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a custom log message with colorized strings.
     * 
     * This function logs a custom log message with colorized strings.
     * 
     * @param logLevel The log level.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, const std::vector<Colorize::Colorize>& colorStack, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a trace log message with colorized strings.
     * 
     * This function logs a custom log message with colorized strings.
     * 
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logTrace(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a debug log message with colorized strings.
     * 
     * This function logs a custom log message with colorized strings.
     * 
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logDebug(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an info log message with colorized strings.
     * 
     * This function logs a custom log message with colorized strings.
     * 
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logInfo(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a warning log message with colorized strings.
     * 
     * This function logs a custom log message with colorized strings.
     * 
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logWarning(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an error log message with colorized strings.
     * 
     * This function logs a custom log message with colorized strings.
     * 
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logError(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a fatal log message with colorized strings.
     * 
     * This function logs a custom log message with colorized strings.
     * 
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logFatal(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a custom log message with colorized strings if a condition is met.
     * 
     * This function logs a custom log message with colorized strings if a condition is met.
     * 
     * @param condition The condition.
     * @param logLevel The log level.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfCustom(bool condition, const LogLevel::LogLevel& level, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a trace log message with colorized strings if a condition is met.
     * 
     * This function logs a custom log message with colorized strings if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfTrace(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a debug log message with colorized strings if a condition is met.
     * 
     * This function logs a custom log message with colorized strings if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfDebug(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an info log message with colorized strings if a condition is met.
     * 
     * This function logs a custom log message with colorized strings if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfInfo(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a warning log message with colorized strings if a condition is met.
     * 
     * This function logs a custom log message with colorized strings if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfWarning(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs an error log message with colorized strings if a condition is met.
     * 
     * This function logs a custom log message with colorized strings if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings.
     * @param label The log label.
     * @param src The source location of the log message. 
    */
    void logIfError(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());

    /**
     * @brief Logs a fatal log message with colorized strings if a condition is met.
     * 
     * This function logs a custom log message with colorized strings if a condition is met.
     * 
     * @param condition The condition.
     * @param msg The message to log.
     * @param colorizeStrings The colorized strings. 
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfFatal(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current());
} // namespace eLog
