set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(libs INTERFACE)

target_include_directories(libs
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(libs INTERFACE project_warnings mpfr gmp)

if(TARGET project_sanitizers)
    target_link_libraries(libs INTERFACE project_sanitizers)
endif()