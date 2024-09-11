cmake_minimum_required(VERSION 3.10)
project(dpll)

add_definitions( -g -ggdb -O0 )

set(CMAKE_EXE_LINKER_FLAGS -static)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "D:\\opencv\\mingw-build\\install")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

add_executable(dpll src/main.cpp
        src/dpll.cpp
        src/cnfparser.cpp
        src/Sudoku_to_cnf.cpp
        src/Sudoku_to_cnf.h
        src/cvui.h
)

target_link_libraries(dpll ${OpenCV_LIBS})
