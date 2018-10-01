cmake_minimum_required(VERSION 3.9.4)

find_package(PNG REQUIRED)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

add_executable(gear_generator main.cpp gear_generator.h gear_generator.cpp png_writer.h png_writer.cpp)
target_include_directories(gear_generator PUBLIC ${CMAKE_SOURCE_DIR}/libfive/include ${PNG_INCLUDE_DIR})
target_link_libraries(gear_generator five ${PNG_LIBRARY})
target_compile_features(gear_generator PRIVATE cxx_std_17)

if (supported)
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET gear_generator PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()
