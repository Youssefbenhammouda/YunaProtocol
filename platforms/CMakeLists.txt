# This CMakeLists.txt file includes the correct subdirectory based on the
# operating system CMake is generating build files for.

if(WIN32)
    # If the target system is Windows (WIN32 is a CMake variable that is true
    # on all Windows systems, including 64-bit).
    message(STATUS "Configuring for Windows platform.")
    add_subdirectory(Windows)
    add_subdirectory(Arduino)
elseif(UNIX AND NOT APPLE)
    # If the target system is Unix-like (but not macOS), we assume Linux.
    message(STATUS "Configuring for Linux platform.")
    add_subdirectory(Linux)
else()
    # Provide a message for unsupported systems.
    message(WARNING "No supported platform (Windows or Linux) detected. Skipping platform-specific targets.")
endif()

# Note: The Arduino directory is intentionally ignored and will not be built.