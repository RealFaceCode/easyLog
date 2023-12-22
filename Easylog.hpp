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
         * @brief Map of ASCII colors with their corresponding escape sequences.
         * 
         * This map contains the ASCII colors with their corresponding escape sequences.
         * It is used to colorize strings using ASCII colors.
         * 
         * @note This map is defined within the Easylog.hpp file.
         */
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

        /**
         * @brief Checks if the current terminal supports ASCII colors.
         * 
         * This function checks if the current terminal supports ASCII colors.
         * 
         * @return true if ASCII colors are supported, false otherwise.
         */
        static bool CheckIfColorIsSupported()
        {
            if (const char* term = std::getenv("TERM"); term == nullptr)
                return false;
            return true;
        }
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
            std::size_t operator()(std::string_view sv) const
            {
                std::hash<std::string_view> hasher;
                return hasher(sv);
            }
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
        bool isCharPunctuationMark(char c)
        {
            if(c == ' ' || c == ',' || c == '.' || c == '!' || c == '?' || c == ';' || c == ':' || c == '\n' || c == '\t')
                return true;
            else
                return false;
        }
    
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
            explicit ColorizedString(std::string_view str) 
            : mStr(str) {}

            /**
             * @brief Sets the context of the string.
             * 
             * @param str The string to set as the context.
            */
            void setContext(std::string_view str) 
            {
                mStr = str;
            }

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
            void setColor(std::string_view strToColorize, AsciiColor::ColorEnum color, bool replaceAllMatching = false)
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

            /**
             * @brief Colorizes the string.
             * 
             * This function colorizes the string by replacing all substrings with their corresponding colorized substrings.
            */
            void colorize() 
            {
                for(const auto& replace : mReplaceStrings)
                {
                    StringHelper::Replace(mStr, replace.mBaseString.view(), replace.mReplaceString.view(), replace.mReplaceAllMatching);
                }
            }

            /**
             * @brief Gets the colorized string.
             * 
             * This function returns the colorized string.
             * 
             * @return The colorized string.
            */
            const std::string& getColorizedString() const 
            {
                return mStr;
            }

            std::string_view view() const
            {
                return mStr;
            }

            /**
             * @brief Prints the colorized string.
             * 
             * This function prints the colorized string to the console.
            */
            void print() const {
                std::cout << getColorizedString() << std::endl;
            }

            /**
             * @brief Overloaded operator for printing the colorized string.
             * 
             * This function overloads the << operator for printing the colorized string to the console.
             * 
             * @param os The output stream.
             * @param colorizedString The colorized string.
             * @return The output stream.
            */
            friend std::ostream& operator<<(std::ostream& os, const ColorizedString& colorizedString)
            {
                os << colorizedString.getColorizedString();
                return os;
            }

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
        Colorize colorize(std::string_view str, AsciiColor::ColorEnum color, bool replaceAllMatching = false)
        {
            return Colorize { 
                .str = std::string(str),
                .color = color,
                .replaceAllMatching = replaceAllMatching
            };
        }

        /**
         * @brief Creates the colorized string based on the colorize information.
         * 
         * This function creates the colorized string based on the colorize information.
         * 
         * @param str The colorized string.
         * @param colorizedStrings The colorize information.
        */
        void createColorizedString(StringHelper::ColorizedString& str, const std::vector<Colorize>& colorizedStrings)
        {
            for(const auto& colorizedString : colorizedStrings)
            {
                str.setColor(colorizedString.str, colorizedString.color, colorizedString.replaceAllMatching);
            }
            str.colorize();
        }
    } // namespace Colorize

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
            bool Data::ThreadedLog = false;

            /**
             * @brief Checks if the library is in the terminal log state.
             * 
             * This function checks if the library is in the terminal log state.
             * 
             * @return true if the library is in the terminal log state, false otherwise.
            */
            bool IsBuffering() {
                return (Data::BufferLog || Data::BufferLogLabel || Data::BufferFileLog || Data::BufferFileLogLabel);
            }
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
            THREADED_LOG
        };
    } // namespace State

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

            std::string_view Data::mLabelColor(AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::BOLD_WHITE));
        } // namespace Impl

        /**
         * @brief Gets the string representation of a log label for terminal logging.
         * 
         * This function gets the string representation of a log label for terminal logging.
         * 
         * @param label The log label.
         * @return The string representation of the log label.
        */
        void getLabelStringLog(std::stringbuf& buf, Impl::Label label)
        {
            std::stringbuf labelString;
            buf.sputc('[');
            buf.sputn(label.data(), label.length());
            buf.sputc(']');
        }
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
            
            std::unordered_map<std::string, AsciiColor::ColorEnum, StringHelper::StringHash, std::equal_to<>> Data::LogLevels = {
                {"TRACE", AsciiColor::ColorEnum::BOLD_CYAN},
                {"DEBUG", AsciiColor::ColorEnum::BOLD_BLUE},
                {"INFO", AsciiColor::ColorEnum::BOLD_GREEN},
                {"WARNING", AsciiColor::ColorEnum::BOLD_YELLOW},
                {"ERROR", AsciiColor::ColorEnum::BOLD_RED},
                {"FATAL", AsciiColor::ColorEnum::BOLD_MAGENTA}
            }; // namespace Impl
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
        void getLogLevelString(std::stringbuf& logLevelString, LogLevel logLevel, bool colorize = true)
        {
            auto it = Impl::Data::LogLevels.find(logLevel);
            if(colorize)
            {
                if(it != Impl::Data::LogLevels.end())
                {
                    auto c = AsciiColor::AsciiColors.at(it->second);
                    logLevelString.sputn(c.data(),c.length());
                    logLevelString.sputn(logLevel.data(), logLevel.length());
                }
                else
                {
                    auto c = AsciiColor::AsciiColors.at(AsciiColor::ColorEnum::BOLD_WHITE);
                    logLevelString.sputn(c.data(), c.length());
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

        /**
         * @brief Fills the base format string with the log information.
         * 
         * This function fills the base format string with the log information.
         * 
         * @param buf The base format string.
         * @param logInfo The log information.
        */
        void fillBaseFormat(std::stringbuf& buf, std::string_view file, std::string_view function, std::string_view line, std::string_view date, std::string_view time)
        {
            buf.sputn("[", 1);
            buf.sputn(date.data(), date.length());
            buf.sputn(" | ", 3);
            buf.sputn(time.data(), time.length());
            buf.sputn(" | ", 3);
            buf.sputn(file.data(), file.length());
            buf.sputn(" | ", 3);
            buf.sputn(function.data(), function.length());
            buf.sputn(" | ", 3);
            buf.sputn(line.data(), line.length());
            buf.sputn("]", 1);
        }

        /**
         * @brief Formats the log information.
         * 
         * This function formats the log information by replacing the placeholders in the format string with the corresponding log information.
         * 
         * @param logInfo The log information.
         * @return The formatted log information.
        */
        void getFmtLogInfo(std::stringbuf& fmtLogInfo, const LogInfo& logInfo, bool colorize = true)
        {
            if(colorize)
                fmtLogInfo.sputn(logInfo.mColor.data(), logInfo.mColor.length());
            fillBaseFormat(fmtLogInfo, logInfo.mFile.filename().string(), logInfo.mFunction, logInfo.mLine, logInfo.mDate, logInfo.mTime);
            if(colorize)
                fmtLogInfo.sputn(AsciiColor::ResetColor.data(), AsciiColor::ResetColor.length());
        }
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

            std::mutex Data::mtx;
        } // namespace Impl


        void fillLogBuffer(std::stringbuf& buf, LogLevel::LogLevel logLevel, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src, bool file = false)
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
                buf.sputc(' ');
            }

            buf.sputn(vFmtLogInfo.data(), vFmtLogInfo.length());
            buf.sputn(" : ", 3);
            buf.sputn(msg.data(), msg.length());
            buf.sputc('\n');
        }

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
                FileLogger(std::ios_base::openmode openMode, const std::filesystem::path& path)
                : mOpenMode(openMode), mPath(path)
                {}
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

            FileLogger Data::DefaultFileLogger = FileLogger{std::ios_base::app, "log.txt"};
            std::unordered_map<std::string, FileLogger, StringHelper::StringHash, std::equal_to<>> Data::FileLoggers;
        } // namespace Impl

        /**
         * @brief Gets the file logger with the specified name.
         * 
         * This function gets the file logger with the specified name.
         * 
         * @param FileLoggerName The name of the file logger.
         * @return The file logger with the specified name.
        */
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

            std::mutex Data::mtx;
            std::vector<std::string> Data::mLogBuffer;
            std::vector<std::string> Data::mFileLogBuffer;
            std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> Data::mLogBufferLabel;
            std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> Data::mFileLogBufferLabel;
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
        void log(LogLevel::LogLevel level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::stringbuf out;
            LogImpl::fillLogBuffer(out, level, msg, label, src);

            if(label == "default")
            {
                if(State::Impl::Data::BufferLog)
                    Impl::Data::mLogBuffer.emplace_back(out.view());
                if(State::Impl::Data::BufferLogLabel)
                    Impl::Data::mLogBufferLabel[std::string(label)].emplace_back(out.view());
            }
            else
            {
                if(State::Impl::Data::BufferLog)
                    Impl::Data::mLogBuffer.emplace_back(out.view());
                if(State::Impl::Data::BufferLogLabel)
                    Impl::Data::mLogBufferLabel[std::string(label)].emplace_back(out.view());
            }
        }

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
        void fileLog(LogLevel::LogLevel level, std::string_view msg, LogLabel::Impl::Label label, const SourceLoc& src)
        {
            std::scoped_lock lock(Impl::Data::mtx);

            std::stringbuf out;
            LogImpl::fillLogBuffer(out, level, msg, label, src);

            if(label == "default")
            {
                if(State::Impl::Data::BufferLog)
                    Impl::Data::mLogBuffer.emplace_back(out.view());
                if(State::Impl::Data::BufferLogLabel)
                    Impl::Data::mLogBufferLabel[std::string(label)].emplace_back(out.view());
            }
            else
            {
                if(State::Impl::Data::BufferLog)
                    Impl::Data::mLogBuffer.emplace_back(out.view());
                if(State::Impl::Data::BufferLogLabel)
                    Impl::Data::mLogBufferLabel[std::string(label)].emplace_back(out.view());
            }
        }
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
        void log(const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
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
                    .mSrc = std::make_unique<SourceLoc>(src)
                };
            }

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
            LogTask makeTask(LogLevel::LogLevel logLevel, std::string_view msg, const std::vector<Colorize::Colorize>& colorStack, LogLabel::Impl::Label label, const SourceLoc& src)
            {
                auto task = makeTask(logLevel, msg, label, src);
                task.mColorStack = std::make_unique<std::vector<Colorize::Colorize>>(colorStack);
                return task;
            }

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
            
            std::mutex Data::mtx;
            std::queue<LogTask> Data::mLogTasks;
            std::condition_variable Data::mCv;
            std::atomic<bool> Data::mIsRunning = false;
            std::atomic<bool> Data::mIsFinished = false;
            std::jthread Data::mThread;

            /**
             * @brief The logger thread function.
             * 
             * This function is the logger thread function.
             * 
             * @param stoken The stop token.
            */
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

        /**
         * @brief Pushes a log task to the log task queue.
         * 
         * This function pushes a log task to the log task queue.
         * 
         * @param task The log task.
        */
        void PushLogTask(Impl::LogTask&& task)
        {
            std::scoped_lock lock(Impl::Data::mtx);
            Impl::Data::mLogTasks.emplace(std::move(task));
            Impl::Data::mCv.notify_one();
        }

        /**
         * @brief Starts the logger thread.
         * 
         * This function starts the logger thread.
        */
        void StartLoggerThread()
        {
            if(!Impl::Data::mIsRunning)
            {
                Impl::Data::mIsRunning = true;
                Impl::Data::mThread = std::jthread(Impl::loggerThreadFunc);
            }
        }

        /**
         * @brief Stops the logger thread.
         * 
         * This function stops the logger thread.
        */
        void StopLoggerThread()
        {
            //std::scoped_lock lock(Impl::Data::mtx);
            if(Impl::Data::mIsRunning)
            {
                Impl::Data::mIsRunning = false;
                Impl::Data::mThread.request_stop();
                Impl::Data::mCv.notify_all();
                Impl::Data::mThread.join();
            }
        }

        /**
         * @brief Waits for the logger thread to finish.
         * 
         * This function waits for the logger thread to finish.
        */
        void WaitLoggerThread()
        {
            if(Impl::Data::mIsRunning)
                Impl::Data::mThread.join();
        }

        /**
         * @brief Checks if the logger thread is running.
         * 
         * This function checks if the logger thread is running.
         * 
         * @return A flag indicating whether the logger thread is running.
        */
        bool IsLoggerThreadRunning()
        {
            return Impl::Data::mIsRunning;
        }

        /**
         * @brief Checks if the logger thread is finished.
         * 
         * This function checks if the logger thread is finished.
         * 
         * @return A flag indicating whether the logger thread is finished.
        */
        bool IsLoggerThreadFinished()
        {
            return Impl::Data::mIsFinished;
        }
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
            }
        }

        /**
         * @brief Sets the default file log path.
         * 
         * This function sets the default file log path.
         * 
         * @param path The default file log path.
        */
        void SetDefaultFileLogPath(const std::filesystem::path& path)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            FileLogImpl::Impl::Data::DefaultFileLogger.mPath = path;
        }

        /**
         * @brief Sets the filelogger wich is used for file logging.
         * 
         * This function sets the filelogger wich is used for file logging.
         * 
         * @param FileLoggerName The name of the filelogger.
        */
        void UseFileLogger(std::string_view FileLoggerName)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            State::Impl::Data::FileLoggerName = FileLoggerName;
        }

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
        bool AddCustomFileLogger(std::string_view FileLoggerName, const std::filesystem::path& path, std::ios_base::openmode openMode = std::ios::app)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            auto [it, success] = FileLogImpl::Impl::Data::FileLoggers.try_emplace(std::string(FileLoggerName), openMode, path);
            return success;
        }

        /**
         * @brief Gets the log buffer object.
         * 
         * This function gets the log buffer object.
         * 
         * @return The log buffer object.
         */
        std::vector<std::string> GetLogBuffer()
        {
            return LogBufferImpl::Impl::Data::mLogBuffer;
        }

        /**
         * @brief Gets the file log buffer object.
         * 
         * This function gets the file log buffer object.
         * 
         * @return The file log buffer object.
        */
        std::vector<std::string> GetFileLogBuffer()
        {
            return LogBufferImpl::Impl::Data::mFileLogBuffer;
        }

        /**
         * @brief Gets the log buffer label object.
         * 
         * This function gets the log buffer label object.
         * 
         * @param label The log label.
         * @return The log buffer label object.
        */
        std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> GetLogBufferLabel()
        {
            return LogBufferImpl::Impl::Data::mLogBufferLabel;
        }

        /**
         * @brief Gets the file log buffer label object.
         * 
         * This function gets the file log buffer label object.
         * 
         * @param label The log label.
         * @return The file log buffer label object.
        */
        std::unordered_map<std::string, std::vector<std::string>, StringHelper::StringHash, std::equal_to<>> GetFileLogBufferLabel()
        {
            return LogBufferImpl::Impl::Data::mFileLogBufferLabel;
        }

        /**
         * @brief Gets the log buffer object for a specific log label.
         * 
         * This function gets the log buffer object for a specific log label.
         * 
         * @param label The log label.
         * @return The log buffer object for the specific log label.
        */
        std::vector<std::string> GetLogBufferByLabel(LogLabel::Impl::Label label)
        {
            if(LogBufferImpl::Impl::Data::mLogBufferLabel.contains(std::string(label)))
                return LogBufferImpl::Impl::Data::mLogBufferLabel[std::string(label)];
            return std::vector<std::string>();
        }

        /**
         * @brief Gets the file log buffer object for a specific log label.
         * 
         * This function gets the file log buffer object for a specific log label.
         * 
         * @param label The log label.
         * @return The file log buffer object for the specific log label.
        */
        std::vector<std::string> GetFileLogBufferByLabel(LogLabel::Impl::Label label)
        {
            if(LogBufferImpl::Impl::Data::mFileLogBufferLabel.contains(std::string(label)))
                return LogBufferImpl::Impl::Data::mFileLogBufferLabel[std::string(label)];
            return std::vector<std::string>();
        }

        /**
         * @brief Adds a new log level.
         * 
         * This function adds a new log level.
         * 
         * @param logLevel The log level.
         * @param color The color to use for the log level.
         * @return A flag indicating whether the log level was added successfully.
        */
        bool AddLogLevel(std::string_view logLevel, AsciiColor::ColorEnum color)
        {
            std::scoped_lock lock(State::Impl::Data::mtx);
            auto [it, success] = LogLevel::Impl::Data::LogLevels.try_emplace(std::string(logLevel), color);
            return success;
        }

        /**
         * @brief Close the stream of a file logger.
         * 
         * This function closes the stream of a file logger. Or all file loggers if no stream is specified.
         * 
         * @param stream The stream to close.
        */
        void CloseStream(std::string_view stream = "")
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
            bool IsNumber(std::string_view str)
            {
                return !str.empty() && std::ranges::all_of(str, ::isdigit);
            }

            /**
             * @brief Converts a string to a hex string.
             * 
             * This function converts a string to a hex string.
             * 
             * @param input The string to convert.
             * @return The hex string.
            */
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
    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        if(State::Impl::Data::ThreadedLog)
            ThreadLog::PushLogTask(ThreadLog::Impl::makeTask(level, msg, label, src));
        else
            CLogImpl::log(level, msg, label, src);
    }

    /**
     * @brief Logs a trace log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logTrace(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("TRACE", msg, label, src);
    }

    /**
     * @brief Logs a debug log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logDebug(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("DEBUG", msg, label, src);
    }

    /**
     * @brief Logs an info log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logInfo(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("INFO", msg, label, src);
    }

    /**
     * @brief Logs a warning log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logWarning(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("WARNING", msg, label, src);
    }

    /**
     * @brief Logs an error log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logError(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("ERROR", msg, label, src);
    }

    /**
     * @brief Logs a fatal log message.
     * 
     * This function logs a custom log message.
     * 
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logFatal(std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("FATAL", msg, label, src);
    }

    /**
     * @brief Logs a custom log message if a condition is met.
     * 
     * This function logs a custom log message if a condition is met.
     * 
     * @param condition The condition.
     * @param logLevel The log level.
     * @param msg The message to log.
     * @param label The log label.
     * @param src The source location of the log message.
    */
    void logIfCustom(bool condition, const LogLevel::LogLevel& level, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        if(condition)
            logCustom(level, msg, label, src);
    }

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
    void logIfTrace(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "TRACE", msg, label, src);
    }

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
    void logIfDebug(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "DEBUG", msg, label, src);
    }

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

    void logIfInfo(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "INFO", msg, label, src);
    }

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

    void logIfWarning(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "WARNING", msg, label, src);
    }

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
    void logIfError(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "ERROR", msg, label, src);
    }

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
    void logIfFatal(bool condition, std::string_view msg, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "FATAL", msg, label, src);
    }

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
    void logCustom(const LogLevel::LogLevel& level, std::string_view msg, const std::vector<Colorize::Colorize>& colorStack, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        if(State::Impl::Data::ThreadedLog)
            ThreadLog::PushLogTask(std::move(ThreadLog::Impl::makeTask(level, msg, colorStack, label, src)));
        else
            CLogImpl::log(level, msg, label, src);
    }

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
    void logTrace(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("TRACE", msg, colorizeStrings, label, src);
    }

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
    void logDebug(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("DEBUG", msg, colorizeStrings, label, src);
    }

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
    void logInfo(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("INFO", msg, colorizeStrings, label, src);
    }

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
    void logWarning(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("WARNING", msg, colorizeStrings, label, src);
    }

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
    void logError(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logCustom("ERROR", msg, colorizeStrings, label, src);
    }

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
    void logFatal(std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {   
        logCustom("FATAL", msg, colorizeStrings, label, src);
    }

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
    void logIfCustom(bool condition, const LogLevel::LogLevel& level, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        if(condition)
            logCustom(level, msg, colorizeStrings, label, src);
    }

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
    void logIfTrace(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "TRACE", msg, colorizeStrings, label, src);
    }

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
    void logIfDebug(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "DEBUG", msg, colorizeStrings, label, src);
    }

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
    void logIfInfo(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {
        logIfCustom(condition, "INFO", msg, colorizeStrings, label, src);
    }

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
    void logIfWarning(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current())
    {   
        logIfCustom(condition, "WARNING", msg, colorizeStrings, label, src);
    }

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
    void logIfError(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current()) 
    {
        logIfCustom(condition, "ERROR", msg, colorizeStrings, label, src);
    }

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
    void logIfFatal(bool condition, std::string_view msg, const std::vector<Colorize::Colorize>& colorizeStrings, LogLabel::Impl::Label label = "default", const SourceLoc src = SourceLoc::current()) 
    {
        logIfCustom(condition, "FATAL", msg, colorizeStrings, label, src);
    }
} // namespace eLog
