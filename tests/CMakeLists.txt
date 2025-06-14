# This script creates a single executable from "main.cpp" in this directory.

# Define the executable target from main.cpp.
add_executable(TestMain main.cpp)

# Link the executable against the YunaCore library.
target_link_libraries(TestMain PRIVATE YunaCore)

# --- Link Platform Specific Headers ---
# Include the correct platform h    eaders based on the target OS.
if(WIN32)
    # If building on Windows, add the Windows include directory.
    target_link_libraries(TestMain PRIVATE WindowsLib)
elseif(UNIX AND NOT APPLE)
    # If building on Linux, add the Linux include directory.
    target_include_directories(TestMain PRIVATE ${CMAKE_SOURCE_DIR}/platforms/Linux/include)
endif()
