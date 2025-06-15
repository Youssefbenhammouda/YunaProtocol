# Create an executable for the Windows platform.
add_library(WindowsLib SHARED)

# Automatically find all source files in the 'src' directory for this platform.
file(GLOB WINDOWS_SOURCES "src/*.cpp")
file(GLOB WINDOWS_HEADERS "include/*.h" )
target_sources(WindowsLib PRIVATE ${WINDOWS_SOURCES}  ${WINDOWS_HEADERS})

include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3  # or latest release tag
)

FetchContent_MakeAvailable(spdlog)

# Add the platform-specific 'include' directory for its own headers.
target_include_directories(WindowsLib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(WindowsLib PRIVATE spdlog::spdlog)
target_link_libraries(WindowsLib PUBLIC ws2_32)
# Link the Windows executable against our core logic library.
# This makes all the public functions and classes from YunaCore available
# to WindowsApp.
target_link_libraries(WindowsLib PRIVATE YunaCore)