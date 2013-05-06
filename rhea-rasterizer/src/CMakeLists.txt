
set (rhea_VERISON_MAJOR 0)
set (rhea_VERSION_MINOR 1)
configure_file("rhea_config.h.in" "rhea_config.h")

add_executable(rhea-rasterizer main.cpp config.cpp FT.cpp)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

find_package (GTK2 REQUIRED)
if (GTK2_FOUND)
  include_directories(${GTK2_INCLUDE_DIRS})
  target_link_libraries (rhea-rasterizer ${GTK2_LIBRARIES})
endif (GTK2_FOUND)

include_directories(rhea-rasterizer "/usr/include/pango-1.0")
target_link_libraries(rhea-rasterizer pthread pango-1.0 gobject-2.0 gmodule-2.0 gthread-2.0 rt glib-2.0)

