cmake_minimum_required( VERSION 3.12 ) 

project( JpDrawApplication )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories( "include" )

# https://stackoverflow.com/questions/31259861/multiple-cmake-prefix-paths/31261919
# this is some information on how to set multiple prefix paths
# Some of these paths are set to local machine config.
# You should adjust your paths here.
#set(CMAKE_PREFIX_PATH "C:\\Qt\\5.15.2\\msvc2019_64\\")
#set(OpenCV_DIR "C:\\opencv\\build")
#set(GTEST_ROOT "C:\\Program Files\\googletest-distribution")

file( GLOB SOURCES "source/*.cpp" "include/*.hpp" )

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# this is dependant on installation path.
# include("/usr/local/lib/cmake/opencv4/OpenCVConfig.cmake")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(GTest REQUIRED)

add_executable( RUN ${SOURCES} )

target_link_libraries( RUN PRIVATE Qt5::Widgets GTest::GTest GTest::Main ${OpenCV_LIBS})
