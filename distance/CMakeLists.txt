find_package(JNI)
find_package(Java)
include(UseJava)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_library(protein_distance protein_distance.cpp protein_distance.h config.h)
target_link_libraries(protein_distance gesamtlib stdc++fs)

add_library(ProteinDistance SHARED ProteinNativeQScoreDistance.h ProteinNativeQScoreDistance.cpp protein_distance.h
            ProteinNativeQScoreDistanceJOIntegration.cpp
        ProteinNativeQScoreDistanceJOIntegration.h)
target_include_directories(ProteinDistance PUBLIC ${PROJECT_BINARY_DIR}/distance)
target_include_directories(ProteinDistance PUBLIC ${JNI_INCLUDE_DIRS})
target_link_libraries(ProteinDistance protein_distance)
install(TARGETS ProteinDistance DESTINATION lib)

# Check for pybind11
find_package(pybind11 QUIET)

if(pybind11_FOUND)
    message(STATUS "Found pybind11, compiling Python bindings.")
    pybind11_add_module(python_distance python_distance.cpp)
    target_link_libraries(python_distance PRIVATE protein_distance)
    install(TARGETS python_distance DESTINATION lib)
else()
    message(WARNING "pybind11 not found, skipping Python bindings.")
endif()
