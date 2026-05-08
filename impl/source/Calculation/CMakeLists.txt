add_library(Calculation STATIC
    src/MonteCarlo.cpp
    src/Rand.cpp
    src/Chudnovsky.cpp
    src/GaussLegendre.cpp
)

target_include_directories(Calculation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Calculation PRIVATE project_warnings)

if(TARGET project_sanitizers)
    target_link_libraries(Calculation PRIVATE project_sanitizers)
endif()

target_link_libraries(Calculation PRIVATE 
    libs
    Measurer
)

set_target_properties(Calculation PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)