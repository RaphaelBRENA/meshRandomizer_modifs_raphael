cmake_minimum_required(VERSION 2.6)
project(meshRandomizer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -fpermissive")
endif()

# Source files needed from parent directory
set(PARENT_SOURCE_FILES
    ../Point3D
    ../Hexahedron
    ../Prism
    ../Pyramid
    ../Tetrahedron
    ../Element
    ../Metric
    ../MetricJens
    ../math/Matrix
    ../math/Polynom
    ../math/PolyMatrix
    ../math/Vector
)

# Source files from LinesweepRegularization directory
set(meshRandomizer_SOURCE_FILES
    main
)

add_executable(meshRandomizer ${PARENT_SOURCE_FILES} ${meshRandomizer_SOURCE_FILES})