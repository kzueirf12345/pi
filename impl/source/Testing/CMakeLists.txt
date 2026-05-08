add_library(Testing STATIC
    src/Testing.cpp
)

target_include_directories(Testing
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Testing PRIVATE project_warnings)

if(TARGET project_sanitizers)
    target_link_libraries(Testing PRIVATE project_sanitizers)
endif()

target_link_libraries(Testing PRIVATE 
    libs
)

set_target_properties(Testing PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)