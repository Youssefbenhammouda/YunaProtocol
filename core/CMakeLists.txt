# Create a library for the core functionality
add_library(YunaCore)

# Add source files to the library
file(GLOB CORE_SOURCES "src/*.cpp")
file(GLOB CORE_HEADERS "include/*.h")
target_sources(YunaCore PRIVATE ${CORE_SOURCES} ${CORE_HEADERS})
# Expose the 'include' directory for other modules
target_include_directories(YunaCore
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Handle dependencies for the core library if needed
# For example, if your core library uses esp8266 libraries:
# target_include_directories(YunaCore
#     PRIVATE
#         ${CMAKE_SOURCE_DIR}/deps/esp8266/avr
#         ${CMAKE_SOURCE_DIR}/deps/esp8266/libb64
# )
# target_link_libraries(YunaCore
#     PRIVATE
#         # Link against dependency libraries if they are built with CMake
# )