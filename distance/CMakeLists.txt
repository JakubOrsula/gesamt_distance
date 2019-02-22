add_library(distance distance.h distance.cpp)
target_include_directories(distance PUBLIC ../gesamt_src)

target_link_libraries(distance gesamtlib stdc++fs)

find_package(JNI)

add_library(ProteinDistance SHARED ProteinDistance.h ProteinDistance.cpp)
target_include_directories(ProteinDistance PUBLIC ${JNI_INCLUDE_DIRS})
target_link_libraries(ProteinDistance distance pthread z stdc++fs)

add_executable(test test.cpp)
target_link_libraries(test distance)
