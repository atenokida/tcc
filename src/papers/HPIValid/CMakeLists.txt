cmake_minimum_required(VERSION 3.10)

project(HPIValid)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LINK_FLAGS "")
set(COMPILE_FLAGS "-Wall -Wextra -pedantic")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

find_package(Boost 1.65.0 REQUIRED COMPONENTS program_options)

include_directories(include ${Boost_INCLUDE_DIRS})

set(HEADERS
  include/config.hpp
  include/essential_table.hpp
  include/helper.hpp
  include/HPIValid.hpp
  include/hypergraph.hpp
  include/preprocessing.hpp
  include/PLITable.hpp
  include/result_collector.hpp
  include/result_collector_output.hpp
  include/table.hpp
  include/TreeSearch.hpp
  )

set(SOURCE_FILES
  src/essential_table.cpp
  src/helper.cpp
  src/HPIValid.cpp
  src/hypergraph.cpp
  src/preprocessing.cpp
  src/result_collector.cpp
  src/result_collector_output.cpp
  src/table.cpp
  src/TreeSearch.cpp
  )

set(EXECUTABLE_FILES
  src/executables/minimal.cpp
  src/executables/HPIValid.cpp)

add_library(code STATIC ${LIBRARY_FILES} ${SOURCE_FILES} ${HEADERS})
foreach (file ${EXECUTABLE_FILES})
    get_filename_component(executable ${file} NAME_WE)
    add_executable(${executable} ${file})
    target_link_libraries(${executable} code ${Boost_LIBRARIES})
endforeach ()

