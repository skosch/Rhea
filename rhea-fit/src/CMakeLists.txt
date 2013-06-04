
set (rhea_VERISON_MAJOR 0)
set (rhea_VERSION_MINOR 1)
configure_file("rhea_config.h.in" "rhea_config.h")

add_executable(rhea-fit main.cpp letter.cpp pair.cpp)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

find_package(OpenCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIRS})
target_link_libraries(rhea-fit ${OPENCL_LIBRARIES})

