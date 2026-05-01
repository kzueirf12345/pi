add_library(CliParser STATIC
    src/CliParser.cpp
)

target_include_directories(CliParser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(CliParser PRIVATE project_warnings)

if(TARGET project_sanitizers)
    target_link_libraries(CliParser PRIVATE project_sanitizers)
endif()

set_target_properties(CliParser PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)