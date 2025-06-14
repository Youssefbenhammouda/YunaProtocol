# Create an executable for the Windows platform.
add_library(ESP82LIB STATIC)

# Automatically find all source files in the 'src' directory for this platform.
file(GLOB ESP82_SOURCES "src/*.cpp")
file(GLOB ESP82_HEADERS "include/*.h" )
target_sources(ESP82LIB PRIVATE ${ESP82_SOURCES}  ${ESP82_HEADERS})

FetchContent_MakeAvailable(spdlog)

# Add the platform-specific 'include' directory for its own headers.
target_include_directories(ESP82LIB
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)



include_directories(../../deps/Arduino/cores/esp8266/
                    ../../deps/Arduino/libraries/ESP8266WiFi/src/
                    ../../deps/Arduino/libraries/ESP8266WebServer/src/
                    ../../deps/Arduino/libraries/ESP8266HTTPClient/src/
                    ../../deps/Arduino/libraries/ESP8266mDNS/src/
                    ../../deps/Arduino/libraries/ESP8266HTTPUpdateServer/src/
                    ../../deps/Arduino/libraries/ESP8266FtpServer/src/
                    ../../deps/Arduino/libraries/ESP8266SSDP/src/
)
target_link_libraries(ESP82LIB PRIVATE YunaCore)