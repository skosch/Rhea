
set (rhea_VERISON_MAJOR 0)
set (rhea_VERSION_MINOR 1)
configure_file("rhea_config.h.in" "rhea_config.h")

add_executable(rhea main.cpp letter.cpp config.cpp FT.cpp)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

find_package (Freetype REQUIRED)
if (FREETYPE_FOUND)
  include_directories(${FREETYPE_INCLUDE_DIRS})
  target_link_libraries (rhea ${FREETYPE_LIBRARIES})
endif (FREETYPE_FOUND)

#find_package (OpenCV REQUIRED)
#if (OpenCV_FOUND)
  include_directories(rhea "/usr/include/opencv2")
  target_link_libraries (rhea m opencv_core opencv_imgproc opencv_highgui opencv_ml)
#else (OpenCV_FOUND)
#  message("Couldnt find OpenCV!")
#endif (OpenCV_FOUND)


