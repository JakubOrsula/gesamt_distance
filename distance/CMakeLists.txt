find_package(JNI)
find_package(Java)
find_package(pybind11)
include(UseJava)

add_executable(sample sample.cpp)
target_link_libraries(sample gesamtlib stdc++fs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_library(protein_distance protein_distance.cpp protein_distance.h config.h)
target_link_libraries(protein_distance gesamtlib stdc++fs)

add_library(ProteinDistance SHARED ProteinNativeQScoreDistance.h ProteinNativeQScoreDistance.cpp protein_distance.h)
target_include_directories(ProteinDistance PUBLIC ${PROJECT_BINARY_DIR}/distance)
target_include_directories(ProteinDistance PUBLIC ${JNI_INCLUDE_DIRS})
target_link_libraries(ProteinDistance protein_distance mysqlclient)
install(TARGETS ProteinDistance DESTINATION lib)

add_executable(calculate_matrix calculate_matrix.cpp)
target_include_directories(calculate_matrix PUBLIC ../gesamt_src)
target_link_libraries(calculate_matrix gesamtlib stdc++fs)

add_executable(get_sizes get_sizes.cpp)
target_include_directories(get_sizes PUBLIC ../gesamt_src)
target_link_libraries(get_sizes protein_distance gesamtlib stdc++fs)

add_executable(chain2bin chain2bin.cpp)
target_link_libraries(chain2bin gesamtlib stdc++)

add_executable(test_parallel test_parallel.cpp protein_distance.h)
target_link_libraries(test_parallel ProteinDistance)

add_executable(create_binary_archive create_binary_archive.cpp common.h)
target_link_libraries(create_binary_archive gesamtlib)

add_jar(java_distance TestJava.java INCLUDE_JARS proteins-1.0.jar ENTRY_POINT TestJava)
add_dependencies(java_distance ProteinDistance)
install_jar(java_distance DESTINATION lib)
install(FILES proteins-1.0.jar DESTINATION lib)

add_jar(get_candidates GetCandidates.java ENTRY_POINT GetCandidates)
install_jar(get_candidates DESTINATION lib)

pybind11_add_module(python_distance python_distance.cpp common.h)
target_link_libraries(python_distance PRIVATE protein_distance)
install(TARGETS python_distance DESTINATION lib)
