# ThorLog

**A minimalistic logging framework for ESP-IDF (no Arduino dependencies)**

ThorLog is a lightweight logging library designed for ESP32 projects using ESP-IDF. It is a rewrite of [ArduinoLog](https://github.com/thijse/Arduino-Log) by Thijs Elenbaas, modified to remove all Arduino dependencies while maintaining API compatibility.

## Features

- Multiple log levels (Fatal, Error, Warning, Info, Trace, Verbose)
- Formatted string output with multiple variables
- Fixed memory allocation (zero malloc)
- Header-only library (C++17) or separate compilation (C++11/14)
- Drop-in compatible with ArduinoLog API
- MIT License

## Installation

Copy the following files into your ESP-IDF project:

- `thorlog.h` - Main library header
- `thorlog_espidf.h` - ESP-IDF output adapter

## Quick Start

```cpp
#include "thorlog.h"
#include "thorlog_espidf.h"

void app_main() {
    // Initialize with log level and output
    Log.begin(LOG_LEVEL_VERBOSE, &EspIdfOutput);

    // Start logging
    Log.errorln("Error with binary values: %b, %B", 23, 345808);
    Log.warningln("Warning with integer values: %d, %d", 34, 799870);
    Log.infoln("Info with hex values: %x, %X", 21, 348972);
}
```

## Usage

### Initialization

```cpp
Log.begin(int level, ThorPrint* output, bool showLevel = true);
```

### Log Levels

| Level | Constant | Output |
|-------|----------|--------|
| 0 | `LOG_LEVEL_SILENT` | No output |
| 1 | `LOG_LEVEL_FATAL` | Fatal errors |
| 2 | `LOG_LEVEL_ERROR` | All errors |
| 3 | `LOG_LEVEL_WARNING` | Errors and warnings |
| 4 | `LOG_LEVEL_INFO` | Errors, warnings, and info |
| 5 | `LOG_LEVEL_TRACE` | Errors, warnings, info, and traces |
| 6 | `LOG_LEVEL_VERBOSE` | All messages |

### Log Functions

```cpp
Log.fatal(format, ...);     Log.fatalln(format, ...);
Log.error(format, ...);     Log.errorln(format, ...);
Log.warning(format, ...);   Log.warningln(format, ...);
Log.notice(format, ...);    Log.noticeln(format, ...);
Log.info(format, ...);      Log.infoln(format, ...);
Log.trace(format, ...);     Log.traceln(format, ...);
Log.verbose(format, ...);   Log.verboseln(format, ...);
```

The `...ln` variants append a carriage return after the message.

### Format Specifiers

| Specifier | Description |
|-----------|-------------|
| `%s` | String (char*) |
| `%c` | Single character |
| `%C` | Character, or hex if non-printable |
| `%d`, `%i` | Integer (decimal) |
| `%l` | Long (decimal) |
| `%u` | Unsigned long (decimal) |
| `%x` | Hexadecimal (lowercase) |
| `%X` | Hexadecimal with `0x` prefix |
| `%b` | Binary |
| `%B` | Binary with `0b` prefix |
| `%t` | Boolean as `t` or `f` |
| `%T` | Boolean as `true` or `false` |
| `%D`, `%F` | Double/float |
| `%p` | Pointer address |
| `%%` | Literal percent sign |

### Examples

```cpp
Log.fatalln("Fatal error: %s", "out of memory");
Log.errorln("Error code: %x", 0xDEAD);
Log.warningln("Temperature: %d degrees", 85);
Log.infoln("Connected: %T", true);
Log.traceln("Value: %D", 3.14159);
Log.verboseln("Debug: ptr=%p, flags=%b", ptr, flags);
```

### Disable Logging

To compile out all logging code (for production builds), define before including the header:

```cpp
#define DISABLE_LOGGING
#include "thorlog.h"
```

This removes all logging calls and significantly reduces binary size.

### Custom Prefix/Suffix

Add timestamps or other context to log lines:

```cpp
void printTimestamp(ThorPrint* output, int logLevel) {
    // Print timestamp before each log message
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "[%lu] ", (unsigned long)esp_timer_get_time() / 1000);
    output->print(buffer);
}

Log.setPrefix(printTimestamp);
```

## Custom Output Adapters

ThorLog uses the `ThorPrint` interface for output. The included `EspIdfPrint` class outputs to stdout via printf. You can create custom adapters for UART, files, network, etc.:

```cpp
class MyCustomPrint : public ThorPrint {
public:
    size_t print(char c) override {
        // Your implementation
    }
    size_t print(const char* str) override {
        // Your implementation
    }
    size_t print(int num, int base = 10) override {
        // Handle decimal (10), hex (16), binary (2)
    }
    size_t print(unsigned int num, int base = 10) override { ... }
    size_t print(long num, int base = 10) override { ... }
    size_t print(unsigned long num, int base = 10) override { ... }
    size_t print(double num) override { ... }
};
```

## API Compatibility

ThorLog maintains full API compatibility with ArduinoLog. The following names are available:

| ThorLog Name  | ArduinoLog Alias |
|---------------|------------------|
| `ThorLog`     | `Log`            |
| `ThorLogging` | `Logging`        |
| `THORLOG_LEVEL_*` | `ARDUINO_LOG_LOG_LEVEL_*` |

Existing code using ArduinoLog can migrate by changing the include:

```cpp
// Before (Arduino)
#include <ArduinoLog.h>
Log.begin(LOG_LEVEL_INFO, &Serial);

// After (ESP-IDF)
#include "thorlog.h"
#include "thorlog_espidf.h"
Log.begin(LOG_LEVEL_INFO, &EspIdfOutput);
```

### Converting to Thorlog

As Thorlog doesn't use/accept FlashStringHelpers, you will need to remove any `F()` macros you were previously using to wrap formatting strings.

How you do this is very specific to your own programming style, but I found the following two search/replace strings very helpful in VSCode, when enabling case sensitivity and regex:

Log strings without values:
Search: `(Log\.[a-z]+\()F\(([^;%]+)\)\);`
Replace: `$1$2);`

Log strings with values:
Search: `(Log\.[a-z]+\()F\(("[^"]+")\),`
Replace: `$1$2,`



## History

ThorLog is based on [ArduinoLog](https://github.com/thijse/Arduino-Log), originally created by Thijs Elenbaas and based on earlier work by Bernd Klein. It was rewritten to remove Arduino dependencies for use with ESP-IDF.

### Original ArduinoLog Contributors

- [Thijs Elenbaas](https://github.com/thijse) - Original author
- [Bernd Klein](https://github.com/mrRobot62) - Original library
- And [many others](https://github.com/thijse/Arduino-Log/graphs/contributors)

## License

MIT License - See [LICENSE.md](LICENSE.md)
