# Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)
# All Rights Reserved.
# Authors: Jason Chevrie <jason.chevrie@iit.it>

cmake_minimum_required(VERSION 3.0.0)
project(grasp-scenario-setup)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
  endif()
endif()

find_package(ICUBcontrib REQUIRED)
find_package(YARP REQUIRED COMPONENTS OS sig cv)
find_package(OpenCV REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})

include(ICUBcontribHelpers)
include(ICUBcontribOptions)
icubcontrib_set_default_prefix()

set(sources src/main.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)
target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES} ${OpenCV_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
icubcontrib_add_uninstall_target()

file(GLOB scripts ${CMAKE_SOURCE_DIR}/app/scripts/*.template)
yarp_install(FILES ${scripts} DESTINATION ${ICUBCONTRIB_APPLICATIONS_TEMPLATES_INSTALL_DIR})

file(GLOB ini ${CMAKE_SOURCE_DIR}/app/conf/*.ini)
yarp_install(FILES ${ini} DESTINATION ${ICUBCONTRIB_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})

file(GLOB xml ${CMAKE_SOURCE_DIR}/app/scripts/*.xml)
yarp_install(FILES ${xml} DESTINATION ${ICUBCONTRIB_APPLICATIONS_INSTALL_DIR})

file(GLOB image ${CMAKE_SOURCE_DIR}/data/*.ppm)
yarp_install(FILES ${image} DESTINATION ${ICUBCONTRIB_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})
