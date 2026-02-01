/*
  _____ _   _  ___  ____  _     ___   ____
 |_   _| | | |/ _ \|  _ \| |   / _ \ / ___|
   | | | |_| | | | | |_) | |  | | | | |  _
   | | |  _  | |_| |  _ <| |__| |_| | |_| |
   |_| |_| |_|\___/|_| \_\_____\___/ \____|

  ThorLog - Logging library for ESP-IDF (no Arduino dependencies)
  Based on ArduinoLog by Thijs Elenbaas
  https://github.com/thijse/Arduino-Log

Licensed under the MIT License <http://opensource.org/licenses/MIT>.

*/
#pragma once

#include <inttypes.h>
#include <stdarg.h>
#include <cstdio>
#include <cstring>

// *************************************************************************
//  Uncomment line below to fully disable logging, and reduce project size
// ************************************************************************
//#define THORLOG_DISABLE_LOGGING

// Base constants for number formatting
#define THORLOG_DEC 10
#define THORLOG_HEX 16
#define THORLOG_BIN 2

// Log levels
#define THORLOG_LEVEL_SILENT  0
#define THORLOG_LEVEL_FATAL   1
#define THORLOG_LEVEL_ERROR   2
#define THORLOG_LEVEL_WARNING 3
#define THORLOG_LEVEL_INFO    4
#define THORLOG_LEVEL_NOTICE  4
#define THORLOG_LEVEL_TRACE   5
#define THORLOG_LEVEL_VERBOSE 6

#define THORLOG_CR "\r"
#define THORLOG_LF "\n"
#define THORLOG_NL "\r\n"
#define THORLOG_VERSION 1_0_0

/**
 * Constrain template function - replaces Arduino's constrain macro
 */
template<typename T>
T thorlog_constrain(T val, T min, T max) {
    return (val < min) ? min : ((val > max) ? max : val);
}

/**
 * ThorPrint - Abstract base class for print output
 * Replaces Arduino's Print class for ESP-IDF compatibility
 */
class ThorPrint {
public:
    virtual ~ThorPrint() = default;
    virtual size_t print(char c) = 0;
    virtual size_t print(const char* str) = 0;
    virtual size_t print(int num, int base = 10) = 0;
    virtual size_t print(unsigned int num, int base = 10) = 0;
    virtual size_t print(long num, int base = 10) = 0;
    virtual size_t print(unsigned long num, int base = 10) = 0;
    virtual size_t print(double num) = 0;
};

typedef void (*printfunction)(ThorPrint*, int);

/**
 * ThorLogging is a minimalistic framework to help the programmer output log statements to an output of choice,
 * fashioned after extensive logging libraries such as log4cpp, log4j and log4net. In case of problems with an
 * application, it is helpful to enable logging so that the problem can be located. ThorLogging is designed so
 * that log statements can remain in the code with minimal performance cost. In order to facilitate this the
 * loglevel can be adjusted, and (if your code is completely tested) all logging code can be compiled out.
 *
 * ---- Wildcards
 *
 * %s   display as string (char*)
 * %c   display as single character
 * %C   display as single character or as hexadecimal value (prefixed by `0x`) if not a printable character
 * %d   display as integer value
 * %l   display as long value
 * %u   display as unsigned long value
 * %x   display as hexadecimal value
 * %X   display as hexadecimal value prefixed by `0x` and leading zeros
 * %b   display as binary number
 * %B   display as binary number, prefixed by `0b`
 * %t   display as boolean value "t" or "f"
 * %T   display as boolean value "true" or "false"
 * %D,%F display as double value
 * %p   display pointer address
 *
 * ---- Loglevels
 *
 * 0 - THORLOG_LEVEL_SILENT     no output
 * 1 - THORLOG_LEVEL_FATAL      fatal errors
 * 2 - THORLOG_LEVEL_ERROR      all errors
 * 3 - THORLOG_LEVEL_WARNING    errors and warnings
 * 4 - THORLOG_LEVEL_INFO       errors, warnings and notices
 * 4 - THORLOG_LEVEL_NOTICE     Same as INFO, kept for backward compatibility
 * 5 - THORLOG_LEVEL_TRACE      errors, warnings, notices, traces
 * 6 - THORLOG_LEVEL_VERBOSE    all
 */

class ThorLogging
{
public:
    /**
     * default Constructor
     */
    ThorLogging()
#ifndef THORLOG_DISABLE_LOGGING
        : _level(THORLOG_LEVEL_SILENT),
          _showLevel(true),
          _logOutput(nullptr)
#endif
    {
    }

    /**
     * Initializing, must be called as first. Note that if you use
     * this variant of Init, you need to initialize the baud rate
     * yourself, if printer happens to be a serial port.
     *
     * \param level - logging levels <= this will be logged.
     * \param output - place that logging output will be sent to.
     * \param showLevel - whether to show the log level prefix.
     * \return void
     */
    void begin(int level, ThorPrint *output, bool showLevel = true)
    {
#ifndef THORLOG_DISABLE_LOGGING
        _level = thorlog_constrain(level, THORLOG_LEVEL_SILENT, THORLOG_LEVEL_VERBOSE);
        _showLevel = showLevel;
        _logOutput = output;
#endif
    }

    /**
     * Set the log level.
     *
     * \param level - The new log level.
     * \return void
     */
    void setLevel(int level)
    {
#ifndef THORLOG_DISABLE_LOGGING
        _level = thorlog_constrain(level, THORLOG_LEVEL_SILENT, THORLOG_LEVEL_VERBOSE);
#endif
    }

    /**
     * Get the log level.
     *
     * \return The current log level.
     */
    int getLevel() const
    {
#ifndef THORLOG_DISABLE_LOGGING
        return _level;
#else
        return THORLOG_LEVEL_SILENT;
#endif
    }

    /**
     * Set whether to show the log level.
     *
     * \param showLevel - true if the log level should be shown for each log
     *                    false otherwise.
     * \return void
     */
    void setShowLevel(bool showLevel)
    {
#ifndef THORLOG_DISABLE_LOGGING
        _showLevel = showLevel;
#endif
    }

    /**
     * Get whether the log level is shown during logging
     *
     * \return true if the log level is be shown for each log
     *         false otherwise.
     */
    bool getShowLevel() const
    {
#ifndef THORLOG_DISABLE_LOGGING
        return _showLevel;
#else
        return false;
#endif
    }

    /**
     * Sets a function to be called before each log command.
     *
     * \param f - The function to be called
     * \return void
     */
    void setPrefix(printfunction f)
    {
#ifndef THORLOG_DISABLE_LOGGING
        _prefix = f;
#endif
    }

    /**
     * clears prefix.
     *
     * \return void
     */
    void clearPrefix()
    {
#ifndef THORLOG_DISABLE_LOGGING
        _prefix = nullptr;
#endif
    }

    /**
     * Sets a function to be called after each log command.
     *
     * \param f - The function to be called
     * \return void
     */
    void setSuffix(printfunction f)
    {
#ifndef THORLOG_DISABLE_LOGGING
        _suffix = f;
#endif
    }

    /**
     * clears suffix.
     *
     * \return void
     */
    void clearSuffix()
    {
#ifndef THORLOG_DISABLE_LOGGING
        _suffix = nullptr;
#endif
    }

    /**
     * Output a fatal error message. Output message contains
     * F: followed by original message
     * Fatal error messages are printed out at
     * loglevels >= THORLOG_LEVEL_FATAL
     *
     * \param msg format string to output
     * \param ... any number of variables
     * \return void
     */
    template <class T, typename... Args>
    void fatal(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_FATAL, false, msg, args...);
#endif
    }

    template <class T, typename... Args>
    void fatalln(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_FATAL, true, msg, args...);
#endif
    }

    /**
     * Output an error message. Output message contains
     * E: followed by original message
     * Error messages are printed out at
     * loglevels >= THORLOG_LEVEL_ERROR
     *
     * \param msg format string to output
     * \param ... any number of variables
     * \return void
     */
    template <class T, typename... Args>
    void error(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_ERROR, false, msg, args...);
#endif
    }

    template <class T, typename... Args>
    void errorln(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_ERROR, true, msg, args...);
#endif
    }

    /**
     * Output a warning message. Output message contains
     * W: followed by original message
     * Warning messages are printed out at
     * loglevels >= THORLOG_LEVEL_WARNING
     *
     * \param msg format string to output
     * \param ... any number of variables
     * \return void
     */
    template <class T, typename... Args>
    void warning(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_WARNING, false, msg, args...);
#endif
    }

    template <class T, typename... Args>
    void warningln(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_WARNING, true, msg, args...);
#endif
    }

    /**
     * Output a notice message. Output message contains
     * N: followed by original message
     * Notice messages are printed out at
     * loglevels >= THORLOG_LEVEL_NOTICE
     *
     * \param msg format string to output
     * \param ... any number of variables
     * \return void
     */
    template <class T, typename... Args>
    void notice(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_NOTICE, false, msg, args...);
#endif
    }

    template <class T, typename... Args>
    void noticeln(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_NOTICE, true, msg, args...);
#endif
    }

    /**
     * Output an info message. Output message contains
     * I: followed by original message
     * Info messages are printed out at
     * loglevels >= THORLOG_LEVEL_INFO
     *
     * \param msg format string to output
     * \param ... any number of variables
     * \return void
     */
    template <class T, typename... Args>
    void info(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_INFO, false, msg, args...);
#endif
    }

    template <class T, typename... Args>
    void infoln(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_INFO, true, msg, args...);
#endif
    }

    /**
     * Output a trace message. Output message contains
     * T: followed by original message
     * Trace messages are printed out at
     * loglevels >= THORLOG_LEVEL_TRACE
     *
     * \param msg format string to output
     * \param ... any number of variables
     * \return void
     */
    template <class T, typename... Args>
    void trace(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_TRACE, false, msg, args...);
#endif
    }

    template <class T, typename... Args>
    void traceln(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_TRACE, true, msg, args...);
#endif
    }

    /**
     * Output a verbose message. Output message contains
     * V: followed by original message
     * Verbose messages are printed out at
     * loglevels >= THORLOG_LEVEL_VERBOSE
     *
     * \param msg format string to output
     * \param ... any number of variables
     * \return void
     */
    template <class T, typename... Args>
    void verbose(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_VERBOSE, false, msg, args...);
#endif
    }

    template <class T, typename... Args>
    void verboseln(T msg, Args... args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        printLevel(THORLOG_LEVEL_VERBOSE, true, msg, args...);
#endif
    }

private:
    void print(const char *format, va_list args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        for (; *format != '\0'; ++format)
        {
            if (*format == '%')
            {
                ++format;
                if (*format == '\0')
                    break;
                printFormat(*format, &args);
            }
            else
            {
                _logOutput->print(*format);
            }
        }
#endif
    }

    void printFormat(const char format, va_list *args)
    {
#ifndef THORLOG_DISABLE_LOGGING
        if (format == '%')
        {
            _logOutput->print('%');
        }
        else if (format == 's')
        {
            const char *s = va_arg(*args, const char *);
            _logOutput->print(s);
        }
        else if (format == 'c')
        {
            int c = va_arg(*args, int);
            _logOutput->print(static_cast<char>(c));
        }
        else if (format == 'C')
        {
            int c = va_arg(*args, int);
            if (c >= 0x20 && c < 0x7F)
            {
                _logOutput->print(static_cast<char>(c));
            }
            else
            {
                _logOutput->print("0x");
                if (c < 0x10)
                {
                    _logOutput->print('0');
                }
                _logOutput->print(static_cast<unsigned int>(c), THORLOG_HEX);
            }
        }
        else if (format == 'd' || format == 'i')
        {
            int d = va_arg(*args, int);
            _logOutput->print(d);
        }
        else if (format == 'l')
        {
            long l = va_arg(*args, long);
            _logOutput->print(l);
        }
        else if (format == 'u')
        {
            unsigned long u = va_arg(*args, unsigned long);
            _logOutput->print(u);
        }
        else if (format == 'x')
        {
            unsigned int x = va_arg(*args, unsigned int);
            _logOutput->print(x, THORLOG_HEX);
        }
        else if (format == 'X')
        {
            unsigned long x = va_arg(*args, unsigned long);
            _logOutput->print("0x");
            // Print leading zeros for 8 hex digits (32-bit)
            if (x < 0x10000000UL) _logOutput->print('0');
            if (x < 0x1000000UL) _logOutput->print('0');
            if (x < 0x100000UL) _logOutput->print('0');
            if (x < 0x10000UL) _logOutput->print('0');
            if (x < 0x1000UL) _logOutput->print('0');
            if (x < 0x100UL) _logOutput->print('0');
            if (x < 0x10UL) _logOutput->print('0');
            _logOutput->print(x, THORLOG_HEX);
        }
        else if (format == 'b')
        {
            unsigned int b = va_arg(*args, unsigned int);
            _logOutput->print(b, THORLOG_BIN);
        }
        else if (format == 'B')
        {
            unsigned int b = va_arg(*args, unsigned int);
            _logOutput->print("0b");
            _logOutput->print(b, THORLOG_BIN);
        }
        else if (format == 't')
        {
            int t = va_arg(*args, int);
            _logOutput->print(t ? "t" : "f");
        }
        else if (format == 'T')
        {
            int t = va_arg(*args, int);
            _logOutput->print(t ? "true" : "false");
        }
        else if (format == 'D' || format == 'F')
        {
            double d = va_arg(*args, double);
            _logOutput->print(d);
        }
        else if (format == 'p')
        {
            // Print pointer address
            void *p = va_arg(*args, void *);
            uintptr_t addr = reinterpret_cast<uintptr_t>(p);
            _logOutput->print("0x");
            // Print as appropriate width for the platform
            _logOutput->print(static_cast<unsigned long>(addr), THORLOG_HEX);
        }
#endif
    }

    template <class T>
    void printLevel(int level, bool cr, T msg, ...)
    {
#ifndef THORLOG_DISABLE_LOGGING
        if (level > _level)
        {
            return;
        }
        if (level < THORLOG_LEVEL_SILENT)
        {
            level = THORLOG_LEVEL_SILENT;
        }

        if (_prefix != nullptr)
        {
            _prefix(_logOutput, level);
        }

        if (_showLevel)
        {
            static const char levels[] = "FEWITV";
            _logOutput->print(levels[level - 1]);
            _logOutput->print(": ");
        }

        va_list args;
        va_start(args, msg);
        print(msg, args);
        va_end(args);

        if (_suffix != nullptr)
        {
            _suffix(_logOutput, level);
        }
        if (cr)
        {
            _logOutput->print(THORLOG_CR);
        }
#endif
    }

#ifndef THORLOG_DISABLE_LOGGING
    int _level;
    bool _showLevel;
    ThorPrint* _logOutput;

    printfunction _prefix = nullptr;
    printfunction _suffix = nullptr;
#endif
};

inline ThorLogging ThorLog;

// *************************************************************************
//  Arduino-Log compatibility aliases (always available for drop-in replacement)
// *************************************************************************

using Logging = ThorLogging;
#define Log ThorLog

// Map old log level names to new ones
#define ARDUINO_LOG_LOG_LEVEL_SILENT  THORLOG_LEVEL_SILENT
#define ARDUINO_LOG_LOG_LEVEL_FATAL   THORLOG_LEVEL_FATAL
#define ARDUINO_LOG_LOG_LEVEL_ERROR   THORLOG_LEVEL_ERROR
#define ARDUINO_LOG_LOG_LEVEL_WARNING THORLOG_LEVEL_WARNING
#define ARDUINO_LOG_LOG_LEVEL_INFO    THORLOG_LEVEL_INFO
#define ARDUINO_LOG_LOG_LEVEL_NOTICE  THORLOG_LEVEL_NOTICE
#define ARDUINO_LOG_LOG_LEVEL_TRACE   THORLOG_LEVEL_TRACE
#define ARDUINO_LOG_LOG_LEVEL_VERBOSE THORLOG_LEVEL_VERBOSE

// Map old macros
#define CR THORLOG_CR
#define LF THORLOG_LF
#define NL THORLOG_NL

// Map old disable logging macro
#ifdef DISABLE_LOGGING
#define THORLOG_DISABLE_LOGGING
#endif
