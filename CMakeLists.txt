cmake_minimum_required(VERSION 3.10)
project(easyprint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCV
find_package(OpenCV REQUIRED CONFIG)

find_package(PkgConfig REQUIRED)

# Cairo
pkg_check_modules(CAIRO REQUIRED cairo)

# Poppler
pkg_check_modules(POPPLER_CPP REQUIRED poppler-cpp)

add_executable(easyprint main.cpp)

target_include_directories(easyprint
    PRIVATE
    ${OpenCV_LIBS}
    ${CAIRO_INCLUDE_DIRS}
    ${POPPLER_INCLUDE_DIRS}
)
target_link_libraries(easyprint
    PRIVATE
    ${OpenCV_LIBS}
    ${CAIRO_LIBRARIES}
    ${POPPLER_CPP_LIBRARIES}
)
target_compile_options(easyprint
    PRIVATE
    ${CAIRO_CFLAGS_OTHER}
    ${POPPLER_CFLAGS_OTHER}
)
