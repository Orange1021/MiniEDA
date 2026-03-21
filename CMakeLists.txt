cmake_minimum_required(VERSION 3.16)

project(MiniEDA VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MINIEDA_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(MINIEDA_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(MINIEDA_ENABLE_SANITIZERS "Enable runtime sanitizers (GCC/Clang)" OFF)
set(MINIEDA_SANITIZERS "address;undefined" CACHE STRING "Sanitizers to enable when MINIEDA_ENABLE_SANITIZERS=ON")
option(MINIEDA_ENABLE_TESTING "Enable unit tests with GoogleTest" OFF)
option(MINIEDA_ENABLE_OPENMP "Enable OpenMP parallel runtime support" OFF)

set(MINIEDA_COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/mini_placement
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/mini_router
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/mini_sta
)

set(MINIEDA_CORE_SOURCES
    lib/src/app_config.cpp
    lib/src/arg_parser.cpp
    lib/src/cell.cpp
    lib/src/csv_exporter.cpp
    lib/src/hpwl_calculator.cpp
    lib/src/lef_parser.cpp
    lib/src/lef_pin_mapper.cpp
    lib/src/liberty.cpp
    lib/src/liberty_parser.cpp
    lib/src/liberty_pin_mapper.cpp
    lib/src/net.cpp
    lib/src/netlist_db.cpp
    lib/src/placer_db.cpp
    lib/src/steiner_tree.cpp
    lib/src/verilog_parser.cpp
    apps/mini_sta/cell_mapper.cpp
    apps/mini_sta/delay_model.cpp
    apps/mini_sta/sta_engine.cpp
    apps/mini_sta/timing_checks.cpp
    apps/mini_sta/timing_constraints.cpp
    apps/mini_sta/timing_graph.cpp
    apps/mini_sta/timing_path.cpp
    apps/mini_sta/timing_report.cpp
    apps/mini_placement/placer_engine.cpp
    apps/mini_placement/macro_mapper.cpp
    apps/mini_placement/poisson_solver.cpp
    apps/mini_placement/global_placer.cpp
    apps/mini_placement/placement_interface.cpp
    apps/mini_placement/detailed_placer.cpp
    apps/mini_placement/legalizer.cpp
    apps/mini_placement/greedy_legalizer.cpp
    apps/mini_placement/abacus_legalizer.cpp
    apps/mini_placement/density_grid.cpp
    apps/mini_placement/overlap_detector.cpp
    apps/mini_router/routing_grid.cpp
    apps/mini_router/maze_router.cpp
    apps/mini_router/routing_interface.cpp
)

add_library(minieda_core STATIC ${MINIEDA_CORE_SOURCES})

target_include_directories(minieda_core PUBLIC ${MINIEDA_COMMON_INCLUDE_DIRS})

if(MINIEDA_ENABLE_STRICT_WARNINGS)
    if(MSVC)
        target_compile_options(minieda_core PRIVATE /W4 /permissive-)
    else()
        target_compile_options(minieda_core PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if(MINIEDA_WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(minieda_core PRIVATE /WX)
    else()
        target_compile_options(minieda_core PRIVATE -Werror)
    endif()
endif()

target_compile_definitions(minieda_core PRIVATE
    $<$<CONFIG:Debug>:DEBUG;DEBUG_LOG_LEVEL=1>
    $<$<CONFIG:Release>:NDEBUG;DEBUG_LOG_LEVEL=1>
)

if(MINIEDA_ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(minieda_core PUBLIC OpenMP::OpenMP_CXX)
        target_compile_definitions(minieda_core PUBLIC MINIEDA_USE_OPENMP=1)
        message(STATUS "OpenMP enabled for MiniEDA targets")
    else()
        message(WARNING "MINIEDA_ENABLE_OPENMP=ON but OpenMP was not found; building without OpenMP.")
    endif()
endif()

add_executable(mini_flow apps/main_flow.cpp)
target_link_libraries(mini_flow PRIVATE minieda_core)
target_include_directories(mini_flow PRIVATE ${MINIEDA_COMMON_INCLUDE_DIRS})

if(MINIEDA_ENABLE_SANITIZERS)
    if(MSVC)
        message(WARNING "Sanitizers are not configured for MSVC in this project.")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        string(REPLACE ";" "," MINIEDA_SANITIZERS_CSV "${MINIEDA_SANITIZERS}")
        set(MINIEDA_SANITIZER_FLAG "-fsanitize=${MINIEDA_SANITIZERS_CSV}")
        set(MINIEDA_SANITIZER_EXTRA_FLAGS "")
        if(MINIEDA_SANITIZERS_CSV MATCHES "(^|,)address(,|$)")
            list(APPEND MINIEDA_SANITIZER_EXTRA_FLAGS -fno-sanitize=leak)
        endif()

        target_compile_options(minieda_core PRIVATE
            -fno-omit-frame-pointer
            ${MINIEDA_SANITIZER_FLAG}
            ${MINIEDA_SANITIZER_EXTRA_FLAGS}
        )
        target_compile_options(mini_flow PRIVATE
            -fno-omit-frame-pointer
            ${MINIEDA_SANITIZER_FLAG}
            ${MINIEDA_SANITIZER_EXTRA_FLAGS}
        )
        target_link_options(mini_flow PRIVATE
            ${MINIEDA_SANITIZER_FLAG}
            ${MINIEDA_SANITIZER_EXTRA_FLAGS}
        )

        message(STATUS "Sanitizers enabled: ${MINIEDA_SANITIZERS_CSV}")
    else()
        message(WARNING "Unknown compiler for sanitizer support: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

set_target_properties(mini_flow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(MINIEDA_ENABLE_TESTING)
    include(CTest)
    include(FetchContent)

    # Keep GoogleTest dependency small and focused on gtest/gtest_main.
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(minieda_unit_tests
        tests/netlist_db_test.cpp
        tests/verilog_parser_test.cpp
        tests/liberty_delay_model_test.cpp
        tests/timing_graph_sta_test.cpp
        tests/routing_test.cpp
        tests/placement_test.cpp
        tests/lef_mapper_test.cpp
        tests/mini_flow_smoke_test.cpp
    )
    target_link_libraries(minieda_unit_tests
        PRIVATE
            minieda_core
            GTest::gtest_main
    )
    target_include_directories(minieda_unit_tests PRIVATE ${MINIEDA_COMMON_INCLUDE_DIRS})

    add_test(NAME minieda_unit_tests COMMAND $<TARGET_FILE:minieda_unit_tests>)

    set_target_properties(minieda_unit_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

if(MINIEDA_ENABLE_TESTING)
    message(STATUS "Configured MiniEDA CMake build with GoogleTest unit targets")
else()
    message(STATUS "Configured MiniEDA CMake build without unit test targets")
endif()
