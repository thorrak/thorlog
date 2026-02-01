/*
  _____ _   _  ___  ____  _     ___   ____
 |_   _| | | |/ _ \|  _ \| |   / _ \ / ___|
   | | | |_| | | | | |_) | |  | | | | |  _
   | | |  _  | |_| |  _ <| |__| |_| | |_| |
   |_| |_| |_|\___/|_| \_\_____\___/ \____|

 ThorLog ESP-IDF Example
 Licensed under the MIT License <http://opensource.org/licenses/MIT>.

 This example demonstrates basic usage of ThorLog with ESP-IDF.
*/

#include "thorlog.h"
#include "thorlog_espidf.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Example: Custom prefix function to add timestamps
void printTimestamp(ThorPrint* output, int logLevel) {
    int64_t time_ms = esp_timer_get_time() / 1000;
    char buffer[24];
    snprintf(buffer, sizeof(buffer), "[%7lld] ", (long long)time_ms);
    output->print(buffer);
}

extern "C" void app_main(void) {
    // Initialize logging with verbose level
    Log.begin(THORLOG_LEVEL_VERBOSE, &EspIdfOutput);

    // Basic logging examples
    Log.noticeln("******************************************");
    Log.noticeln("***       ThorLog ESP-IDF Example      ***");
    Log.noticeln("******************************************");

    // Integer values
    int intValue1 = 42;
    int intValue2 = 12345;
    Log.infoln("Integer values: %d, %d", intValue1, intValue2);

    // Hexadecimal values
    Log.infoln("Hex values: %x, %X", 0xDEAD, 0xBEEF);

    // Binary values
    Log.infoln("Binary values: %b, %B", 0b1010, 0b11110000);

    // Long values
    long longValue = 1234567890L;
    Log.infoln("Long value: %l", longValue);

    // Unsigned long values
    unsigned long ulongValue = 4000000000UL;
    Log.infoln("Unsigned long value: %u", ulongValue);

    // Boolean values
    bool boolTrue = true;
    bool boolFalse = false;
    Log.infoln("Boolean values: %t/%T, %t/%T", boolTrue, boolTrue, boolFalse, boolFalse);

    // String values
    const char* stringValue = "Hello, ThorLog!";
    Log.infoln("String value: %s", stringValue);

    // Float/Double values
    double doubleValue = 3.14159265;
    Log.infoln("Double value: %D", doubleValue);

    // Character values
    char charValue = 'A';
    char nonPrintable = 0x07;  // Bell character
    Log.infoln("Character values: %c, %C", charValue, nonPrintable);

    // Pointer values
    void* ptr = &intValue1;
    Log.infoln("Pointer value: %p", ptr);

    // Different log levels
    Log.fatalln("This is a FATAL message");
    Log.errorln("This is an ERROR message");
    Log.warningln("This is a WARNING message");
    Log.infoln("This is an INFO message");
    Log.noticeln("This is a NOTICE message");
    Log.traceln("This is a TRACE message");
    Log.verboseln("This is a VERBOSE message");

    // Using prefix for timestamps
    Log.noticeln("Enabling timestamp prefix...");
    Log.setPrefix(printTimestamp);
    Log.infoln("Message with timestamp");
    Log.infoln("Another message with timestamp");
    Log.clearPrefix();
    Log.infoln("Message without timestamp");

    // Demonstrate log level filtering
    Log.noticeln("Setting log level to WARNING...");
    Log.setLevel(THORLOG_LEVEL_WARNING);
    Log.verboseln("This VERBOSE message will NOT appear");
    Log.infoln("This INFO message will NOT appear");
    Log.warningln("This WARNING message WILL appear");
    Log.errorln("This ERROR message WILL appear");

    // Reset to verbose
    Log.setLevel(THORLOG_LEVEL_VERBOSE);
    Log.noticeln("Log level reset to VERBOSE");

    // Main loop
    int counter = 0;
    while (true) {
        Log.traceln("Loop iteration: %d", counter++);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
