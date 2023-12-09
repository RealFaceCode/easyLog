# Logging System in C++

This logging system in C++ supports colored output and string replacement. It includes several functions and data structures to facilitate logging with various log levels and customization options.

## Requires

### Development Environment

C++ Compiler: Ensure you have a C++ compiler that supports C++20 or higher for compiling the provided code.

### Libraries and Headers

Standard Libraries: The code utilizes standard C++ libraries such as <iostream>, <string>, <vector>, <cstdint>, <cstring>, <source_location>, <filesystem>, etc. Ensure these standard libraries are available in your development environment.

### Platform Compatibility

Terminal Support: The colored output is based on ANSI escape sequences and requires a terminal/console that supports ANSI color codes (e.g., UNIX-like systems, Windows Command Prompt with ANSI support enabled).

### Dependencies

No External Dependencies: This logging system does not rely on external third-party libraries or dependencies. It is self-contained using standard C++ libraries.

### Compiler Compatibility

C++20 Support: Ensure your compiler supports C++20 or later standards for proper compilation and functionality of the provided code.

## Usage

To use this logging system:

1. Include the necessary header files.
2. Utilize the provided functions and structures to create logs with different levels of severity, colors, and string replacements.

## Example

Here's a simple example of how to use this logging system:

```cpp
int main()
{
    eLog::logDebug("this is a debug message");
    eLog::logInfo("this is a info message");
    eLog::logWarning("this is a warning message");
    eLog::logError("this is a error message");
    eLog::logFatal("this is a fatal message");

    std::cout << std::endl;

    eLog::logDebug("this is a debug message", 
    {
        eLog::Colorize::colorize("debug", eLog::AsciiColor::BOLD_RED),
        eLog::Colorize::colorize("message", eLog::AsciiColor::BOLD_GREEN)
    });
    eLog::logInfo("this is a info message", 
    {
        eLog::Colorize::colorize("info", eLog::AsciiColor::BOLD_RED),
        eLog::Colorize::colorize("message", eLog::AsciiColor::BOLD_GREEN)
    });
    eLog::logWarning("this is a warning message", 
    {
        eLog::Colorize::colorize("warning", eLog::AsciiColor::BOLD_RED),
        eLog::Colorize::colorize("message", eLog::AsciiColor::BOLD_GREEN)
    });
    eLog::logError("this is a error message", 
    {
        eLog::Colorize::colorize("error", eLog::AsciiColor::BOLD_RED),
        eLog::Colorize::colorize("message", eLog::AsciiColor::BOLD_GREEN)
    });
    eLog::logFatal("this is a fatal message", 
    {
        eLog::Colorize::colorize("fatal", eLog::AsciiColor::BOLD_RED),
        eLog::Colorize::colorize("message", eLog::AsciiColor::BOLD_GREEN)
    });

    std::cout << std::endl;

    std::string str = "this is a test string for colorizing the string with the help of the eLog::ColorizedString class";

    eLog::logDebug(str, 
    {
        eLog::Colorize::colorize("string", eLog::AsciiColor::BOLD_GREEN, true),
        eLog::Colorize::colorize("the", eLog::AsciiColor::MAGENTA, true)
    });
    eLog::logInfo(str, 
    {
        eLog::Colorize::colorize("string", eLog::AsciiColor::BOLD_CYAN, true),
        eLog::Colorize::colorize("the", eLog::AsciiColor::RED, true)
    });
    eLog::logWarning(str, 
    {
        eLog::Colorize::colorize("string", eLog::AsciiColor::BOLD_RED, true),
        eLog::Colorize::colorize("the", eLog::AsciiColor::WHITE, true)
    });
    eLog::logError(str, 
    {
        eLog::Colorize::colorize("string", eLog::AsciiColor::BOLD_WHITE, true),
        eLog::Colorize::colorize("the", eLog::AsciiColor::YELLOW, true)
    });
    eLog::logFatal(str, 
    {
        eLog::Colorize::colorize("string", eLog::AsciiColor::BOLD_YELLOW, true),
        eLog::Colorize::colorize("the", eLog::AsciiColor::GREEN, true)
    });

    return 0;
}
```

## Dependencies

This logging system uses standard C++ libraries such as `<iostream>`, `<string>`, `<vector>`, etc. No additional external dependencies are required.

## Contributions and Issues

Contributions and bug reports are welcome! Please feel free to open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
```
