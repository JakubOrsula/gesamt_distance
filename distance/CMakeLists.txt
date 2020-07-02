find_package(JNI)
find_package(Java)
include(UseJava)

add_executable(sample sample.cpp)
target_link_libraries(sample gesamtlib stdc++fs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_library(ProteinDistance SHARED ProteinNativeQScoreDistance.h ProteinNativeQScoreDistance.cpp)
target_include_directories(ProteinDistance PUBLIC ${PROJECT_BINARY_DIR}/distance)
target_include_directories(ProteinDistance PUBLIC ${JNI_INCLUDE_DIRS})
target_link_libraries(ProteinDistance gesamtlib stdc++fs)
install(TARGETS ProteinDistance DESTINATION lib)

add_executable(calculate_matrix calculate_matrix.cpp)
target_include_directories(calculate_matrix PUBLIC ../gesamt_src)
target_link_libraries(calculate_matrix gesamtlib stdc++fs)

add_executable(get_sizes get_sizes.cpp)
target_include_directories(get_sizes PUBLIC ../gesamt_src)
target_link_libraries(get_sizes gesamtlib stdc++fs)

add_executable(chain2bin chain2bin.cpp)
target_link_libraries(chain2bin gesamtlib stdc++)

add_executable(test_parallel test_parallel.cpp protein_distance.h)
target_link_libraries(test_parallel ProteinDistance)

add_jar(java_distance TestJava.java INCLUDE_JARS proteins-1.0.jar ENTRY_POINT TestJava)
add_dependencies(java_distance ProteinDistance)
