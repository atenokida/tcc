cmake_minimum_required(VERSION 3.14)
project(DataFrameTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Direct paths to vcpkg installation
set(VCPKG_ROOT ~/vcpkg)
set(VCPKG_INSTALLED ${VCPKG_ROOT}/installed/x64-linux)

add_executable(test main.cc
                   cardinality_estimation/predicate.cc
                   cardinality_estimation/simple_profile.cc
                   cardinality_estimation/tug_of_war.cc
                   cardinality_estimation/table.cc
                   cardinality_estimation/utils.cc
                   experiment/e_simple_profile.cc
                   experiment/e_tug_of_war.cc)
target_include_directories(test PRIVATE ${VCPKG_INSTALLED}/include)
target_link_directories(test PRIVATE ${VCPKG_INSTALLED}/lib)

# Link against the DataFrame library (adjust name as needed)
target_link_libraries(test DataFrame)  # or DataFrame, depending on the actual lib name

# Link against the xtensor library
find_package(xtensor REQUIRED)
target_link_libraries(test xtensor)

find_package(TBB REQUIRED)
target_link_libraries(test TBB::tbb)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/cardinality_estimation)
include_directories(${CMAKE_SOURCE_DIR}/experiment)