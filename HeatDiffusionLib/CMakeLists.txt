project (HeatDiffusion)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

if(NOT MSVC)
    include(FindPkgConfig)
    find_package(Eigen REQUIRED)

else()
    set(EIGEN_INCLUDE_DIR "" CACHE PATH "")
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_definitions("-DEIGEN_NO_DEBUG")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../SimpleMesh/include)
include_directories(${EIGEN_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Third-party/arpack++/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Third-party/nanoflann-1.1.7/include)

if(MSVC)
    link_directories(${BLAS_DIR})
    link_directories(${ARPACK_DIR})
    link_directories(${SUPERLU_DIR})

    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../build)

endif()

add_library(HeatDiffusion SHARED src/Laplacian.cpp src/HeatKernelSignature.cpp src/WaveKernelSignature.cpp src/DiffusionDistance.cpp src/ConmuteTimeDistance.cpp src/BiharmonicDistance.cpp)

if(MSVC)
    target_link_libraries(HeatDiffusion SimpleMesh ${ARPACK_LIB} ${BLAS_LIB} ${SUPERLU_LIB})
endif()
