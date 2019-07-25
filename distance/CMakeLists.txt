add_library(distance distance.h distance.cpp config.h.in test_length.cpp calculate_matrix.cpp)
target_include_directories(distance PUBLIC ../gesamt_src)
target_include_directories(distance PUBLIC ${PROJECT_BINARY_DIR}/distance)

target_link_libraries(distance gesamtlib stdc++fs)

find_package(JNI)
find_package(Java)
include(UseJava)

configure_file(config.h.in config.h)

add_library(ProteinDistance SHARED ProteinDistance.h ProteinDistance.cpp)
target_include_directories(ProteinDistance PUBLIC ${JNI_INCLUDE_DIRS})
target_link_libraries(ProteinDistance distance pthread z stdc++fs)

add_executable(test_distance test_distance.cpp)
target_link_libraries(test_distance distance)

add_jar(java_distance ProteinDistance.java ENTRY_POINT ProteinDistance)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(test_length test_length.cpp)
target_include_directories(test_length PUBLIC ../gesamt_src)
target_link_libraries(test_length gesamtlib stdc++fs)

add_executable(calculate_matrix calculate_matrix.cpp)
target_include_directories(calculate_matrix PUBLIC ../gesamt_src)
target_link_libraries(calculate_matrix gesamtlib stdc++fs -static)

add_executable(get_sizes get_sizes.cpp)
target_include_directories(get_sizes PUBLIC ../gesamt_src)
target_link_libraries(get_sizes gesamtlib stdc++fs -static)
