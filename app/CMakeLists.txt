add_executable(lightahedron
    main.cpp
)
target_include_directories(lightahedron PUBLIC ${CMAKE_SOURCE_DIR}/libfive/include)
target_link_libraries(lightahedron five)

