set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Measurer INTERFACE)

target_include_directories(Measurer
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(Measurer INTERFACE project_warnings)

if(TARGET project_sanitizers)
    target_link_libraries(Measurer INTERFACE project_sanitizers)
endif()