add_library(distance distance.h distance.cpp config.h.in)
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
