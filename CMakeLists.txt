cmake_minimum_required(VERSION 3.10)
project(easyprint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCV
find_package(OpenCV REQUIRED CONFIG)

# Cairo
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo)

add_executable(easyprint main.cpp)

target_include_directories(easyprint PRIVATE ${CAIRO_INCLUDE_DIRS})
target_link_libraries(easyprint
    PRIVATE
    ${OpenCV_LIBS}
    ${CAIRO_LIBRARIES}
)
