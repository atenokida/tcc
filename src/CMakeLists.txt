cmake_minimum_required(VERSION 3.14)
project(CardinalityEstimation VERSION 1.0)

# Build settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies configuration
if(NOT DEFINED VCPKG_ROOT)
    set(VCPKG_ROOT $ENV{HOME}/vcpkg)
endif()
set(VCPKG_INSTALLED ${VCPKG_ROOT}/installed/x64-linux)

# Required packages
find_package(xtensor REQUIRED)
find_package(Boost REQUIRED)

# Source files
set(SOURCES
    main.cc
    cardinality_estimation/predicate.cc
    cardinality_estimation/simple_profile.cc
    cardinality_estimation/tug_of_war.cc
    cardinality_estimation/table.cc
    cardinality_estimation/utils.cc
    experiment/e_simple_profile.cc
    experiment/e_tug_of_war.cc
)

# Target configuration
add_executable(cardinality_estimator ${SOURCES})

# Include directories
target_include_directories(cardinality_estimator
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/cardinality_estimation
        ${CMAKE_SOURCE_DIR}/experiment
        ${VCPKG_INSTALLED}/include
        ${Boost_INCLUDE_DIRS}
)

# Link directories
target_link_directories(cardinality_estimator
    PRIVATE
        ${VCPKG_INSTALLED}/lib
)

# Link dependencies
target_link_libraries(cardinality_estimator
    PRIVATE
        xtensor
        Boost::boost
)