cmake_minimum_required(VERSION 2.8.3)
project(positioning)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11 -march=native -O3 -pthread)

#set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_FLAGS "-std=c++11 -march=native -O3 -pthread")


# set directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)

# add Eigen .h files
include_directories( "/usr/include/eigen3" )

#set(OpenCV_DIR "/home/nvidia/Downloads/opencv-3.1.0/build")
include_directories("/opt/MVS/include")
link_directories("/opt/MVS/lib/64")
link_directories("./libs")
link_libraries(pthread)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages


find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
#  opencv
  roscpp
  rospy
  tf
  nav_msgs
  std_msgs
  message_generation
  actionlib_msgs
  actionlib
)

find_package(OpenCV REQUIRED)
message(STATUS "version: ${OpenCV_VERSION}")

## Generate actions in the 'action' folder
add_action_files(
  FILES
  Move.action
)

add_message_files(
  FILES
  Encoder.msg
)


## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  geometry_msgs   
  std_msgs
  nav_msgs
  actionlib_msgs
)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a exec_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS 
  geometry_msgs
  rospy
  roscpp
  nav_msgs
  std_msgs
  message_runtime
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  ${OpenCV_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

set(THIRD_PARTY_LIBS
    ${OpenCV_LIBS}
    ${catkin_LIBS}
  	#MvCameraControl
)

catkin_install_python(PROGRAMS
    script/transfer.py
    DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

################## dependencies ################
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(src)
