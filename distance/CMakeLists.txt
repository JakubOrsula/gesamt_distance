find_package(JNI)
find_package(Java)
find_package(pybind11)
include(UseJava)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_library(protein_distance protein_distance.cpp protein_distance.h config.h)
target_link_libraries(protein_distance gesamtlib stdc++fs)

add_library(ProteinDistance SHARED ProteinNativeQScoreDistance.h ProteinNativeQScoreDistance.cpp protein_distance.h)
target_include_directories(ProteinDistance PUBLIC ${PROJECT_BINARY_DIR}/distance)
target_include_directories(ProteinDistance PUBLIC ${JNI_INCLUDE_DIRS})
target_link_libraries(ProteinDistance protein_distance)
install(TARGETS ProteinDistance DESTINATION lib)

add_executable(create_binary_archive create_binary_archive.cpp)
target_link_libraries(create_binary_archive gesamtlib)

pybind11_add_module(python_distance python_distance.cpp)
target_link_libraries(python_distance PRIVATE protein_distance)
install(TARGETS python_distance DESTINATION lib)
