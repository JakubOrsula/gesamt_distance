add_subdirectory(mmdb2)
add_subdirectory(rvapi)
add_subdirectory(gesamtlib)

add_executable(gesamt
        gesamt.cpp
        gesamt_domains.cpp
        gesamt_domains.h
        gesamt_input.cpp
        gesamt_input.h
        gesamt_multiple.cpp
        gesamt_multiple.h
        gesamt_pairwise.cpp
        gesamt_pairwise.h
        gesamt_sheaf.cpp
        gesamt_sheaf.h
        gesamt_usage.cpp
        gesamt_usage.h
        gesamt_utils.cpp
        gesamt_utils.h
)

target_link_libraries(gesamt gesamtlib pthread z)
