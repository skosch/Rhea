
set (rhea_VERISON_MAJOR 0)
set (rhea_VERSION_MINOR 1)
configure_file("rhea_config.h.in" "rhea_config.h")

add_executable(rhea main.cpp letter.cpp config.cpp FT.cpp pair.cpp triplet.cpp)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

find_package (Freetype REQUIRED)
if (FREETYPE_FOUND)
  include_directories(${FREETYPE_INCLUDE_DIRS})
  target_link_libraries (rhea ${FREETYPE_LIBRARIES})
endif (FREETYPE_FOUND)

find_package (GTK2 REQUIRED)
if (GTK2_FOUND)
  include_directories(${GTK2_INCLUDE_DIRS})
  target_link_libraries (rhea ${GTK2_LIBRARIES})
endif (GTK2_FOUND)

include_directories(rhea "/usr/include/pango-1.0")
target_link_libraries(rhea pthread pango-1.0 gobject-2.0 gmodule-2.0 gthread-2.0 rt glib-2.0)


find_package (Boost REQUIRED)
if (BOOST_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries (rhea ${Boost_LIBRARIES})
endif (BOOST_FOUND)




#find_package (OpenCV REQUIRED)
#if (OpenCV_FOUND)
  include_directories(rhea "/usr/include/opencv2")
  target_link_libraries (rhea m opencv_core opencv_imgproc opencv_highgui opencv_ml opencv_features2d opencv_objdetect )
#else (OpenCV_FOUND)
#  message("Couldnt find OpenCV!")
#endif (OpenCV_FOUND)


