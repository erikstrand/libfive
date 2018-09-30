add_executable(gear_generator main.cpp)
target_include_directories(gear_generator PUBLIC ${CMAKE_SOURCE_DIR}/libfive/include)
target_link_libraries(gear_generator five)
target_compile_features(gear_generator PRIVATE cxx_std_17)

